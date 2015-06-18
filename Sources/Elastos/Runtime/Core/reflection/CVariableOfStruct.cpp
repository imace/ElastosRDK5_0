//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CVariableOfStruct.h"
#include "CStructInfo.h"
#include "CVariableOfCppVector.h"

CVariableOfStruct::CVariableOfStruct()
{
    m_pVarBuf = NULL;
    m_bAlloc = FALSE;
    m_uVarSize = 0;
    m_iCount = 0;
    m_pCppVectorSGetters = NULL;
}

CVariableOfStruct::~CVariableOfStruct()
{
    if (m_bAlloc && m_pVarBuf) free(m_pVarBuf);
    if (m_pCppVectorSGetters) {
        for (Int32 i = 0; i < m_iCount; i++) {
            if (m_pCppVectorSGetters[i]) m_pCppVectorSGetters[i]->Release();
        }
        delete [] m_pCppVectorSGetters;
    }
}

UInt32 CVariableOfStruct::AddRef()
{
    return ElLightRefBase::AddRef();
}

UInt32 CVariableOfStruct::Release()
{
    return ElLightRefBase::Release();
}

PInterface CVariableOfStruct::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IVariableOfStruct *)this;
    }
    else if (riid == EIID_IVariableOfStruct) {
        return (IVariableOfStruct *)this;
    }
    else if (riid == EIID_IStructSetter) {
        return (IStructSetter *)this;
    }
    else if (riid == EIID_IStructGetter) {
        return (IStructGetter *)this;
    }
    return NULL;
}

ECode CVariableOfStruct::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

ECode CVariableOfStruct::Init(
    /* [in] */ IStructInfo *pStructInfo,
    /* [in] */ PVoid pVarBuf)
{
    ECode ec = pStructInfo->GetFieldCount(&m_iCount);
    if (FAILED(ec)) return ec;

    m_pStructInfo = pStructInfo;
    m_pStructFieldDesc = ((CStructInfo *)m_pStructInfo.Get())->m_pStructFieldDesc;
    m_uVarSize = ((CStructInfo *)m_pStructInfo.Get())->m_uSize;
    if (!pVarBuf) {
        m_pVarBuf = (PByte)malloc(m_uVarSize);
        if (m_pVarBuf == NULL) {
            return E_OUT_OF_MEMORY;
        }

        m_bAlloc = TRUE;
    }
    else {
        m_pVarBuf = (PByte)pVarBuf;
        m_bAlloc = FALSE;
    }

    //Get CppVector fields Info
    m_pCppVectorSGetters = new PInterface[m_iCount];
    if (!m_pCppVectorSGetters) {
        delete m_pVarBuf;
        m_pVarBuf = NULL;
        return E_OUT_OF_MEMORY;
    }
    memset(m_pCppVectorSGetters, 0, m_iCount * sizeof(IInterface*));

    return NOERROR;
}

ECode CVariableOfStruct::GetTypeInfo(
    /* [out] */ IDataTypeInfo ** ppTypeInfo)
{
    if (!ppTypeInfo) {
        return E_INVALID_ARGUMENT;
    }

    *ppTypeInfo = m_pStructInfo;
    (*ppTypeInfo)->AddRef();
    return NOERROR;
}

ECode CVariableOfStruct::GetPayload(
    /* [out] */ PVoid * pPayload)
{
    if (!pPayload) {
        return E_INVALID_ARGUMENT;
    }

    *pPayload = m_pVarBuf;

    return NOERROR;
}

ECode CVariableOfStruct::Rebox(
    /* [in] */ PVoid localVariablePtr)
{
    if (!localVariablePtr) {
        return E_INVALID_OPERATION;
    }

    if (m_bAlloc && m_pVarBuf) {
        free(m_pVarBuf);
        m_bAlloc = FALSE;
    }

    m_pVarBuf = (PByte)localVariablePtr;

    if (m_pCppVectorSGetters) {
        for (Int32 i = 0; i < m_iCount; i++) {
            if (m_pCppVectorSGetters[i]) {
                ((CVariableOfCppVector *)(ICppVectorSetter *) \
                    m_pCppVectorSGetters[i])->Rebox(
                    m_pVarBuf + m_pStructFieldDesc[i].pos);
            }
        }
    }

    return NOERROR;
}

ECode CVariableOfStruct::GetSetter(
    /* [out] */ IStructSetter ** ppSetter)
{
    if (!ppSetter) {
        return E_INVALID_ARGUMENT;
    }

    *ppSetter = (IStructSetter *)this;
    (*ppSetter)->AddRef();

    return NOERROR;
}

ECode CVariableOfStruct::GetGetter(
    /* [out] */ IStructGetter ** ppGetter)
{
    if (!ppGetter) {
        return E_INVALID_ARGUMENT;
    }

    *ppGetter = (IStructGetter *)this;
    (*ppGetter)->AddRef();

    return NOERROR;
}

