
#include "os/storage/CStorageVolume.h"
#include <elastos/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

const String CStorageVolume::EXTRA_STORAGE_VOLUME = String("storage_volume");

CStorageVolume::CStorageVolume()
    : mStorageId(0)
    , mDescriptionId(0)
    , mPrimary(FALSE)
    , mRemovable(FALSE)
    , mEmulated(FALSE)
    , mMtpReserveSpace(0)
    , mAllowMassStorage(0)
    , mMaxFileSize(0)
{}

CStorageVolume::~CStorageVolume()
{}

ECode CStorageVolume::constructor()
{
    return NOERROR;
}

ECode CStorageVolume::constructor(
    /* [in] */ IFile* path,
    /* [in] */ Int32 descriptionId,
    /* [in] */ Boolean primary,
    /* [in] */ Boolean removable,
    /* [in] */ Boolean emulated,
    /* [in] */ Int32 mtpReserveSpace,
    /* [in] */ Boolean allowMassStorage,
    /* [in] */ Int64 maxFileSize,
    /* [in] */ IUserHandle* owner)
{
    mPath = path;
    mDescriptionId = descriptionId;
    mPrimary = primary;
    mRemovable = removable;
    mEmulated = emulated;
    mMtpReserveSpace = mtpReserveSpace;
    mAllowMassStorage = allowMassStorage;
    mMaxFileSize = maxFileSize;
    mOwner = owner;
    return NOERROR;
}

//AutoPtr<IStorageVolume> CStorageVolume::FromTemplate(
//    /* [in] */ IStorageVolume* temp,
//    /* [in] */ IFile* path,
//    /* [in] */ IUserHandle* owner)
//{
//    AutoPtr<IStorageVolume> result;
//    Int32 descriptionId, mtpReserveSpace;
//    Int64 maxFileSize;
//    Boolean primary, removable, emulated, allowMassStorage;
//
//    temp->GetDescriptionId(&descriptionId);
//    temp->IsPrimary(&primary);
//    temp->IsRemovable(&removable);
//    temp->IsEmulated(&emulated);
//    temp->GetMtpReserveSpace(&mtpReserveSpace);
//    temp->AllowMassStorage(&allowMassStorage);
//    temp->GetMaxFileSize(&maxFileSize);
//
//    CStorageVolume::New(path, descriptionId, primary, removable, emulated, mtpReserveSpace,
//            allowMassStorage, maxFileSize, owner, (IStorageVolume**)&result);
//
//    return result;
//}

/**
* Returns the mount path for the volume.
*
* @return the mount path
*/
ECode CStorageVolume::GetPath(
    /* [out] */ String* pMPath)
{
    VALIDATE_NOT_NULL(pMPath);
    return mPath->ToString(pMPath);
}

ECode CStorageVolume::GetPathFile(
    /* [out] */ IFile** pPathFile)
{
    VALIDATE_NOT_NULL(pPathFile);
    *pPathFile = mPath;
    INTERFACE_ADDREF(*pPathFile);
    return NOERROR;
}

/**
* Returns a user visible description of the volume.
*
* @return the volume description
*/
ECode CStorageVolume::GetDescription(
    /* [in] */ IContext * pContext,
    /* [out] */ String * pDesc)
{
    VALIDATE_NOT_NULL(pDesc);
    AutoPtr<IResources> res;
    pContext->GetResources((IResources**)&res);
    return res->GetString(mDescriptionId, pDesc);
}

ECode CStorageVolume::GetDescriptionId(
    /* [out] */ Int32 * pMDescriptionId)
{
    VALIDATE_NOT_NULL(pMDescriptionId);
    *pMDescriptionId = mDescriptionId;
    return NOERROR;
}

ECode CStorageVolume::IsPrimary(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPrimary;
    return NOERROR;
}

/**
* Returns true if the volume is removable.
*
* @return is removable
*/
ECode CStorageVolume::IsRemovable(
    /* [out] */ Boolean * pMRemovable)
{
    VALIDATE_NOT_NULL(pMRemovable);
    *pMRemovable = mRemovable;
    return NOERROR;
}

/**
* Returns true if the volume is emulated.
*
* @return is removable
*/
ECode CStorageVolume::IsEmulated(
    /* [out] */ Boolean * pMEmulated)
{
    VALIDATE_NOT_NULL(pMEmulated);
    *pMEmulated = mEmulated;
    return NOERROR;
}

/**
* Returns the MTP storage ID for the volume.
* this is also used for the storage_id column in the media provider.
*
* @return MTP storage ID
*/
ECode CStorageVolume::GetStorageId(
    /* [out] */ Int32 * pMStorageId)
{
    VALIDATE_NOT_NULL(pMStorageId);
    *pMStorageId = mStorageId;
    return NOERROR;
}

/**
* Do not call this unless you are MountService
*/
ECode CStorageVolume::SetStorageId(
    /* [in] */ Int32 index)
{
// storage ID is 0x00010001 for primary storage,
// then 0x00020001, 0x00030001, etc. for secondary storages
    mStorageId = ((index + 1) << 16) + 1;
    return NOERROR;
}

