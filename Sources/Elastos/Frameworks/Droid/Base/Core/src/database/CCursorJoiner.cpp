
#include "ext/frameworkdef.h"
#include "database/CCursorJoiner.h"
#include <elastos/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {

CCursorJoiner::CCursorJoiner()
    : mCompareResultIsValid(FALSE)
{}

AutoPtr<ArrayOf<Int32> > CCursorJoiner::BuildColumnIndiciesArray(
    /* [in] */ ICursor* cursor,
    /* [in] */ const ArrayOf<String>& columnNames)
{
    AutoPtr< ArrayOf<Int32> > columns = ArrayOf<Int32>::Alloc(columnNames.GetLength());
    for (Int32 i = 0; i < columnNames.GetLength(); i++) {
        cursor->GetColumnIndexOrThrow(columnNames[i], &(*columns)[i]);
    }
    return columns;
}

ECode CCursorJoiner::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean r1, r2;
    if(mCompareResultIsValid) {
        switch(mCompareResult) {
            case CursorJoinerResult_BOTH:
                mCursorLeft->IsLast(&r1);
                mCursorRight->IsLast(&r2);
                *result = !r1 || !r2;
                return NOERROR;

            case CursorJoinerResult_LEFT:
                mCursorLeft->IsLast(&r1);
                mCursorRight->IsAfterLast(&r2);
                *result = !r1 || !r2;
                return NOERROR;

            case CursorJoinerResult_RIGHT:
                mCursorLeft->IsAfterLast(&r1);
                mCursorRight->IsLast(&r2);
                *result = !r1 || !r2;
                return NOERROR;

            default:
                // throw new IllegalStateException("bad value for mCompareResult, "
                //             + mCompareResult);
                Slogger::E(String("CursorJoiner"), "bad value for mCompareResult, %d", mCompareResult);
                return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    else {
        mCursorLeft->IsAfterLast(&r1);
        mCursorRight->IsAfterLast(&r2);
        *result = !r1 || !r2;
        return NOERROR;
    }
}

ECode CCursorJoiner::GetNext(
    /* [out] */ CursorJoinerResult* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean hasNext;
    if (HasNext(&hasNext), !hasNext) {
        // throw new IllegalStateException("you must only call next() when hasNext() is true");
        Slogger::E(String("CursorJoiner"), "you must only call next() when hasNext() is true");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    IncrementCursors();
    HasNext(&hasNext);
    assert(hasNext);

    Boolean isAfterLast;
    mCursorLeft->IsAfterLast(&isAfterLast);
    Boolean hasLeft = !isAfterLast;
    mCursorRight->IsAfterLast(&isAfterLast);
    Boolean hasRight = !isAfterLast;

    if (hasLeft && hasRight) {
        PopulateValues(*mValues, mCursorLeft, *mColumnsLeft, 0 /* start filling at index 0 */);
        PopulateValues(*mValues, mCursorRight, *mColumnsRight, 1 /* start filling at index 1 */);
        Int32 result;
        CompareStrings(*mValues, &result);
        switch (result) {
            case -1:
                mCompareResult = CursorJoinerResult_LEFT;
                break;
            case 0:
                mCompareResult = CursorJoinerResult_BOTH;
                break;
            case 1:
                mCompareResult = CursorJoinerResult_RIGHT;
                break;
        }
    }
    else if (hasLeft) {
        mCompareResult = CursorJoinerResult_LEFT;
    }
    else  {
        assert(hasRight);
        mCompareResult = CursorJoinerResult_RIGHT;
    }
    mCompareResultIsValid = TRUE;
    *result = mCompareResult;
    return NOERROR;
}

ECode CCursorJoiner::Remove()
{
//    throw new UnsupportedOperationException("not implemented");
    Slogger::E(String("CursorJoiner"), "not implemented");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

void CCursorJoiner::PopulateValues(
    /* [in] */ ArrayOf<String>& values,
    /* [in] */ ICursor* cursor,
    /* [in] */ const ArrayOf<Int32>& columnIndicies,
    /* [in] */ Int32 startingIndex)
{
    assert(startingIndex == 0 || startingIndex == 1);
    for (Int32 i = 0; i < columnIndicies.GetLength(); i++) {
        cursor->GetString(columnIndicies[i], &values[startingIndex + i * 2]);
    }
}

void CCursorJoiner::IncrementCursors()
{
    if(mCompareResultIsValid) {
        Boolean succeeded;
        switch(mCompareResult) {
            case CursorJoinerResult_LEFT:
                mCursorLeft->MoveToNext(&succeeded);
                break;
            case CursorJoinerResult_RIGHT:
                mCursorRight->MoveToNext(&succeeded);
                break;
            case CursorJoinerResult_BOTH:
                mCursorLeft->MoveToNext(&succeeded);
                mCursorRight->MoveToNext(&succeeded);
                break;
        }
        mCompareResultIsValid = FALSE;
    }
}

ECode CCursorJoiner::CompareStrings(
    /* [in] */ const ArrayOf<String>& values,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if ((values.GetLength() % 2) != 0) {
        // throw new IllegalArgumentException("you must specify an even number of values");
        Slogger::E(String("CursorJoiner"), "you must specify an even number of values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    for (Int32 index = 0; index < values.GetLength(); index += 2) {
        if (values[index].IsNull()) {
            if (values[index + 1].IsNull()) continue;
            *result = -1;
            return NOERROR;
        }

        if (values[index + 1].IsNull()) {
            *result = 1;
            return NOERROR;
        }

        Int32 comp = values[index].Compare(values[index + 1]);
        if (comp != 0) {
            *result = comp < 0 ? -1 : 1;
            return NOERROR;
        }
    }

    *result = 0;
    return NOERROR;
}

ECode CCursorJoiner::constructor(
    /* [in] */ ICursor* cursorLeft,
    /* [in] */ const ArrayOf<String>& columnNamesLeft,
    /* [in] */ ICursor* cursorRight,
    /* [in] */ const ArrayOf<String>& columnNamesRight)
{
    if(columnNamesLeft.GetLength() != columnNamesRight.GetLength()) {
        // throw new IllegalArgumentException(
        //             "you must have the same number of columns on the left and right, "
        //                     + columnNamesLeft.length + " != " + columnNamesRight.length);
        Slogger::E(String("CursorJoiner"), "you must have the same number of columns on the left and right, %d != %d"
                , columnNamesLeft.GetLength(), columnNamesRight.GetLength());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mCursorLeft = cursorLeft;
    mCursorRight = cursorRight;

    Boolean succeeded;
    mCursorLeft->MoveToFirst(&succeeded);
    mCursorRight->MoveToFirst(&succeeded);

    mCompareResultIsValid = FALSE;

    mColumnsLeft = BuildColumnIndiciesArray(cursorLeft, columnNamesLeft);
    mColumnsRight = BuildColumnIndiciesArray(cursorRight, columnNamesRight);

    mValues = ArrayOf<String>::Alloc(mColumnsLeft->GetLength() * 2);
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
