
#include "content/IntentFilter.h"
#ifdef DROID_CORE
#include "content/CIntentFilter.h"
#include "content/CIntentFilterAuthorityEntry.h"
#include "os/CPatternMatcher.h"
#endif
#include "util/XmlUtils.h"
#include <elastos/utility/etl/Algorithm.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::CPatternMatcher;
using Elastos::Droid::Utility::XmlUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {

const String IntentFilter::SGLOB_STR = String("sglob");
const String IntentFilter::PREFIX_STR = String("prefix");
const String IntentFilter::LITERAL_STR = String("literal");
const String IntentFilter::PATH_STR = String("path");
const String IntentFilter::PORT_STR = String("port");
const String IntentFilter::HOST_STR = String("host");
const String IntentFilter::AUTH_STR = String("auth");
const String IntentFilter::SCHEME_STR = String("scheme");
const String IntentFilter::TYPE_STR = String("type");
const String IntentFilter::CAT_STR = String("cat");
const String IntentFilter::NAME_STR = String("name");
const String IntentFilter::ACTION_STR = String("action");

IntentFilter::IntentFilter()
    : mPriority(0)
    , mHasPartialTypes(FALSE)
{}

IntentFilter::IntentFilter(
    /* [in] */ const String& action)
    : mPriority(0)
    , mHasPartialTypes(FALSE)
{
    Init(action);
}

IntentFilter::IntentFilter(
    /* [in] */ const String& action,
    /* [in] */ const String& dataType)
    : mPriority(0)
    , mHasPartialTypes(FALSE)
{
    Init(action, dataType);
}

IntentFilter::IntentFilter(
    /* [in] */ IIntentFilter* o)
    : mPriority(0)
    , mHasPartialTypes(FALSE)
{
    Init(o);
}

IntentFilter::~IntentFilter()
{
    mActions.Clear();
    mCategories = NULL;
    mDataSchemes = NULL;
    mDataAuthorities = NULL;
    mDataPaths = NULL;
    mDataTypes = NULL;
}

ECode IntentFilter::Init()
{
    return NOERROR;
}

ECode IntentFilter::Init(
    /* [in] */ const String& action)
{
    AddAction(action);
    return NOERROR;
}

ECode IntentFilter::Init(
    /* [in] */ const String& action,
    /* [in] */ const String& dataType)
{
    AddAction(action);
    AddDataType(dataType);
    return NOERROR;
}

ECode IntentFilter::Init(
    /* [in] */ IIntentFilter* other)
{
    other->GetPriority(&mPriority);
    AutoPtr< ArrayOf<String> > actions;
    other->GetActions((ArrayOf<String>**)&actions);
    assert(actions != NULL);
    Int32 len = actions->GetLength();
    for (Int32 i = 0; i != len; i++) {
        mActions.PushBack((*actions)[i]);
    }
    AutoPtr< ArrayOf<String> > categories;
    other->GetCategories((ArrayOf<String>**)&categories);
    if (NULL != categories) {
        mCategories = new List<String>();
        Int32 len = categories->GetLength();
        for (Int32 i = 0; i != len; i++) {
            mCategories->PushBack((*categories)[i]);
        }
    }
    AutoPtr< ArrayOf<String> > dataTypes;
    other->GetTypes((ArrayOf<String>**)&dataTypes);
    if (NULL != dataTypes) {
        mDataTypes = new List<String>();
        Int32 len = dataTypes->GetLength();
        for (Int32 i = 0; i != len; i++) {
            mDataTypes->PushBack((*dataTypes)[i]);
        }
    }
    AutoPtr< ArrayOf<String> > schemes;
    other->GetSchemes((ArrayOf<String>**)&schemes);
    if (NULL != schemes) {
        mDataSchemes = new List<String>();
        Int32 len = schemes->GetLength();
        for (Int32 i = 0; i != len; i++) {
            mDataSchemes->PushBack((*schemes)[i]);
        }
    }
    AutoPtr< ArrayOf<IIntentFilterAuthorityEntry*> > authorities;
    other->GetAuthorities((ArrayOf<IIntentFilterAuthorityEntry*>**)&authorities);
    if (NULL != authorities) {
        mDataAuthorities = new List< AutoPtr<IIntentFilterAuthorityEntry> >();
        Int32 len = authorities->GetLength();
        for (Int32 i = 0; i != len; i++) {
            mDataAuthorities->PushBack((*authorities)[i]);
        }
    }
    AutoPtr<ArrayOf<IPatternMatcher*> > paths;
    other->GetPaths((ArrayOf<IPatternMatcher*>**)&paths);
    if (NULL != paths) {
        mDataPaths = new List< AutoPtr<IPatternMatcher> >();
        Int32 len = paths->GetLength();
        for (Int32 i = 0; i != len; i++) {
            mDataPaths->PushBack((*paths)[i]);
        }
    }
    other->HasPartialTypes(&mHasPartialTypes);
    return NOERROR;
}

Int32 IntentFilter::FindStringInSet(
    /* [in] */ ArrayOf<String>* set,
    /* [in] */ const String& string,
    /* [in] */ const ArrayOf<Int32>& lengths,
    /* [in] */ Int32 lenPos)
{
    if (set == NULL) return -1;
    const Int32 N = lengths[lenPos];
    for (Int32 i = 0; i < N; i++) {
        if ((*set)[i].Equals(string)) return i;
    }
    return -1;
}

AutoPtr< ArrayOf<String> > IntentFilter::AddStringToSet(
    /* [in] */ ArrayOf<String>* inSet,
    /* [in] */ const String& string,
    /* [in] */ ArrayOf<Int32>& lengths,
    /* [in] */ Int32 lenPos)
{
    if (FindStringInSet(inSet, string, lengths, lenPos) >= 0) return inSet;
    AutoPtr<ArrayOf<String> > set = inSet;
    if (set == NULL) {
        set = ArrayOf<String>::Alloc(2);
        (*set)[0] = string;
        lengths[lenPos] = 1;
        return set;
    }
    const Int32 N = lengths[lenPos];
    if (N < set->GetLength()) {
        (*set)[N] = string;
        lengths[lenPos] = N + 1;
        return set;
    }

    AutoPtr<ArrayOf<String> > newSet = ArrayOf<String>::Alloc((N * 3) / 2 + 2);
    for (Int32 i = 0; i < N; ++i) {
        (*newSet)[i] = (*set)[i];
    }
    (*newSet)[N] = string;
    lengths[lenPos] = N + 1;
    return newSet;
}

AutoPtr< ArrayOf<String> > IntentFilter::RemoveStringFromSet(
    /* [in] */ ArrayOf<String>* set,
    /* [in] */ const String& string,
    /* [in] */ ArrayOf<Int32>& lengths,
    /* [in] */ Int32 lenPos)
{
    AutoPtr<ArrayOf<String> > newSet;
    Int32 pos = FindStringInSet(set, string, lengths, lenPos);
    if (pos < 0) {
        newSet = set;
        return newSet;
    }

    const Int32 N = lengths[lenPos];
    if (N > (set->GetLength() / 4)) {
        Int32 copyLen = N - (pos + 1);
        if (copyLen > 0) {
            for (Int32 i = pos; i < pos + copyLen; ++i) {
                (*set)[i] = (*set)[i + 1];
            }
        }
        (*set)[N - 1] = NULL;
        lengths[lenPos] = N - 1;

        newSet = set;
        return newSet;
    }

    newSet = ArrayOf<String>::Alloc(set->GetLength() / 3);
    if (pos > 0) {
        for (Int32 i = 0; i < pos; ++i) {
            (*newSet)[i] = (*set)[i];
        }
    }
    if ((pos + 1) < N) {
        for (Int32 i = pos; i < N - 1; ++i) {
            (*newSet)[i] = (*set)[i + 1];
        }
    }
    return newSet;
}

ECode IntentFilter::Create(
    /* [in] */ const String& action,
    /* [in] */ const String& dataType,
    /* [out] */ IIntentFilter** filter)
{
    VALIDATE_NOT_NULL(filter)
    ECode ecode = CIntentFilter::New(action, dataType, filter);
    if (E_MALFORMED_MIME_TYPE_EXCEPTION == ecode) return E_RUNTIME_EXCEPTION;
    return ecode;
}

ECode IntentFilter::SetPriority(
    /* [in] */ Int32 priority)
{
    mPriority = priority;
    return NOERROR;
}

Int32 IntentFilter::GetPriority()
{
    return mPriority;
}

ECode IntentFilter::AddAction(
    /* [in] */ const String& action)
{
    if (action.IsNull()) {
        Logger::E("IntentFilter", "Failed to add Action, action name is NULL.");
        assert(0 && "ERROR: action name is NULL.");
        return E_INVALID_ARGUMENT;
    }

    List<String>::Iterator it = Find(mActions.Begin(), mActions.End(), action);
    if (it == mActions.End()) {
        mActions.PushBack(action);
    }
    return NOERROR;
}

Int32 IntentFilter::CountActions()
{
    return mActions.GetSize();
}

String IntentFilter::GetAction(
    /* [in] */ Int32 index)
{
    if (index < 0) return String(NULL);
    return mActions[index];
}

Boolean IntentFilter::HasAction(
    /* [in] */ const String& action)
{
    return !action.IsNull() &&
        Find(mActions.Begin(), mActions.End(), action) != mActions.End();
}

Boolean IntentFilter::MatchAction(
    /* [in] */ const String& action)
{
    return HasAction(action);
}

AutoPtr< ArrayOf<String> > IntentFilter::GetActions()
{
    Int32 size = mActions.GetSize();
    AutoPtr< ArrayOf<String> > actions = ArrayOf<String>::Alloc(size);
    Int32 i = 0;
    List<String>::Iterator it;
    for (it = mActions.Begin(); it != mActions.End(); ++it, ++i) {
        (*actions)[i] = *it;
    }
    return actions;
}

ECode IntentFilter::AddDataType(
    /* [in] */ const String& type)
{
    Int32 slashpos = type.IndexOf('/');
    Int32 typelen = type.GetLength();
    if (slashpos > 0 && typelen >= slashpos + 2) {
        if (NULL == mDataTypes) mDataTypes = new List<String>();
        if (typelen == slashpos + 2 && type.GetChar(slashpos + 1) == '*') {
            String str = type.Substring(0, slashpos);
            if (Find(mDataTypes->Begin(), mDataTypes->End(), str) == mDataTypes->End()) {
                mDataTypes->PushBack(str);
            }
            mHasPartialTypes = TRUE;
        }
        else {
            if (Find(mDataTypes->Begin(), mDataTypes->End(), type) == mDataTypes->End()) {
                mDataTypes->PushBack(type);
            }
        }
        return NOERROR;
    }

    // throw new MalformedMimeTypeException(type);
    return E_MALFORMED_MIME_TYPE_EXCEPTION;
}

Boolean IntentFilter::HasDataType(
    /* [in] */ const String& type)
{
    return mDataTypes != NULL && FindMimeType(type);
}

Int32 IntentFilter::CountDataTypes()
{
    return mDataTypes != NULL ? mDataTypes->GetSize() : 0;
}

String IntentFilter::GetDataType(
    /* [in] */ Int32 index)
{
    if (index < 0) return String(NULL);

    Int32 i;
    List<String>::Iterator it;
    for (it = mDataTypes->Begin(), i = 0; it != mDataTypes->End() && i < index; ++it) {}
    if (it != mDataTypes->End()) {
        return *it;
    }
    return String(NULL);
}

AutoPtr< ArrayOf<String> > IntentFilter::GetTypes()
{
    if (mDataTypes == NULL) return NULL;

    Int32 size = mDataTypes->GetSize();
    AutoPtr< ArrayOf<String> > types = ArrayOf<String>::Alloc(size);
    Int32 i;
    List<String>::Iterator it;
    for (it = mDataTypes->Begin(), i = 0; it != mDataTypes->End(); ++it, ++i) {
        (*types)[i] = *it;
    }
    return types;
}

ECode IntentFilter::AddDataScheme(
    /* [in] */ const String& scheme)
{
    if (mDataSchemes == NULL) mDataSchemes = new List<String>();
    if (Find(mDataSchemes->Begin(), mDataSchemes->End(), scheme) == mDataSchemes->End()) {
        mDataSchemes->PushBack(scheme);
    }
    return NOERROR;
}

Int32 IntentFilter::CountDataSchemes()
{
    return mDataSchemes != NULL ? mDataSchemes->GetSize() : 0;
}

String IntentFilter::GetDataScheme(
    /* [in] */ Int32 index)
{
    if (index < 0) return String(NULL);

    Int32 i;
    List<String>::Iterator it;
    for (it = mDataSchemes->Begin(), i = 0; it != mDataSchemes->End() && i < index; ++it) {}
    if (it != mDataSchemes->End()) {
        return *it;
    }
    return String(NULL);
}

Boolean IntentFilter::HasDataScheme(
    /* [in] */ const String& scheme)
{
    return mDataSchemes != NULL && Find(mDataSchemes->Begin(),
            mDataSchemes->End(), scheme) != mDataSchemes->End();
}

AutoPtr< ArrayOf<String> > IntentFilter::GetSchemes()
{
    if (mDataSchemes == NULL) return NULL;

    Int32 size = mDataSchemes->GetSize();
    AutoPtr< ArrayOf<String> > schemes = ArrayOf<String>::Alloc(size);
    Int32 i;
    List<String>::Iterator it;
    for (it = mDataSchemes->Begin(), i = 0; it != mDataSchemes->End(); ++it, ++i) {
        (*schemes)[i] = *it;
    }
    return schemes;
}

ECode IntentFilter::AddDataAuthority(
    /* [in] */ const String& host,
    /* [in] */ const String& port)
{
    if (mDataAuthorities == NULL) {
        mDataAuthorities = new List< AutoPtr<IIntentFilterAuthorityEntry> >();
    }
    AutoPtr<IIntentFilterAuthorityEntry> authEntry;
    FAIL_RETURN(CIntentFilterAuthorityEntry::New(host, port, (IIntentFilterAuthorityEntry**)&authEntry))
    mDataAuthorities->PushBack(authEntry);
    return NOERROR;
}

Int32 IntentFilter::CountDataAuthorities()
{
    return mDataAuthorities != NULL ? mDataAuthorities->GetSize() : 0;
}

AutoPtr<IIntentFilterAuthorityEntry> IntentFilter::GetDataAuthority(
    /* [in] */ Int32 index)
{
    if (index < 0) return NULL;

    Int32 i;
    List< AutoPtr<IIntentFilterAuthorityEntry> >::Iterator it;
    for (it = mDataAuthorities->Begin(), i = 0; it != mDataAuthorities->End() && i < index; ++it) {}
    if (it != mDataAuthorities->End()) {
        return *it;
    }
    return NULL;
}

Boolean IntentFilter::HasDataAuthority(
    /* [in] */ IUri* data)
{
    return MatchDataAuthority(data) >= 0;
}

AutoPtr< ArrayOf<IIntentFilterAuthorityEntry*> > IntentFilter::GetAuthorities()
{
    if (mDataAuthorities == NULL) return NULL;

    Int32 size = mDataAuthorities->GetSize();
    AutoPtr< ArrayOf<IIntentFilterAuthorityEntry*> > authorities =
            ArrayOf<IIntentFilterAuthorityEntry*>::Alloc(size);
    Int32 i;
    List< AutoPtr<IIntentFilterAuthorityEntry> >::Iterator it;
    for (it = mDataAuthorities->Begin(), i = 0; it != mDataAuthorities->End(); ++it, ++i) {
        authorities->Set(i, *it);
    }
    return authorities;
}

ECode IntentFilter::AddDataPath(
    /* [in] */ const String& path,
    /* [in] */ Int32 type)
{
    if (mDataPaths == NULL) {
        mDataPaths = new List< AutoPtr<IPatternMatcher> >();
    }
    AutoPtr<IPatternMatcher> pm;
    CPatternMatcher::New(path, type, (IPatternMatcher**)&pm);
    mDataPaths->PushBack(pm);
    return NOERROR;
}

Int32 IntentFilter::CountDataPaths()
{
    return mDataPaths != NULL ? mDataPaths->GetSize() : 0;
}

AutoPtr<IPatternMatcher> IntentFilter::GetDataPath(
    /* [in] */ Int32 index)
{
    if (index < 0) return NULL;

    Int32 i;
    List< AutoPtr<IPatternMatcher> >::Iterator it;
    for (it = mDataPaths->Begin(), i = 0; it != mDataPaths->End() && i < index; ++it) {}
    if (it != mDataPaths->End()) {
        return *it;
    }
    return NULL;
}

Boolean IntentFilter::HasDataPath(
    /* [in] */ const String& data)
{
    if (mDataPaths == NULL) {
        return FALSE;
    }
    Boolean isMatched;
    List< AutoPtr<IPatternMatcher> >::Iterator it = mDataPaths->Begin();
    while (it != mDataPaths->End()) {
        AutoPtr<IPatternMatcher> pe = *it;
        pe->Match(data, &isMatched);
        if (isMatched) {
            return TRUE;
        }
        ++it;
    }
    return FALSE;
}

AutoPtr< ArrayOf<IPatternMatcher*> > IntentFilter::GetPaths()
{
    if (mDataPaths == NULL) return NULL;

    Int32 size = mDataPaths->GetSize();
    AutoPtr< ArrayOf<IPatternMatcher*> > paths = ArrayOf<IPatternMatcher*>::Alloc(size);
    Int32 i;
    List< AutoPtr<IPatternMatcher> >::Iterator it;
    for (it = mDataPaths->Begin(), i = 0; it != mDataPaths->End(); ++it, ++i) {
        paths->Set(i, *it);
    }
    return paths;
}

Int32 IntentFilter::MatchDataAuthority(
    /* [in] */ IUri* data)
{
    if (mDataAuthorities == NULL) {
        return IIntentFilter::NO_MATCH_DATA;
    }
    List< AutoPtr<IIntentFilterAuthorityEntry> >::Iterator it = mDataAuthorities->Begin();
    while (it != mDataAuthorities->End()) {
        AutoPtr<IIntentFilterAuthorityEntry> ae = *it;
        Int32 match;
        ae->Match(data, &match);
        if (match >= 0) {
            return match;
        }
        ++it;
    }
    return IIntentFilter::NO_MATCH_DATA;
}

Int32 IntentFilter::MatchData(
    /* [in] */ const String& type,
    /* [in] */ const String& scheme,
    /* [in] */ IUri* data)
{
    AutoPtr< List<String> > types = mDataTypes;
    AutoPtr< List<String> > schemes = mDataSchemes;
    AutoPtr< List< AutoPtr<IIntentFilterAuthorityEntry> > > authorities = mDataAuthorities;
    AutoPtr< List< AutoPtr<IPatternMatcher> > > paths = mDataPaths;

    Int32 match = IIntentFilter::MATCH_CATEGORY_EMPTY;

    if (types == NULL && schemes == NULL) {
        return ((type.IsNull() && data == NULL)
            ? (IIntentFilter::MATCH_CATEGORY_EMPTY + IIntentFilter::MATCH_ADJUSTMENT_NORMAL)
            : IIntentFilter::NO_MATCH_DATA);
    }

    if (schemes != NULL) {
        if (Find(schemes->Begin(), schemes->End(),
                !scheme.IsNull() ? scheme : String("")) != schemes->End()) {
            match = IIntentFilter::MATCH_CATEGORY_SCHEME;
        }
        else {
            return IIntentFilter::NO_MATCH_DATA;
        }

        if (authorities != NULL) {
            Int32 authMatch = MatchDataAuthority(data);
            if (authMatch >= 0) {
                String path;
                if (paths == NULL) {
                    match = authMatch;
                }
                else if (data->GetPath(&path), HasDataPath(path)) {
                    match = IIntentFilter::MATCH_CATEGORY_PATH;
                }
                else {
                    return IIntentFilter::NO_MATCH_DATA;
                }
            }
            else {
                return IIntentFilter::NO_MATCH_DATA;
            }
        }
    }
    else {
        // Special case: match either an Intent with no data URI,
        // or with a scheme: URI.  This is to give a convenience for
        // the common case where you want to deal with data in a
        // content provider, which is done by type, and we don't want
        // to force everyone to say they handle content: or file: URIs.
        if (!scheme.IsNull() && !scheme.Equals("")
                && !scheme.Equals("content")
                && !scheme.Equals("file")) {
            return IIntentFilter::NO_MATCH_DATA;
        }
    }

    if (types != NULL) {
        if (FindMimeType(type)) {
            match = IIntentFilter::MATCH_CATEGORY_TYPE;
        }
        else {
            return IIntentFilter::NO_MATCH_TYPE;
        }
    }
    else {
        // If no MIME types are specified, then we will only match against
        // an Intent that does not have a MIME type.
        if (!type.IsNull()) {
            return IIntentFilter::NO_MATCH_TYPE;
        }
    }

    return match + IIntentFilter::MATCH_ADJUSTMENT_NORMAL;
}

ECode IntentFilter::AddCategory(
    /* [in] */ const String& category)
{
    if (mCategories == NULL) {
        mCategories = new List<String>();
    }
    if (Find(mCategories->Begin(), mCategories->End(), category) == mCategories->End()) {
        mCategories->PushBack(category);
    }
    return NOERROR;
}

Int32 IntentFilter::CountCategories()
{
    return mCategories != NULL ? mCategories->GetSize() : 0;
}

String IntentFilter::GetCategory(
    /* [in] */ Int32 index)
{
    if (index < 0) return String(NULL);

    Int32 i;
    List<String>::Iterator it;
    for (it = mCategories->Begin(), i = 0; it != mCategories->End() && i < index; ++it, ++i) {}
    if (it != mCategories->End()) {
        return *it;
    }
    return String(NULL);
}

Boolean IntentFilter::HasCategory(
    /* [in] */ const String& category)
{
    return mCategories != NULL && Find(mCategories->Begin(),
            mCategories->End(), category) != mCategories->End();
}

AutoPtr< ArrayOf<String> > IntentFilter::GetCategories()
{
    if (mCategories == NULL) return NULL;

    Int32 size = mCategories->GetSize();
    AutoPtr< ArrayOf<String> > categories = ArrayOf<String>::Alloc(size);
    Int32 i;
    List<String>::Iterator it;
    for (it = mCategories->Begin(), i = 0; it != mCategories->End(); ++it, ++i) {
        (*categories)[i] = *it;
    }
    return categories;
}

String IntentFilter::MatchCategories(
    /* [in] */ ArrayOf<String>* categories)
{
    if (categories == NULL) {
        return String(NULL);
    }

    if (mCategories == NULL) {
        return categories->GetLength() > 0 ? (*categories)[0] : String(NULL);
    }

    for (Int32 i = 0; i < categories->GetLength(); ++i) {
        const String& category = (*categories)[i];
        if (Find(mCategories->Begin(), mCategories->End(), category)
                == mCategories->End()) {
            return category;
        }
    }

    return String(NULL);
}

Int32 IntentFilter::Match(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean resolve,
    /* [in] */ const String& logTag)
{
    String action, type, scheme;
    intent->GetAction(&action);
    if (resolver != NULL) {
        intent->ResolveType(resolver, &type);
    }
    else {
        intent->GetType(&type);
    }
    intent->GetScheme(&scheme);
    AutoPtr<IUri> data;
    intent->GetData((IUri**)&data);
    AutoPtr< ArrayOf<String> > categories;
    intent->GetCategories((ArrayOf<String>**)&categories);
    return Match(action, type, scheme, data, categories, logTag);
}

Int32 IntentFilter::Match(
    /* [in] */ const String& action,
    /* [in] */ const String& type,
    /* [in] */ const String& scheme,
    /* [in] */ IUri* data,
    /* [in] */ ArrayOf<String>* categories,
    /* [in] */ const String& logTag)
{
    if (!action.IsNull() && !MatchAction(action)) {
        if (FALSE) Logger::V(logTag, String("No matching action ") + action + String(" for ")
                + String("this IntentFilter") /* this ToString() ? */);
        return IIntentFilter::NO_MATCH_ACTION;
    }

    Int32 dataMatch = MatchData(type, scheme, data);
    if (dataMatch < 0) {
        if (FALSE) {
            if (dataMatch == IIntentFilter::NO_MATCH_TYPE) {
                Logger::V(logTag, String("No matching type ") + type
                        + String(" for ") + String("this IntentFilter") /* this ToString() ? */);
            }
            if (dataMatch == IIntentFilter::NO_MATCH_DATA) {
                String str;
                data->ToString(&str);
                Logger::V(logTag, String("No matching scheme/path ") + str
                        + String(" for ") + String("this IntentFilter") /* this ToString() ? */);
            }
        }
        return dataMatch;
    }

    String categoryMatch = MatchCategories(categories);
    if (!categoryMatch.IsNull()) {
        if (FALSE) Logger::V(logTag, String("No matching category ")
                + categoryMatch + String(" for ") + String("this IntentFilter") /* this ToString() ? */);
        return IIntentFilter::NO_MATCH_CATEGORY;
    }

    // It would be nice to treat container activities as more
    // important than ones that can be embedded, but this is not the way...
    if (FALSE) {
        if (NULL != categories) {
            dataMatch -= mCategories->GetSize() - categories->GetLength();
        }
    }

    return dataMatch;
}

ECode IntentFilter::WriteToXml(
    /* [in] */ IXmlSerializer* serializer)
{
    List<String>::Iterator it;
    for (it = mActions.Begin(); it != mActions.End(); ++it) {
        FAIL_RETURN(serializer->WriteStartTag(String(NULL), ACTION_STR));
        FAIL_RETURN(serializer->WriteAttribute(String(NULL), NAME_STR, *it));
        FAIL_RETURN(serializer->WriteEndTag(String(NULL), ACTION_STR));
    }

    if (mCategories != NULL) {
        for (it = mCategories->Begin(); it != mCategories->End(); ++it) {
            FAIL_RETURN(serializer->WriteStartTag(String(NULL), CAT_STR));
            FAIL_RETURN(serializer->WriteAttribute(String(NULL), NAME_STR, *it));
            FAIL_RETURN(serializer->WriteEndTag(String(NULL), CAT_STR));
        }
    }

    if (mDataTypes != NULL) {
        for (it = mDataTypes->Begin(); it != mDataTypes->End(); ++it) {
            FAIL_RETURN(serializer->WriteStartTag(String(NULL), TYPE_STR));
            String type = *it;
            if (type.IndexOf('/') < 0) type = type + "/*";
            FAIL_RETURN(serializer->WriteAttribute(String(NULL), NAME_STR, *it));
            FAIL_RETURN(serializer->WriteEndTag(String(NULL), TYPE_STR));
        }
    }

    if (mDataSchemes != NULL) {
        for (it = mDataSchemes->Begin(); it != mDataSchemes->End(); ++it) {
            FAIL_RETURN(serializer->WriteStartTag(String(NULL), SCHEME_STR));
            FAIL_RETURN(serializer->WriteAttribute(String(NULL), NAME_STR, *it));
            FAIL_RETURN(serializer->WriteEndTag(String(NULL), SCHEME_STR));
        }
    }

    if (mDataAuthorities != NULL) {
        List< AutoPtr<IIntentFilterAuthorityEntry> >::Iterator daIt;
        for (daIt = mDataAuthorities->Begin(); daIt != mDataAuthorities->End(); ++daIt) {
            FAIL_RETURN(serializer->WriteStartTag(String(NULL), AUTH_STR));
            AutoPtr<IIntentFilterAuthorityEntry> ae = *daIt;
            String host;
            Int32 port;
            ae->GetHost(&host);
            ae->GetPort(&port);
            FAIL_RETURN(serializer->WriteAttribute(String(NULL), HOST_STR, host));
            if (port >= 0) {
                FAIL_RETURN(serializer->WriteAttribute(String(NULL), PORT_STR, StringUtils::Int32ToString(port)));
            }
            FAIL_RETURN(serializer->WriteEndTag(String(String(NULL)), AUTH_STR));
        }
    }

    if (mDataPaths != NULL) {
        List< AutoPtr<IPatternMatcher> >::Iterator dpIt;
        for (dpIt = mDataPaths->Begin(); dpIt != mDataPaths->End(); ++dpIt) {
            FAIL_RETURN(serializer->WriteStartTag(String(NULL), PATH_STR));
            AutoPtr<IPatternMatcher> pe = *dpIt;
            Int32 type;
            pe->GetType(&type);
            String path;
            switch (type) {
                case IPatternMatcher::PATTERN_LITERAL:
                    pe->GetPath(&path);
                    FAIL_RETURN(serializer->WriteAttribute(String(NULL), LITERAL_STR, path));
                    break;
                case IPatternMatcher::PATTERN_PREFIX:
                    pe->GetPath(&path);
                    FAIL_RETURN(serializer->WriteAttribute(String(NULL), PREFIX_STR, path));
                    break;
                case IPatternMatcher::PATTERN_SIMPLE_GLOB:
                    pe->GetPath(&path);
                    FAIL_RETURN(serializer->WriteAttribute(String(NULL), SGLOB_STR, path));
                    break;
            }
            FAIL_RETURN(serializer->WriteEndTag(String(NULL), PATH_STR));
        }
    }
    return NOERROR;
}

ECode IntentFilter::ReadFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    Int32 type, depth;
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
           && (type != IXmlPullParser::END_TAG
                   || (parser->GetDepth(&depth), depth > outerDepth))) {
        if (type == IXmlPullParser::END_TAG
                || type == IXmlPullParser::TEXT) {
            continue;
        }

        String tagName;
        FAIL_RETURN(parser->GetName(&tagName));
        if (tagName.Equals(ACTION_STR)) {
            String name;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), NAME_STR, &name));
            if (!name.IsNull()) {
                AddAction(name);
            }
        }
        else if (tagName.Equals(CAT_STR)) {
            String name;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), NAME_STR, &name));
            if (!name.IsNull()) {
                AddCategory(name);
            }
        }
        else if (tagName.Equals(TYPE_STR)) {
            String name;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), NAME_STR, &name));
            if (!name.IsNull()) {
//                try {
                AddDataType(name);
//                } catch (MalformedMimeTypeException e) {
//                }
            }
        }
        else if (tagName.Equals(SCHEME_STR)) {
            String name;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), NAME_STR, &name));
            if (!name.IsNull()) {
                AddDataScheme(name);
            }
        }
        else if (tagName.Equals(AUTH_STR)) {
            String host, port;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), HOST_STR, &host));
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), PORT_STR, &port));
            if (!host.IsNull()) {
                AddDataAuthority(host, port);
            }
        }
        else if (tagName.Equals(PATH_STR)) {
            String path;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), LITERAL_STR, &path));
            if (!path.IsNull()) {
                AddDataPath(path, IPatternMatcher::PATTERN_LITERAL);
            }
            else {
                FAIL_RETURN(parser->GetAttributeValue(String(NULL), PREFIX_STR, &path));
                if (!path.IsNull()) {
                    AddDataPath(path, IPatternMatcher::PATTERN_PREFIX);
                }
                else {
                    FAIL_RETURN(parser->GetAttributeValue(String(NULL), SGLOB_STR, &path));
                    if (!path.IsNull()) {
                        AddDataPath(path, IPatternMatcher::PATTERN_SIMPLE_GLOB);
                    }
                }
            }
        }
        else {
            Logger::W("IntentFilter", String("Unknown tag parsing IntentFilter: ") + tagName);
        }
        XmlUtils::SkipCurrentTag(parser);
    }
    return NOERROR;
}

