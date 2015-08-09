//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <stdlib.h>
#include <lube.h>

int LubeContext::ModuleMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    switch (member) {
        case Member_Type:
            if (m_pModule->mAttribs & CARAttrib_library) {
                pszOutput = "library";
            }
            else {
                pszOutput = "module";
            }
            break;
        case Member_Name:
            pszOutput = m_pModule->mName;
            break;
        case Member_Attrib:
            sprintf(pszBuffer, "%08x", m_pModule->mAttribs);
            return LUBE_OK;
        case Member_Version:
            sprintf(pszBuffer, "%d.%d", m_pModule->mMajorVersion, m_pModule->mMinorVersion);
            return LUBE_OK;
        case Member_Major_Version:
            sprintf(pszBuffer, "%d", m_pModule->mMajorVersion);
            return LUBE_OK;
        case Member_Minor_Version:
            sprintf(pszBuffer, "%d", m_pModule->mMinorVersion);
            return LUBE_OK;
        case Member_CarCode:
            sprintf(pszBuffer, "%08x", m_pModule->mChecksum);
            return LUBE_OK;
        case Member_BuildDate:
            sprintf(pszBuffer, "%08x", m_pModule->mBarcode);
            return LUBE_OK;
        case Member_Uuid:
            pszOutput = Uuid2CString(&m_pModule->mUuid, TRUE);
            break;
        case Member_Uunm:
            if (m_pModule->mUunm) {
                sprintf(pszBuffer, "\"%s\"", m_pModule->mUunm);
                return LUBE_OK;
            }
            pszOutput = "(null uunm)";
            break;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::LibraryMember(MemberType member, char *pszBuffer)
{
    char *p;

    assert(NULL != m_pLibrary);

    p = m_pLibrary + strlen(m_pLibrary);
    while (p != m_pLibrary && !IS_PATH_SEPARATOR(*(p - 1)))
        p--;

    if (Member_Name == member) {
        while (*p && strcasecmp(".eco", p) != 0) *pszBuffer++= *p++;
        *pszBuffer = 0;
    }
    else if (Member_Type == member) {
        while (*p && '.' != *p) p++;
        if (*p) while (*++p) *pszBuffer = *p;
        *pszBuffer = 0;
    }
    else {
        return LUBE_FAIL;
    }

    return LUBE_OK;
}

int LubeContext::ClassMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pClass);

    switch (member) {
        case Member_Type:
            if (m_pClass->mDesc->dwAttribs & ClassAttrib_t_generic) {
                pszOutput = "generic";
            }
            else if (m_pClass->mDesc->dwAttribs & ClassAttrib_t_aspect) {
                pszOutput = "aspect";
            }
            else if (m_pClass->mDesc->dwAttribs & ClassAttrib_t_regime) {
                pszOutput = "regime";
            }
            else {
                pszOutput = "class";
            }
            break;
        case Member_Name:
            pszOutput = m_pClass->mName;
            break;
        case Member_NameSpace:
            if (m_pClass->mNameSpace != NULL && m_pClass->mNameSpace[0] != '\0' &&
                strcmp(m_pClass->mNameSpace, "systypes")) {
                char *pszNamespace = (char*)malloc(strlen(m_pClass->mNameSpace) + 1);
                strcpy(pszNamespace, m_pClass->mNameSpace);
                char buffer[1024];
                buffer[0] = '\0';
                char *begin = pszNamespace;
                while (begin != NULL) {
                    char *dot = strchr(begin, '.');
                    if (dot != NULL) *dot = '\0';
                    strcat(buffer, begin);
                    strcat(buffer, "::");
                    if (dot != NULL) begin = dot + 1;
                    else begin = NULL;
                }
                free(pszNamespace);
                sprintf(pszBuffer, "%s", buffer);
            }
            return LUBE_OK;
        case Member_FullName:
            char buffer[1024];
            buffer[0] = '\0';
            if (m_pClass->mNameSpace != NULL && m_pClass->mNameSpace[0] != '\0' &&
                strcmp(m_pClass->mNameSpace, "systypes")) {
                char *pszNamespace = (char*)malloc(strlen(m_pClass->mNameSpace) + 1);
                strcpy(pszNamespace, m_pClass->mNameSpace);
                char *begin = pszNamespace;
                while (begin != NULL) {
                    char *dot = strchr(begin, '.');
                    if (dot != NULL) *dot = '\0';
                    strcat(buffer, begin);
                    strcat(buffer, "_");
                    if (dot != NULL) begin = dot + 1;
                    else begin = NULL;
                }
                free(pszNamespace);
            }
            strcat(buffer, m_pClass->mName);
            sprintf(pszBuffer, "%s", buffer);
            return LUBE_OK;
        case Member_Attrib:
            sprintf(pszBuffer, "%08x", m_pClass->mDesc->dwAttribs);
            return LUBE_OK;
        case Member_Uuid:
            pszOutput = Uuid2CString(&m_pClass->mDesc->clsid, TRUE);
            break;
        case Member_Clsid:
            if (m_pModule->mUunm) {
                sprintf(pszBuffer,
                    "    {%s, \\\n"
                    "    \"%s\"}",
                    Uuid2CString(&m_pClass->mDesc->clsid, TRUE),
                    m_pModule->mUunm);
                return LUBE_OK;
            }
            pszOutput = "(null uunm)";
            break;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::InterfaceMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pInterface);

    switch (member) {
        case Member_Type:
            pszOutput = "interface";
            break;
        case Member_Name:
            pszOutput = m_pInterface->mName;
            break;
        case Member_NameSpace:
            if (m_pInterface->mNameSpace != NULL && m_pInterface->mNameSpace[0] != '\0' &&
                strcmp(m_pInterface->mNameSpace, "systypes")) {
                char *pszNamespace = (char*)malloc(strlen(m_pInterface->mNameSpace) + 1);
                strcpy(pszNamespace, m_pInterface->mNameSpace);
                char buffer[1024];
                buffer[0] = '\0';
                char *begin = pszNamespace;
                while (begin != NULL) {
                    char *dot = strchr(begin, '.');
                    if (dot != NULL) *dot = '\0';
                    strcat(buffer, begin);
                    strcat(buffer, "::");
                    if (dot != NULL) begin = dot + 1;
                    else begin = NULL;
                }
                free(pszNamespace);
                sprintf(pszBuffer, "%s", buffer);
            }
            return LUBE_OK;
        case Member_FullName:
            char buffer[1024];
            buffer[0] = '\0';
            if (m_pInterface->mNameSpace != NULL && m_pInterface->mNameSpace[0] != '\0' &&
                strcmp(m_pInterface->mNameSpace, "systypes")) {
                char *pszNamespace = (char*)malloc(strlen(m_pInterface->mNameSpace) + 1);
                strcpy(pszNamespace, m_pInterface->mNameSpace);
                char *begin = pszNamespace;
                while (begin != NULL) {
                    char *dot = strchr(begin, '.');
                    if (dot != NULL) *dot = '\0';
                    strcat(buffer, begin);
                    strcat(buffer, "_");
                    if (dot != NULL) begin = dot + 1;
                    else begin = NULL;
                }
                free(pszNamespace);
            }
            strcat(buffer, m_pInterface->mName);
            sprintf(pszBuffer, "%s", buffer);
            return LUBE_OK;
        case Member_Attrib:
            sprintf(pszBuffer, "%08x", m_pInterface->mDesc->dwAttribs);
            return LUBE_OK;
        case Member_Iid:
        case Member_Uuid:
            pszOutput = Uuid2CString(&m_pInterface->mDesc->iid, TRUE);
            break;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::StructMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pStruct);

    switch (member) {
        case Member_Type:
            pszOutput = "struct";
            break;
        case Member_Name:
            pszOutput = m_pStruct->mName;
            break;
        case Member_Attrib:
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::ConstMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pConst);

    switch (member) {
        case Member_Type:
            pszOutput = "const";
            break;
        case Member_Name:
            pszOutput = m_pConst->mName;
            break;
        case Member_Value:
            if (m_pConst->type == TYPE_INTEGER32) {
                sprintf(pszBuffer, (m_pConst->v.intValue.bHexFormat ? "0x%08x":"%d"),
                    m_pConst->v.intValue.nValue);
            }
            else {
                assert(m_pConst->type == TYPE_STRING);
                sprintf(pszBuffer, "\"%s\"", m_pConst->v.strValue.pszValue);
            }
            return LUBE_OK;
        case Member_Attrib:
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::EnumMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pEnum);

    switch (member) {
        case Member_Type:
            pszOutput = "enum";
            break;
        case Member_Name:
            pszOutput = m_pEnum->mName;
            break;
        case Member_FullName:
            char buffer[1024];
            buffer[0] = '\0';
            if (m_pEnum->mNameSpace != NULL && m_pEnum->mNameSpace[0] != '\0' &&
                strcmp(m_pEnum->mNameSpace, "systypes")) {
                char *pszNamespace = (char*)malloc(strlen(m_pEnum->mNameSpace) + 1);
                strcpy(pszNamespace, m_pEnum->mNameSpace);
                char *begin = pszNamespace;
                while (begin != NULL) {
                    char *dot = strchr(begin, '.');
                    if (dot != NULL) *dot = '\0';
                    strcat(buffer, begin);
                    strcat(buffer, "_");
                    if (dot != NULL) begin = dot + 1;
                    else begin = NULL;
                }
                free(pszNamespace);
            }
            strcat(buffer, m_pEnum->mName);
            sprintf(pszBuffer, "%s", buffer);
            return LUBE_OK;
        case Member_Attrib:
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::TypedefMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pTypedef);

    switch (member) {
        case Member_Type:
            pszOutput = Type2CString(m_pModule, &m_pTypedef->type);
            break;
        case Member_Name:
            pszOutput = m_pTypedef->mName;
            break;
        case Member_Attrib:
            if (m_pTypedef->bDummyType) {
                strcpy(pszBuffer, "dummytype");
            }
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::InterfaceConstMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pInterfaceConst);

    switch (member) {
        case Member_Type:
            {
                unsigned char type = m_pInterfaceConst->type;
                if (type == TYPE_BOOLEAN) {
                    pszOutput = "Boolean";
                }
                else if (type == TYPE_CHAR32) {
                    pszOutput = "Char32";
                }
                else if (type == TYPE_BYTE) {
                    pszOutput = "Byte";
                }
                else if (type == TYPE_INTEGER16) {
                    pszOutput = "Int16";
                }
                else if (type == TYPE_INTEGER32) {
                    pszOutput = "Int32";
                }
                else if (type == TYPE_INTEGER64) {
                    pszOutput = "Int64";
                }
                else if (type == TYPE_FLOAT) {
                    pszOutput = "Float";
                }
                else if (type == TYPE_DOUBLE) {
                    pszOutput = "Double";
                }
                else {
                    assert(type == TYPE_STRING);
                    pszOutput = "String";
                }
                break;
            }
        case Member_Name:
            pszOutput = m_pInterfaceConst->mName;
            break;
        case Member_Value:
            {
                InterfaceConstDescriptor *pDesc = m_pInterfaceConst;
                if (pDesc->type == TYPE_BOOLEAN) {
                    sprintf(pszBuffer, pDesc->v.bValue ? "TRUE" : "FALSE");
                }
                else if (pDesc->type == TYPE_CHAR32) {
                    if (pDesc->v.int32Value.format == FORMAT_DECIMAL) {
                        sprintf(pszBuffer, "%d", pDesc->v.int32Value.nValue);
                    }
                    else if (pDesc->v.int32Value.format == FORMAT_HEX) {
                        sprintf(pszBuffer, "0x%08x", pDesc->v.int32Value.nValue);
                    }
                    else {
                        switch(pDesc->v.int32Value.nValue) {
                            case '\a' :
                                sprintf(pszBuffer, "\'\\a\'");
                                break;
                            case 'b' :
                                sprintf(pszBuffer, "\'\\b\'");
                                break;
                            case 'f' :
                                sprintf(pszBuffer, "\'\\f\'");
                                break;
                            case 'n' :
                                sprintf(pszBuffer, "\'\\n\'");
                                break;
                            case 'r' :
                                sprintf(pszBuffer, "\'\\r\'");
                                break;
                            case 't' :
                                sprintf(pszBuffer, "\'\\t\'");
                                break;
                            case 'v' :
                                sprintf(pszBuffer, "\'\\v\'");
                                break;
                            case '\\':
                                sprintf(pszBuffer, "\'\\\\\'");
                                break;
                            case '\'':
                                sprintf(pszBuffer, "\'\\\'\'");
                                break;
                            case '\"':
                                sprintf(pszBuffer, "\'\\\"\'");
                                break;
                            case '0' :
                                sprintf(pszBuffer, "\'\\0\'");
                                break;
                            default:
                                sprintf(pszBuffer, "\'%c\'", pDesc->v.int32Value.nValue);
                        }
                    }
                }
                else if (pDesc->type == TYPE_BYTE) {
                    sprintf(pszBuffer, pDesc->v.int32Value.format == FORMAT_HEX ? "0x%08x":"%d",
                            (unsigned char)pDesc->v.int32Value.nValue);
                }
                else if (pDesc->type == TYPE_INTEGER16) {
                    sprintf(pszBuffer, pDesc->v.int32Value.format == FORMAT_HEX ? "0x%08x":"%d",
                            (short)pDesc->v.int32Value.nValue);
                }
                else if (pDesc->type == TYPE_INTEGER32) {
                    sprintf(pszBuffer, pDesc->v.int32Value.format == FORMAT_HEX ? "0x%08x":"%d",
                            pDesc->v.int32Value.nValue);
                }
                else if (pDesc->type == TYPE_INTEGER64) {
                    sprintf(pszBuffer, pDesc->v.int64Value.format == FORMAT_HEX ? "0x%llx":"%lld",
                            pDesc->v.int64Value.nValue);
                }
                else if (pDesc->type == TYPE_FLOAT) {
                    sprintf(pszBuffer, "%f", pDesc->v.dValue);
                }
                else if (pDesc->type == TYPE_DOUBLE) {
                    sprintf(pszBuffer, "%f", pDesc->v.dValue);
                }
                else {
                    assert(pDesc->type == TYPE_STRING);
                    sprintf(pszBuffer, "String(\"%s\")", pDesc->v.pStrValue);
                }
                return LUBE_OK;
            }
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::MethodMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pMethod);

    switch (member) {
        case Member_Type:
            pszOutput = Type2CString(m_pModule, &m_pMethod->type);
            break;
        case Member_Name:
            pszOutput = m_pMethod->mName;
            break;
        case Member_Attrib:
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::ParamMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput = NULL;
    TypeDescriptor type;

    assert(NULL != m_pParam);

    switch (member) {
        case Member_Type:
            if (Type_struct == m_pParam->type.mType
                    || Type_EMuid == m_pParam->type.mType
                    || Type_EGuid == m_pParam->type.mType) {
                pszOutput = StructType2CString(m_pModule, &m_pParam->type);
            }
            else if (Type_alias == m_pParam->type.mType) {
                GetOriginalType(m_pModule, &m_pParam->type, &type);
                if ((Type_EMuid == type.mType)
                        || (Type_EGuid == type.mType)
                        || (Type_struct == type.mType)) {
                    pszOutput = StructType2CString(m_pModule, &m_pParam->type);
                }
                else if (Type_ArrayOf == type.mType) {
                    if (m_pParam->dwAttribs & ParamAttrib_in) {
                        if (0 == type.mPointer) {
                            strcpy(pszBuffer, "const ");
                            strcat(pszBuffer, Type2CString(m_pModule, &m_pParam->type));
                            strcat(pszBuffer, " &");
                            return LUBE_OK;
                        }
                        else {
                            assert(1 == type.mPointer);
                            strcpy(pszBuffer, Type2CString(m_pModule, &m_pParam->type));
                            return LUBE_OK;
                        }
                    }
                    if (m_pParam->dwAttribs & ParamAttrib_out) {
                        strcpy(pszBuffer, Type2CString(m_pModule, &m_pParam->type));
                        strcpy(pszBuffer, " *");
                        return LUBE_OK;
                    }
                }
                else {
                    pszOutput = Type2CString(m_pModule, &m_pParam->type);
                }
            }
            else if (Type_ArrayOf == m_pParam->type.mType) {
                if ((m_pParam->dwAttribs & ParamAttrib_in)
                    && (0 == m_pParam->type.mPointer)) pszOutput = "const ArrayOf";
                else pszOutput = "ArrayOf";
                strcpy(pszBuffer, pszOutput);
                strcat(pszBuffer, "<");
                strcat(pszBuffer, Type2CString(m_pModule, m_pParam->type.mNestedType));
                strcat(pszBuffer, ">");
                if (m_pParam->dwAttribs & ParamAttrib_in) {
                    if (0 == m_pParam->type.mPointer) strcat(pszBuffer, " &");
                    else {
                       assert (1 == m_pParam->type.mPointer);
                       strcat(pszBuffer, " *");
                    }
                }
                else if (0 == m_pParam->type.mPointer) strcat(pszBuffer, " *");
                else strcat(pszBuffer, " **");
                return LUBE_OK;
            }
            else {
                pszOutput = Type2CString(m_pModule, &m_pParam->type);
            }
            break;
        case Member_Name:
            pszOutput = m_pParam->mName;
            break;
        case Member_Attrib:
            pszOutput = ParamAttrib2String(m_pParam->dwAttribs);
            break;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::StructMemMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;
    const TypeDescriptor *pType;
    TypeDescriptor baseType;

    assert(NULL != m_pStructMember);

    switch (member) {
        case Member_Type:
            pType = &m_pStructMember->type;
            if (Type_Array == pType->mType) {
                //Get base type of Array
                memset(&baseType, 0, sizeof(TypeDescriptor));
                GetArrayBaseType(m_pModule, pType, &baseType);
                pszOutput = Type2CString(m_pModule, &baseType);
            }
            else {
                pszOutput = Type2CString(m_pModule, &m_pStructMember->type);
            }
            break;
        case Member_Name:
            pszOutput = m_pStructMember->mName;
            break;
        case Member_Dimention:
            //Handle the case of [m][n]...
            if (Type_Array == m_pStructMember->type.mType) {
                pszOutput = Dims2CString(m_pModule,  &m_pStructMember->type);
                break;
            }
            else {
                pszBuffer[0] = 0;
                return LUBE_OK;
            }
        case Member_Attrib:
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::EnumMemMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pEnumMember);

    switch (member) {
        case Member_Type:
        case Member_Attrib:
            return LUBE_OK;
        case Member_Name:
            pszOutput = m_pEnumMember->mName;
            break;
        case Member_Value:
            sprintf(pszBuffer, (m_pEnumMember->bHexFormat ? "0x%08x":"%d"),
                m_pEnumMember->nValue);
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::ClassParentMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pClass);

    int nRet;
    ClassDirEntry *pOrigClass;

    if (!(m_pClass->mDesc->dwAttribs & ClassAttrib_hasparent)) return LUBE_OK;

    pOrigClass = m_pClass;
    m_pClass = m_pClassParent;
    nRet = ClassMember(member, pszBuffer);

    m_pClass = pOrigClass;
    return nRet;
}

int LubeContext::AspectMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pAspect);

    int nRet;
    ClassDirEntry *pOrigClass;

    pOrigClass = m_pClass;
    m_pClass = m_pAspect;
    nRet = ClassMember(member, pszBuffer);

    m_pClass = pOrigClass;
    return nRet;
}

