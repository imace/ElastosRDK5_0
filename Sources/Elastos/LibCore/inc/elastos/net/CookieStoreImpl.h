
#ifndef __ELASTOS_NET_COOKIESTOREIMPL_H__
#define __ELASTOS_NET_COOKIESTOREIMPL_H__

#include "Object.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::IList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;

using Elastos::Net::IURI;

#ifndef HASH_FUNC_FOR_AUTOPTR_IURI
#define HASH_FUNC_FOR_AUTOPTR_IURI

_ETL_NAMESPACE_BEGIN

template<> struct Hash<AutoPtr<IURI> >
{
    size_t operator()(AutoPtr<IURI> name) const
    {
        return (size_t)name.Get();
    }
};

template<> struct EqualTo<AutoPtr<IURI> >
{
    Boolean operator()(const AutoPtr<IURI>& x,
                       const AutoPtr<IURI>& y) const
    {
        return Object::Equals(x.Get(), y.Get());
    }
};

_ETL_NAMESPACE_END
#endif

namespace Elastos {
namespace Net {

class CookieStoreImpl
    : public Object
    , public ICookieStore
{
public:
    CAR_INTERFACE_DECL()

    ~CookieStoreImpl();

    CARAPI Add(
        /* [in] */ IURI* uri,
        /* [in] */ IHttpCookie* cookie);

    CARAPI Get(
        /* [in] */ IURI* uri,
        /* [out] */ IList** httpCookies);

    CARAPI GetCookies(
        /* [out] */ IList** httpCookies);

    CARAPI GetURIs(
        /* [out] */ IList** URIs);

    CARAPI Remove(
        /* [in] */ IURI* uri,
        /* [in] */ IHttpCookie* cookie,
        /* [out] */ Boolean* succeeded);

    CARAPI RemoveAll(
        /* [out] */ Boolean* succeeded);

private:
    CARAPI_(AutoPtr<IURI>) GetCookiesUri(
        /* [in] */ IURI* uri);

private:
    /** this map may have null keys! */
    HashMap<AutoPtr<IURI>, AutoPtr<List<AutoPtr<IHttpCookie> > > > mMap;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_COOKIESTOREIMPL_H__
