
#include "ext/frameworkext.h"
#include <elastos/Map.h>
#include <pthread.h>

namespace Elastos {
namespace Utility {

    const RBTreeColorType S_RBTreeRed = FALSE;
    const RBTreeColorType S_RBTreeBlack = TRUE;
    // {C6F9F8F0-9887-492B-87DA-4A8D61762F91}
    extern "C" const ClassID ECLSID_CDummyObject =
        {{0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
        (char *)"elastos.dll"};

} // namespace Utility
} // namespace Elastos

#ifdef DROID_CORE
pthread_key_t sLooperKey;
Boolean sLooperKeyInited = FALSE;
#endif