/**
* Number of megabytes of space to leave unallocated by MTP.
* MTP will subtract this value from the free space it reports back
* to the host via GetStorageInfo, and will not allow new files to
* be added via MTP if there is less than this amount left free in the storage.
* If MTP has dedicated storage this value should be zero, but if MTP is
* sharing storage with the rest of the system, set this to a positive value
* to ensure that MTP activity does not result in the storage being
* too close to full.
*
* @return MTP reserve space
*/
ECode CStorageVolume::GetMtpReserveSpace(
    /* [out] */ Int32 * pMMtpReserveSpace)
{
    VALIDATE_NOT_NULL(pMMtpReserveSpace);
    *pMMtpReserveSpace = mMtpReserveSpace;
    return NOERROR;
}

/**
* Returns true if this volume can be shared via USB mass storage.
*
* @return whether mass storage is allowed
*/
ECode CStorageVolume::AllowMassStorage(
    /* [out] */ Boolean * pMAllowMassStorage)
{
    VALIDATE_NOT_NULL(pMAllowMassStorage);
    *pMAllowMassStorage = mAllowMassStorage;
    return NOERROR;
}

/**
* Returns maximum file size for the volume, or zero if it is unbounded.
*
* @return maximum file size
*/
ECode CStorageVolume::GetMaxFileSize(
    /* [out] */ Int64 * pMMaxFileSize)
{
    VALIDATE_NOT_NULL(pMMaxFileSize);
    *pMMaxFileSize = mMaxFileSize;
    return NOERROR;
}

ECode  CStorageVolume::GetOwner(
    /* [out] */ IUserHandle** pOwner)
{
    VALIDATE_NOT_NULL(pOwner);
    *pOwner = mOwner;
    INTERFACE_ADDREF(*pOwner);
    return NOERROR;
}

ECode CStorageVolume::EqualsEx(
    /* [in] */ IStorageVolume * pObj,
    /* [out] */ Boolean * pEqu)
{
    VALIDATE_NOT_NULL(pEqu);
    *pEqu = FALSE;
    VALIDATE_NOT_NULL(pObj);

    if (NULL != mPath) {
        String path, curPath;
        pObj->GetPath(&path);
        GetPath(&curPath);
        if (!curPath.IsNull()) {
            *pEqu = curPath.Equals(path);
        }
    }

    return NOERROR;
}

ECode CStorageVolume::Equals(
    /* [in] */ IInterface * pObj,
    /* [out] */ Boolean * pEqu)
{
    VALIDATE_NOT_NULL(pEqu);
    *pEqu = FALSE;
    VALIDATE_NOT_NULL(pObj);

    return EqualsEx(IStorageVolume::Probe(pObj), pEqu);
}

ECode CStorageVolume::GetHashCode(
   /* [out] */ Int32* pCode)
{
    VALIDATE_NOT_NULL(pCode);
   return mPath->GetHashCode(pCode);
}

ECode CStorageVolume::ToString(
    /* [out] */ String * pStr)
{
    StringBuilder builder("StorageVolume [");
    builder += String("mStorageId=");
    builder += mStorageId;
    builder += String(" mPath=");
    builder += mPath;
    builder += String(" mDescriptionId=");
    builder += mDescriptionId;
    builder += String(" mPrimary=");
    builder += mPrimary;
    builder += String(" mRemovable=");
    builder += mRemovable;
    builder += String(" mEmulated=");
    builder += mEmulated;
    builder += String(" mMtpReserveSpace=");
    builder += mMtpReserveSpace;
    builder += String(" mAllowMassStorage=");
    builder += mAllowMassStorage;
    builder += String(" mMaxFileSize=");
    builder += mMaxFileSize;
    builder += String(" mOwner=");
    builder += mOwner;
    builder += String("]");
    *pStr = builder.ToString();
    return NOERROR;
}

ECode CStorageVolume::DescribeContents(
    /* [out] */ Int32 * pContents)
{
    VALIDATE_NOT_NULL(pContents);
    *pContents = 0;
    return NOERROR;
}

ECode CStorageVolume::WriteToParcel(
    /* [in] */ IParcel * pParcel)
{
    pParcel->WriteInt32(mStorageId);

    String str;
    mPath->ToString(&str);
    pParcel->WriteString(str);

    pParcel->WriteInt32(mDescriptionId);
    pParcel->WriteInt32(mPrimary ? 1 : 0);
    pParcel->WriteInt32(mRemovable ? 1 : 0);
    pParcel->WriteInt32(mEmulated ? 1 : 0);
    pParcel->WriteInt32(mMtpReserveSpace);
    pParcel->WriteInt32(mAllowMassStorage ? 1 : 0);
    pParcel->WriteInt64(mMaxFileSize);
//    pParcel->WriteParcelable(mOwner, flags);
    return NOERROR;
}

ECode CStorageVolume::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    VALIDATE_NOT_NULL(in);

    Int32 num;
    String str;

    in->ReadInt32(&mStorageId);

    in->ReadString(&str);
    CFile::New(str, (IFile**)&mPath);

    in->ReadInt32(&mDescriptionId);

    in->ReadInt32(&num);
    mPrimary = (num != 0)? TRUE : FALSE;

    in->ReadInt32(&num);
    mRemovable = (num != 0)? TRUE : FALSE;

    in->ReadInt32(&num);
    mEmulated = (num != 0)? TRUE : FALSE;

    in->ReadInt32(&mMtpReserveSpace);

    in->ReadInt32(&num);
    mAllowMassStorage = (num != 0)? TRUE : FALSE;

    in->ReadInt64(&mMaxFileSize);

//    mOwner = in->ReadParcelable(NULL);
    return NOERROR;
}

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos
