#ifndef __CWALLPAPERBACKUPHELPERHELPER_H__
#define __CWALLPAPERBACKUPHELPERHELPER_H__

#include "_CWallpaperBackupHelperHelper.h"
#include "ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CWallpaperBackupHelperHelper)
{
public:
    CARAPI GetWALLPAPER_IMAGE(
        /* [out] */ String* image);

    CARAPI GetWALLPAPER_INFO(
        /* [out] */ String* info);
};
}
}
}
}

#endif
