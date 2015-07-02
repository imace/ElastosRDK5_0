//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CAR_FACTORY_H__
#define __CAR_FACTORY_H__

extern "C" {

typedef interface IClassObject IClassObject;
typedef interface IClassObject *PClassObject;

#ifdef _UNDEFDLLEXP
#define DLLEXP
#else
#define DLLEXP __declspec(dllimport)
#endif

EXTERN_C DLLEXP const _ELASTOS InterfaceID EIID_IClassObject;
EXTERN_C DLLEXP const _ELASTOS ClassID ECLSID_CClassObject;

CAR_INTERFACE("00000001-0000-0000-C000-000000000046")
IClassObject : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
            /* [in] */ _ELASTOS REIID riid) = 0;

    virtual CARAPI CreateObject(
            /* [in] */ PInterface pOuter,
            /* [in] */ _ELASTOS REIID riid,
            /* [out] */ PInterface *ppObject) = 0;

    virtual CARAPI StayResident(
            /* [in] */ _ELASTOS Boolean bIsStayResident) = 0;
};

typedef _ELASTOS ECode (CARAPICALLTYPE *_CreateObjectFn)(
        PInterface *ppObject);

class _CBaseClassObject :
    public IObject,
    public IClassObject
{
public:
    CARAPI_(PInterface) Probe(
                /* [in] */ _ELASTOS REIID riid);

    CARAPI_(_ELASTOS UInt32) AddRef();

    CARAPI_(_ELASTOS UInt32) Release();

    CARAPI GetInterfaceID(
                /* [in] */ IInterface *pObject,
                /* [out] */ _ELASTOS InterfaceID *pIID);

    CARAPI Aggregate(
                /* [in] */ AggrType type,
                /* [in] */ PInterface pObj);

    CARAPI GetDomain(
                /* [out] */ PInterface *ppObj);

    CARAPI GetClassID(
                /* [out] */ _ELASTOS ClassID *pCLSID);

    CARAPI CreateObject(
                /* [in] */ PInterface pOuter,
                /* [in] */ _ELASTOS REIID riid,
                /* [out] */ PInterface *ppObject);

    CARAPI StayResident(
                /* [in] */ _ELASTOS Boolean bIsStayResident);

    _CBaseClassObject(_CreateObjectFn fn) : m_fnCreateObject(fn) {}

private:
    _CreateObjectFn m_fnCreateObject;
};

struct _IInterface : IInterface {};

typedef enum _SingletonObjState_
{
    _SingletonObjState_Uninitialize,
    _SingletonObjState_Initializing,
    _SingletonObjState_Initialized,
} _SingletonObjState_;

typedef enum _InitComponentFlags_
{
    _InitComponentFlags_InitSyncLock            = 0x1,
    _InitComponentFlags_IncrementDllLockCount   = 0x2,
    _InitComponentFlags_InitCallbackLock        = 0x4,
} _InitComponentFlags_;

ELAPI_(void) IncrementDllLockCount();
ELAPI_(void) DecrementDllLockCount();

}   // extern "C"

#endif // __CAR_FACTORY_H__