int LubeContext::AggregateMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pAggregate);

    int nRet;
    ClassDirEntry *pOrigClass;

    pOrigClass = m_pClass;
    m_pClass = m_pAggregate;
    nRet = ClassMember(member, pszBuffer);

    m_pClass = pOrigClass;
    return nRet;
}

int LubeContext::ClassForAspectMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pClass);

    int nRet;
    ClassDirEntry *pOrigClass;

    pOrigClass = m_pClass;
    m_pClass = m_pForClass;
    nRet = ClassMember(member, pszBuffer);

    m_pClass = pOrigClass;
    return nRet;
}

int LubeContext::IntfParentMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pInterface);

    int nRet;
    InterfaceDirEntry *pOrigInterface;

    pOrigInterface = m_pInterface;
    m_pInterface = m_pIntfParent;
    nRet = InterfaceMember(member, pszBuffer);

    m_pInterface = pOrigInterface;
    return nRet;
}

int LubeContext::IntfParentParentMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pInterface);

    int nRet;
    InterfaceDirEntry *pOrigInterface;

    pOrigInterface = m_pInterface;
    if (0 != m_pInterface->mDesc->sParentIndex) {
        m_pInterface = this->m_pModule->mInterfaceDirs[m_pIntfParent->mDesc->sParentIndex];
    }
    nRet = InterfaceMember(member, pszBuffer);

    m_pInterface = pOrigInterface;
    return nRet;
}