//--------------Setter----------------------------------------------------------

ECode CVariableOfStruct::GetIndexByName(
    /* [in] */ const String& name,
    /* [out] */ Int32 *pIndex)
{
    Int32 count = 0;
    ECode ec = m_pStructInfo->GetFieldCount(&count);
    if (FAILED(ec)) return ec;

    for (Int32 i = 0; i < count; i++) {
        if (name.Equals(m_pStructFieldDesc[i].pszName)) {
            *pIndex = i;
            return NOERROR;
        }
    }

    return E_DOES_NOT_EXIST;
}

ECode CVariableOfStruct::SetFieldValueByName(
    /* [in] */ const String& name,
    /* [in] */ void *pParam,
    /* [in] */ CarDataType type)
{
	if(name.IsNull() || !pParam) {
    	return E_INVALID_ARGUMENT;
    }

    if (!m_pVarBuf) {
        return E_INVALID_OPERATION;
    }

    Int32 index = 0;
    ECode ec = GetIndexByName(name, &index);
    if (FAILED(ec)) return ec;

    if (type != m_pStructFieldDesc[index].type) {
        return E_INVALID_ARGUMENT;
    }

    memcpy(m_pVarBuf + m_pStructFieldDesc[index].pos, pParam,
            m_pStructFieldDesc[index].size);

    return NOERROR;
}

ECode CVariableOfStruct::ZeroAllFields()
{
    if (!m_pVarBuf) {
        return E_INVALID_OPERATION;
    }
    else {
        memset(m_pVarBuf, 0, m_uVarSize);
        return NOERROR;
    }
}

ECode CVariableOfStruct::SetInt16Field(
    /* [in] */ const String& name,
    /* [in] */ Int16 value)
{
    return SetFieldValueByName(name, &value, CarDataType_Int16);
}

ECode CVariableOfStruct::SetInt32Field(
    /* [in] */ const String& name,
    /* [in] */ Int32 value)
{
    return SetFieldValueByName(name, &value, CarDataType_Int32);
}

ECode CVariableOfStruct::SetInt64Field(
    /* [in] */ const String& name,
    /* [in] */ Int64 value)
{
    return SetFieldValueByName(name, &value, CarDataType_Int64);
}

ECode CVariableOfStruct::SetByteField(
    /* [in] */ const String& name,
    /* [in] */ Byte value)
{
    return SetFieldValueByName(name, &value, CarDataType_Byte);
}

ECode CVariableOfStruct::SetFloatField(
    /* [in] */ const String& name,
    /* [in] */ Float value)
{
    return SetFieldValueByName(name, &value, CarDataType_Float);
}

ECode CVariableOfStruct::SetDoubleField(
    /* [in] */ const String& name,
    /* [in] */ Double value)
{
    return SetFieldValueByName(name, &value, CarDataType_Double);
}

ECode CVariableOfStruct::SetChar8Field(
    /* [in] */ const String& name,
    /* [in] */ Char8 value)
{
    return SetFieldValueByName(name, &value, CarDataType_Char8);
}

ECode CVariableOfStruct::SetChar16Field(
    /* [in] */ const String& name,
    /* [in] */ Char16 value)
{
    return SetFieldValueByName(name, &value, CarDataType_Char16);
}

ECode CVariableOfStruct::SetChar32Field(
    /* [in] */ const String& name,
    /* [in] */ Char32 value)
{
    return SetFieldValueByName(name, &value, CarDataType_Char32);
}

ECode CVariableOfStruct::SetBooleanField(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    return SetFieldValueByName(name, &value, CarDataType_Boolean);
}

ECode CVariableOfStruct::SetEMuidField(
    /* [in] */ const String& name,
    /* [in] */ EMuid * pValue)
{
    return SetFieldValueByName(name, &pValue, CarDataType_EMuid);
}

ECode CVariableOfStruct::SetEGuidField(
    /* [in] */ const String& name,
    /* [in] */ EGuid * pValue)
{
    return SetFieldValueByName(name, &pValue, CarDataType_EGuid);
}

ECode CVariableOfStruct::SetECodeField(
    /* [in] */ const String& name,
    /* [in] */ ECode value)
{
    return SetFieldValueByName(name, &value, CarDataType_ECode);
}

ECode CVariableOfStruct::SetLocalPtrField(
    /* [in] */ const String& name,
    /* [in] */ LocalPtr value)
{
    return SetFieldValueByName(name, &value, CarDataType_LocalPtr);
}

ECode CVariableOfStruct::SetLocalTypeField(
    /* [in] */ const String& name,
    /* [in] */ PVoid value)
{
    return SetFieldValueByName(name, value, CarDataType_LocalType);
}

ECode CVariableOfStruct::SetEnumField(
    /* [in] */ const String& name,
    /* [in] */ Int32 value)
{
    return SetFieldValueByName(name, &value, CarDataType_Enum);
}

ECode CVariableOfStruct::GetStructFieldSetter(
    /* [in] */ const String& name,
    /* [out] */ IStructSetter ** ppSetter)
{
    if (name.IsNull() || !ppSetter) {
        return E_INVALID_ARGUMENT;
    }

    Int32 index = 0;
    ECode ec = GetIndexByName(name, &index);
    if (FAILED(ec)) return ec;

    if (m_pStructFieldDesc[index].type != CarDataType_Struct) {
        return E_INVALID_ARGUMENT;
    }

    AutoPtr<IFieldInfo> pFieldInfo;
    AutoPtr<IStructInfo> pStructInfo;
    ec = m_pStructInfo->GetFieldInfo(name, (IFieldInfo**)&pFieldInfo);
    if (FAILED(ec)) return ec;

    ec = pFieldInfo->GetTypeInfo((IDataTypeInfo **)&pStructInfo);
    if (FAILED(ec)) return ec;

    AutoPtr<IVariableOfStruct> pVariable;
    ec = pStructInfo->CreateVariableBox(
        m_pVarBuf + m_pStructFieldDesc[index].pos, (IVariableOfStruct**)&pVariable);
    if (FAILED(ec)) return ec;

    return pVariable->GetSetter(ppSetter);
}

ECode CVariableOfStruct::GetCppVectorFieldSetter(
    /* [in] */ const String& name,
    /* [out] */ ICppVectorSetter ** ppSetter)
{
    return AcquireCppVectorFieldSGetter(name, TRUE, (IInterface**)ppSetter);
}

//--------------Getter----------------------------------------------------------

ECode CVariableOfStruct::GetFieldValueByName(
    /* [in] */ const String& name,
    /* [in] */ void *pParam,
    /* [in] */ CarDataType type)
{
    if(name.IsNull() || !pParam) {
    	return E_INVALID_ARGUMENT;
    }

    if (!m_pVarBuf) {
        return E_INVALID_OPERATION;
    }

    Int32 index = 0;
    ECode ec = GetIndexByName(name, &index);
    if (FAILED(ec)) return ec;

    if (type != m_pStructFieldDesc[index].type) {
        return E_INVALID_ARGUMENT;
    }

    memcpy(pParam, m_pVarBuf + m_pStructFieldDesc[index].pos,
            m_pStructFieldDesc[index].size);

    return NOERROR;
}

ECode CVariableOfStruct::GetInt16Field(
    /* [in] */ const String& name,
    /* [out] */ Int16 * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_Int16);
}

ECode CVariableOfStruct::GetInt32Field(
    /* [in] */ const String& name,
    /* [out] */ Int32 * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_Int32);
}

ECode CVariableOfStruct::GetInt64Field(
    /* [in] */ const String& name,
    /* [out] */ Int64 * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_Int64);
}

ECode CVariableOfStruct::GetByteField(
    /* [in] */ const String& name,
    /* [out] */ Byte * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_Byte);
}

ECode CVariableOfStruct::GetFloatField(
    /* [in] */ const String& name,
    /* [out] */ Float * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_Float);
}

ECode CVariableOfStruct::GetDoubleField(
    /* [in] */ const String& name,
    /* [out] */ Double * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_Double);
}

ECode CVariableOfStruct::GetChar8Field(
    /* [in] */ const String& name,
    /* [out] */ Char8 * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_Char8);
}

ECode CVariableOfStruct::GetChar16Field(
    /* [in] */ const String& name,
    /* [out] */ Char16 * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_Char16);
}

ECode CVariableOfStruct::GetChar32Field(
    /* [in] */ const String& name,
    /* [out] */ Char32 * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_Char32);
}

ECode CVariableOfStruct::GetBooleanField(
    /* [in] */ const String& name,
    /* [out] */ Boolean * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_Boolean);
}

ECode CVariableOfStruct::GetEMuidField(
    /* [in] */ const String& name,
    /* [out] */ EMuid * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_EMuid);
}

ECode CVariableOfStruct::GetEGuidField(
    /* [in] */ const String& name,
    /* [out] */ EGuid * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_EGuid);
}

ECode CVariableOfStruct::GetECodeField(
    /* [in] */ const String& name,
    /* [out] */ ECode * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_ECode);
}

ECode CVariableOfStruct::GetLocalPtrField(
    /* [in] */ const String& name,
    /* [out] */ LocalPtr * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_LocalPtr);
}

ECode CVariableOfStruct::GetLocalTypeField(
    /* [in] */ const String& name,
    /* [out] */ PVoid value)
{
    return GetFieldValueByName(name, value, CarDataType_LocalType);
}

