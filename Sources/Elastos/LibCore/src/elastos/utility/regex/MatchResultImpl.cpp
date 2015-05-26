#include "MatchResultImpl.h"

namespace Elastos {
namespace Utility {
namespace Regex {

CAR_INTERFACE_IMPL(MatchResultImpl, IMatchResult)

MatchResultImpl::MatchResultImpl(
    /* [in] */ const String& text,
    /* [in] */ const ArrayOf<Int32>& offsets)
    : mText(text)
{
    mOffsets = offsets.Clone();
}

ECode MatchResultImpl::End(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    return EndEx(0, index);
}

ECode MatchResultImpl::EndEx(
    /* [in] */ Int32 group,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = (*mOffsets)[2 * group + 1];
    return NOERROR;
}

ECode MatchResultImpl::Group(
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text);
    Int32 startId, endId;
    Start(&startId);
    End(&endId);
    *text = mText.Substring(startId, endId);
    return NOERROR;
}

ECode MatchResultImpl::GroupEx(
    /* [in] */ Int32 group,
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text);
    Int32 from = (*mOffsets)[group * 2];
    Int32 to = (*mOffsets)[(group * 2) + 1];
    if (from == -1 || to == -1) {
        *text = String(NULL);
    }
    else {
        *text = mText.Substring(from, to);
    }
    return NOERROR;
}

ECode MatchResultImpl::GroupCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = (mOffsets->GetLength() / 2) - 1;
    return NOERROR;
}

ECode MatchResultImpl::StartEx(
    /* [in] */ Int32 group,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = (*mOffsets)[2 * group];
    return NOERROR;
}

ECode MatchResultImpl::Start(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    return StartEx(0, index);
}

} // namespace Regex
} // namespace Utility
} // namespace Elastos