int LubeContext::IntfChildMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pInterface);

    int nRet;
    InterfaceDirEntry *pOrigInterface;

    pOrigInterface = m_pInterface;
    m_pInterface = m_pIntfChild;
    nRet = InterfaceMember(member, pszBuffer);

    m_pInterface = pOrigInterface;
    return nRet;
}

int LubeContext::GetMemberString(
    ObjectType object, MemberType member, char *pszBuffer)
{
    *pszBuffer = 0;

    switch (object) {
        case Object_Module:
            return this->ModuleMember(member, pszBuffer);
        case Object_Library:
            return this->LibraryMember(member, pszBuffer);
        case Object_Class:
            return this->ClassMember(member, pszBuffer);
        case Object_Aspect:
            return this->AspectMember(member, pszBuffer);
        case Object_Aggregate:
            return this->AggregateMember(member, pszBuffer);
        case Object_ClassForAspect:
            return this->ClassForAspectMember(member, pszBuffer);
        case Object_ClsIntf:
        case Object_Interface:
        case Object_ClsIntfAndParents:
            return this->InterfaceMember(member, pszBuffer);
        case Object_Struct:
            return this->StructMember(member, pszBuffer);
        case Object_Enum:
            return this->EnumMember(member, pszBuffer);
        case Object_Const:
            return this->ConstMember(member, pszBuffer);
        case Object_Typedef:
            return this->TypedefMember(member, pszBuffer);
        case Object_IntfConst:
            return this->InterfaceConstMember(member, pszBuffer);
        case Object_ClassMethod:
        case Object_ClassConstructor:
        case Object_CbCoalescence:
        case Object_IntfMethod:
            return this->MethodMember(member, pszBuffer);
        case Object_Param:
            return this->ParamMember(member, pszBuffer);
        case Object_StcMember:
            return this->StructMemMember(member, pszBuffer);
        case Object_EnumMember:
            return this->EnumMemMember(member, pszBuffer);
        case Object_Super:
            return this->ClassParentMember(member, pszBuffer);
        case Object_Parent:
            return this->IntfParentMember(member, pszBuffer);
        case Object_Child:
            return this->IntfChildMember(member, pszBuffer);
        case Object_IntfParentParent:
            return this->IntfParentParentMember(member, pszBuffer);
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
}
