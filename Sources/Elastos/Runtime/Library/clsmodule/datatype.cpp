//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <stdio.h>
#include <assert.h>

#include "clsbase.h"

int GetOriginalType(
    /* [in] */ const CLSModule* module,
    /* [in] */ const TypeDescriptor* srcDescriptor,
    /* [in] */ TypeDescriptor* destDescriptor)
{
    destDescriptor->nPointer = srcDescriptor->nPointer;
    destDescriptor->bUnsigned = srcDescriptor->bUnsigned;

    while (srcDescriptor->mType == Type_alias) {
        srcDescriptor = &module->mAliasDirs[srcDescriptor->sIndex]->type;
        destDescriptor->nPointer += srcDescriptor->nPointer;
        destDescriptor->bUnsigned |= srcDescriptor->bUnsigned;
    }

    destDescriptor->mType = srcDescriptor->mType;
    destDescriptor->sIndex = srcDescriptor->sIndex;
    destDescriptor->pNestedType = srcDescriptor->pNestedType;

    _ReturnOK(CLS_NoError);
}

int GetArrayBaseType(
    /* [in] */ const CLSModule* module,
    /* [in] */ const TypeDescriptor* srcDescriptor,
    /* [in] */ TypeDescriptor* destDescriptor)
{
    TypeDescriptor* type = (TypeDescriptor *)srcDescriptor;

    while (Type_Array == type->mType) {
        type = &module->mArrayDirs[type->sIndex]->type;
    }

    memcpy(destDescriptor, type, sizeof(TypeDescriptor));

    _ReturnOK(CLS_NoError);
}

BOOL IsEqualType(
    /* [in] */ const CLSModule* module,
    /* [in] */ const TypeDescriptor* descriptor1,
    /* [in] */ const TypeDescriptor* descriptor2)
{
    TypeDescriptor src, dest;

    GetOriginalType(module, descriptor1, &src);
    GetOriginalType(module, descriptor2, &dest);

    _Return (!memcmp(&src, &dest, sizeof(src)));
}