ECode IntentFilter::Dump(
    /* [in] */ IPrinter* du,
    /* [in] */ const String& prefix)
{
    AutoPtr<StringBuilder> sb = new StringBuilder(256);
    if (mActions.IsEmpty() == FALSE) {
        List<String>::Iterator it = mActions.Begin();
        for (; it != mActions.End(); ++it) {
            sb->Reset();
            *sb += prefix;
            *sb += "Action: \"";
            *sb += *it;
            *sb += "\"";
            du->Println(sb->ToString());
        }
    }
    if (NULL != mCategories) {
        List<String>::Iterator it = mCategories->Begin();
        for (; it != mCategories->End(); ++it) {
            sb->Reset();
            *sb += prefix;
            *sb += "Category: \"";
            *sb += *it;
            *sb += "\"";
            du->Println(sb->ToString());
        }
    }
    if (NULL != mDataSchemes) {
        List<String>::Iterator it = mDataSchemes->Begin();
        for (; it != mDataSchemes->End(); ++it) {
            sb->Reset();
            *sb += prefix;
            *sb += "Scheme: \"";
            *sb += *it;
            *sb += "\"";
            du->Println(sb->ToString());
        }
    }
    if (NULL != mDataAuthorities) {
        List< AutoPtr<IIntentFilterAuthorityEntry> >::Iterator it = mDataAuthorities->Begin();
        for (; it != mDataAuthorities->End(); ++it) {
            AutoPtr<IIntentFilterAuthorityEntry> ae = *it;
            String host;
            Int32 port = 0;
            ae->GetHost(&host);
            ae->GetPort(&port);
            sb->Reset();
            *sb += prefix;
            *sb += "Authority: \"";
            *sb += host;
            *sb += "\": ";
            *sb += StringUtils::Int32ToString(port);
            Boolean wild = FALSE;
            ae->GetWild(&wild);
            if (wild) *sb += " WILD";
            du->Println(sb->ToString());
        }

    }
    if (NULL != mDataPaths) {
        List< AutoPtr<IPatternMatcher> >::Iterator it = mDataPaths->Begin();
        for (; it != mDataPaths->End(); ++it) {
            AutoPtr<IPatternMatcher> pe = *it;
            String str;
            pe->ToString(&str);
            sb->Reset();
            *sb += prefix;
            *sb += "Path: \"";
            *sb += str;
            *sb += "\"";
            du->Println(sb->ToString());
        }
    }
    if (NULL != mDataTypes) {
        List<String>::Iterator it = mDataTypes->Begin();
        for (; it != mDataTypes->End(); ++it) {
            sb->Reset();
            *sb += prefix;
            *sb += "Type: \"";
            *sb += *it;
            *sb += "\"";
            du->Println(sb->ToString());
        }
    }
    if (0 != mPriority || mHasPartialTypes) {
        sb->Reset();
        *sb += prefix;
        *sb += "mPriority=";
        *sb += StringUtils::Int32ToString(mPriority);
        *sb += ", mHasPartialTypes=";
        *sb += StringUtils::BooleanToString(mHasPartialTypes);
        du->Println(sb->ToString());
    }

    return NOERROR;
}

ECode IntentFilter::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    Int32 count;
    String str;
    source->ReadInt32(&count);
    if(count > 0){
        for(Int32 i = 0; i < count; i++){
            source->ReadString(&str);
            mActions.PushBack(str);
        }
    }

    source->ReadInt32(&count);
    if(count > 0){
        if (mCategories == NULL) {
            mCategories = new List<String>();
        }

        for(Int32 i = 0; i < count; i++){
            source->ReadString(&str);
            mCategories->PushBack(str);
        }
    }

    source->ReadInt32(&count);
    if(count > 0){
        if (mDataSchemes == NULL) {
            mDataSchemes = new List<String>();
        }

        for(Int32 i = 0; i < count; i++){
            source->ReadString(&str);
            mDataSchemes->PushBack(str);
        }
    }

    source->ReadInt32(&count);
    if(count > 0){
        if (mDataTypes == NULL) {
            mDataTypes = new List<String>();
        }

        for(Int32 i = 0; i < count; i++){
            source->ReadString(&str);
            mDataTypes->PushBack(str);
        }
    }

    source->ReadInt32(&count);
    if(count > 0){
        if (mDataAuthorities == NULL) {
            mDataAuthorities = new List<AutoPtr<IIntentFilterAuthorityEntry> >();
        }

        for(Int32 i = 0; i < count; i++){
            AutoPtr<IIntentFilterAuthorityEntry> ae;
            source->ReadInterfacePtr((Handle32*)&ae);
            mDataAuthorities->PushBack(ae);
        }
    }

    source->ReadInt32(&count);
    if(count > 0){
        if (mDataPaths == NULL) {
            mDataPaths = new List<AutoPtr<IPatternMatcher> >();
        }

        for(Int32 i = 0; i < count; i++){
            AutoPtr<IPatternMatcher> pm;
            source->ReadInterfacePtr((Handle32*)&pm);
            mDataPaths->PushBack(pm);
        }
    }

    source->ReadInt32(&mPriority);
    source->ReadBoolean(&mHasPartialTypes);
    return NOERROR;
}