ECode CVariableOfStruct::GetEnumField(
    /* [in] */ const String& name,
    /* [out] */ Int32 * pValue)
{
    return GetFieldValueByName(name, pValue, CarDataType_Enum);
}

ECode CVariableOfStruct::GetStructFieldGetter(
    /* [in] */ const String& name,
    /* [out] */ IStructGetter ** ppGetter)
{
    if (name.IsNull() || !ppGetter) {
        return E_INVALID_ARGUMENT;
    }

    Int32 index = 0;
    ECode ec = GetIndexByName(name, &index);
    if (FAILED(ec)) return ec;

    if (m_pStructFieldDesc[index].type != CarDataType_Struct) {
        return E_INVALID_ARGUMENT;
    }

    AutoPtr<IFieldInfo> pFieldInfo;
    AutoPtr<IStructInfo> pStructInfo;
    ec = m_pStructInfo->GetFieldInfo(name, (IFieldInfo **)&pFieldInfo);
    if (FAILED(ec)) return ec;

    ec = pFieldInfo->GetTypeInfo((IDataTypeInfo **)&pStructInfo);
    if (FAILED(ec)) return ec;

    AutoPtr<IVariableOfStruct> pVariable;
    ec = pStructInfo->CreateVariableBox(
        m_pVarBuf + m_pStructFieldDesc[index].pos, (IVariableOfStruct **)&pVariable);
    if (FAILED(ec)) return ec;

    return pVariable->GetGetter(ppGetter);
}

ECode CVariableOfStruct::GetCppVectorFieldGetter(
    /* [in] */ const String& name,
    /* [out] */ ICppVectorGetter ** ppGetter)
{
    return AcquireCppVectorFieldSGetter(name, FALSE, (IInterface**)ppGetter);
}

ECode CVariableOfStruct::AcquireCppVectorFieldSGetter(
    /* [in] */ const String& name,
    /* [in] */ Boolean bSetter,
    /* [out] */ IInterface ** ppSGetter)
{
    if (name.IsNull() || !ppSGetter) {
        return E_INVALID_ARGUMENT;
    }

    Int32 index = 0;
    ECode ec = GetIndexByName(name, &index);
    if (FAILED(ec)) return ec;

    if (m_pStructFieldDesc[index].type != CarDataType_CppVector) {
        return E_INVALID_ARGUMENT;
    }

    assert(m_pCppVectorSGetters);

    g_objInfoList.LockHashTable(EntryType_Struct);

    CVariableOfCppVector *pSGetter = (CVariableOfCppVector *)
                        (IVariableOfCarArray *)m_pCppVectorSGetters[index];
    if (!pSGetter) {
        AutoPtr<IFieldInfo> pFieldInfo;
        ec = m_pStructInfo->GetFieldInfo(name, (IFieldInfo**)&pFieldInfo);
        if (FAILED(ec)) {
            g_objInfoList.UnlockHashTable(EntryType_Struct);
            return ec;
        }

        AutoPtr<ICppVectorInfo> pTypeInfo;
        pFieldInfo->GetTypeInfo((IDataTypeInfo**)&pTypeInfo);
        if (FAILED(ec)) {
            g_objInfoList.UnlockHashTable(EntryType_Struct);
            return ec;
        }

        Int32 length = 0;
        ec = pTypeInfo->GetLength(&length);
        if (FAILED(ec)) {
            g_objInfoList.UnlockHashTable(EntryType_Struct);
            return ec;
        }

        AutoPtr<IDataTypeInfo> pElementTypeInfo;
        ec = pTypeInfo->GetElementTypeInfo((IDataTypeInfo**)&pElementTypeInfo);
        if (FAILED(ec)) {
            g_objInfoList.UnlockHashTable(EntryType_Struct);
            return ec;
        }

        pSGetter = new CVariableOfCppVector(
                                    pElementTypeInfo, length,
                                    m_pVarBuf + m_pStructFieldDesc[index].pos);
        if (pSGetter == NULL) {
            g_objInfoList.UnlockHashTable(EntryType_Struct);
            return E_OUT_OF_MEMORY;
        }

        ec = pSGetter->Init();
        if (FAILED(ec)) {
            delete pSGetter;
            g_objInfoList.UnlockHashTable(EntryType_Struct);
            return ec;
        }

        pSGetter->AddRef();
        m_pCppVectorSGetters[index] = (IVariableOfCarArray *)pSGetter;
    }

    if (bSetter) {
        pSGetter->GetSetter((ICppVectorSetter**)ppSGetter);
    }
    else {
        pSGetter->GetGetter((ICppVectorGetter**)ppSGetter);
    }

    g_objInfoList.UnlockHashTable(EntryType_Struct);

    return NOERROR;
}
