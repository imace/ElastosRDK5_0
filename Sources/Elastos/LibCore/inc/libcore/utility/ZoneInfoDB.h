#ifndef __LIBCORE_UTILITY_ZONEINFODB_H__
#define __LIBCORE_UTILITY_ZONEINFODB_H__

#include <Elastos.CoreLibrary_server.h>

using Libcore::IO::IMemoryMappedFile;
using Libcore::IO::IBufferIterator;

namespace Libcore {
namespace Utility {

/**
 * A class used to initialize the time zone database. This implementation uses the
 * Olson tzdata as the source of time zone information. However, to conserve
 * disk space (inodes) and reduce I/O, all the data is concatenated into a single file,
 * with an index to indicate the starting position of each time zone record.
 *
 * @hide - used to implement TimeZone
 */
class ZoneInfoDB
{
public:
    class TzData :
        public ElRefBase
    {
    public:
        TzData();

        void Init(
            /* [in] */ ArrayOf<String>* paths);

        AutoPtr<ArrayOf<String> > GetAvailableIDs();

        AutoPtr<ArrayOf<String> > GetAvailableIDs(
            /* [in] */ Int32 rawUtcOffset);

    private:
        Boolean LoadData(
            /* [in] */ const String& path);

        CARAPI ReadHeader();

        CARAPI ReadZoneTab(
            /* [in] */ IBufferIterator* it,
            /* [in] */ Int32 zoneTabOffset,
            /* [in] */ Int32 zoneTabSize);

        CARAPI ReadIndex(
            /* [in] */ IBufferIterator* it,
            /* [in] */ Int32 indexOffset,
            /* [in] */ Int32 dataOffset);

        AutoPtr<ArrayOf<Int32> > GetRawUtcOffsets();

        String GetVersion();

        String GetZoneTab();

        AutoPtr<IZoneInfo> MakeTimeZone(
            /* [in] */ const String& id);

    private:
        class MyBasicLruCache
            : public ElRefBase
            //: public BasicLruCache
        {
        protected:
            AutoPtr<IZoneInfo> Create(
                /* [in] */ const String& id);

            virtual CARAPI_(AutoPtr<IInterface>) Create(
                /* [in] */ IInterface* key);
        };
    private:
        /**
         * Rather than open, read, and close the big data file each time we look up a time zone,
         * we map the big data file during startup, and then just use the MemoryMappedFile.
         *
         * At the moment, this "big" data file is about 500 KiB. At some point, that will be small
         * enough that we could just keep the byte[] in memory, but using mmap(2) like this has the
         * nice property that even if someone replaces the file under us (because multiple gservices
         * updates have gone out, say), we still get a consistent (if outdated) view of the world.
         */
        AutoPtr<IMemoryMappedFile> mMappedFile;

        String mVersion;
        String mZoneTab;

        /**
         * The 'ids' array contains time zone ids sorted alphabetically, for binary searching.
         * The other two arrays are in the same order. 'byteOffsets' gives the byte offset
         * of each time zone, and 'rawUtcOffsetsCache' gives the time zone's raw UTC offset.
         */
        AutoPtr<ArrayOf<String> > mIds;
        AutoPtr<ArrayOf<Int32> > mByteOffsets;
        AutoPtr<ArrayOf<Int32> > mRawUtcOffsetsCache;// Access this via getRawUtcOffsets instead.

        /**
         * ZoneInfo objects are worth caching because they are expensive to create.
         * See http://b/8270865 for context.
         */
        static const Int32 CACHE_SIZE = 1;

        AutoPtr<MyBasicLruCache> mCache;
    };

public:
    static AutoPtr<TzData> GetInstance();

private:
    static AutoPtr<TzData> DATA;

private:
    ZoneInfoDB();
    ZoneInfoDB(const ZoneInfoDB&);
};

} // namespace Utility
} // namespace Libcore

#endif //__LIBCORE_UTILITY_ZONEINFODB_H__