ECode IntentFilter::WriteToParcel(
    /* [in] */ IParcel *dest)
{
    Int32 count = CountActions();
    dest->WriteInt32(count);
    List<String>::Iterator it = mActions.Begin();
    for (; it != mActions.End(); ++it) {
        dest->WriteString(*it);
    }

    count = CountCategories();
    dest->WriteInt32(count);
    if(count > 0){
        List<String>::Iterator it = mCategories->Begin();
        for (; it != mCategories->End(); ++it) {
            dest->WriteString(*it);
        }
    }

    count = CountDataSchemes();
    dest->WriteInt32(count);
    if(count > 0){
        List<String>::Iterator it = mDataSchemes->Begin();
        for (; it != mDataSchemes->End(); ++it) {
            dest->WriteString(*it);
        }
    }

    count = CountDataTypes();
    dest->WriteInt32(count);
    if(count > 0){
        List<String>::Iterator it = mDataTypes->Begin();
        for (; it != mDataTypes->End(); ++it) {
            dest->WriteString(*it);
        }
    }

    count = CountDataAuthorities();
    dest->WriteInt32(count);
    if(count > 0){
        List< AutoPtr<IIntentFilterAuthorityEntry> >::Iterator it = mDataAuthorities->Begin();
        for (; it != mDataAuthorities->End(); ++it) {
            dest->WriteInterfacePtr(*it);
        }
    }

    count = CountDataPaths();
    dest->WriteInt32(count);
    if(count > 0){
        List< AutoPtr<IPatternMatcher> >::Iterator it = mDataPaths->Begin();
        for (; it != mDataPaths->End(); ++it) {
            dest->WriteInterfacePtr(*it);
        }
    }

    dest->WriteInt32(mPriority);
    dest->WriteBoolean(mHasPartialTypes);
    return NOERROR;
}

Boolean IntentFilter::DebugCheck()
{
    return TRUE;
}

Boolean IntentFilter::FindMimeType(
    /* [in] */ const String& type)
{
    AutoPtr< List<String> > t = mDataTypes;

    if (type.IsNull()) {
        return FALSE;
    }

    if (Find(t->Begin(), t->End(), type) != t->End()) {
        return TRUE;
    }

    // Deal with an Intent wanting to match every type in the IntentFilter.
    const Int32 typeLength = type.GetLength();
    if (typeLength == 3 && type.Equals("*/*")) {
        return !t->IsEmpty();
    }

    // Deal with this IntentFilter wanting to match every Intent type.
    if (mHasPartialTypes &&
            Find(t->Begin(), t->End(), String("*")) != t->End()) {
        return TRUE;
    }

    const Int32 slashpos = type.IndexOf('/');
    if (slashpos > 0) {
        if (mHasPartialTypes &&
                Find(t->Begin(), t->End(), type.Substring(0, slashpos))
                != t->End()) {
            return TRUE;
        }
        if (typeLength == slashpos + 2 && type.GetChar(slashpos + 1) == '*') {
            // Need to look through all types for one that matches
            // our base...
            List<String>::Iterator it = t->Begin();
            while (it != t->End()) {
                String v = *it;
                if (type.RegionMatches(0, v, 0, slashpos + 1)) {
                    return TRUE;
                }
                it++;
            }
        }
    }

    return FALSE;
}

} // namespace Content
} // namespace Droid
} // namespace Elastos
