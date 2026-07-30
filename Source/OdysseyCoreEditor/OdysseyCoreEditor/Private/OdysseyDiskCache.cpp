// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyDiskCache.h"

#include "DerivedDataCacheInterface.h"
#include "DerivedDataCache.h"
#include "DerivedDataValue.h"
#include "DerivedDataRequestTypes.h"
#include "DerivedDataRequestOwner.h"
#include "HAL/PlatformTime.h"

FOdysseyDiskCache::~FOdysseyDiskCache()
{
}

FOdysseyDiskCache::FOdysseyDiskCache(const FString& iCacheName, const FString& iCacheVersion)
    : mCacheName(iCacheName)
    , mCacheVersion(iCacheVersion)
{
}

void
FOdysseyDiskCache::Save(const FString& iId, const FSharedBuffer& iBuffer)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyDiskCache::Save);
    FString CacheKey = FDerivedDataCacheInterface::BuildCacheKey(
        *mCacheName,
        *mCacheVersion, //a GUID identifying the version of the key
        iId
    );

    UE::DerivedData::FValue derivedDataValue = UE::DerivedData::FValue::Compress(iBuffer);

    //Store the tile in cache
    UE::DerivedData::FRequestOwner putOwner(UE::DerivedData::EPriority::Highest);

    UE::DerivedData::GetCache().PutValue(
        {
            UE::DerivedData::FCachePutValueRequest
            {
                UE::FSharedString(TEXT("FOdysseyDiskCache")),
                UE::DerivedData::ConvertLegacyCacheKey(CacheKey),
                MoveTemp(derivedDataValue),
                UE::DerivedData::ECachePolicy::StoreLocal //Use "StoreLocal" instead of "Local" to store and override existing value
            }
        },
        putOwner
    );
    putOwner.KeepAlive();
}

bool
FOdysseyDiskCache::Load(const FString& iId, FUniqueBuffer& oBuffer)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyDiskCache::Load);

    bool success = false;

    // put code you want to time here.

    //Load Block from DDC
    FString CacheKey = FDerivedDataCacheInterface::BuildCacheKey(
        *mCacheName,
        *mCacheVersion, //a GUID identifying the version of the key
        iId
    );

    UE::DerivedData::FRequestOwner getOwner(UE::DerivedData::EPriority::Blocking);
    UE::DerivedData::GetCache().GetValue(
        {
            UE::DerivedData::FCacheGetValueRequest
            {
                UE::FSharedString(TEXT("FOdysseyDiskCache")),
                UE::DerivedData::ConvertLegacyCacheKey(CacheKey),
                UE::DerivedData::ECachePolicy::Local
            }
        },
        getOwner,
        [&, this](UE::DerivedData::FCacheGetValueResponse&& iResponse)
        {
            if (iResponse.Status != UE::DerivedData::EStatus::Ok)
                return;

            if ( !iResponse.Value.HasData() || iResponse.Value.GetRawSize() == 1) //assume the block is empty, see RemoveValueFromCache()
                return;

            //FUniqueBuffer uniqueBuffer = FUniqueBuffer::MakeView(oBlock->Bits(), oBlock->BytesTotal());

            oBuffer = FUniqueBuffer::Alloc(iResponse.Value.GetRawSize());
            if ( !iResponse.Value.GetData().TryDecompressTo(oBuffer) )
                return;

            //oBuffer = uniqueBuffer.MoveToShared();
            success = true;
        }
    );
    getOwner.Wait();
    return success;
}

void
FOdysseyDiskCache::Remove(const FString& iId)
{
    FString CacheKey = FDerivedDataCacheInterface::BuildCacheKey(
        *mCacheName,
        *mCacheVersion, //a GUID identifying the version of the key
        iId
    );

    uint8 dummy = 0;
    FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(&dummy, 1);
    UE::DerivedData::FValue derivedDataValue = UE::DerivedData::FValue::Compress(sharedBuffer);

    //Store the tile in cache
    UE::DerivedData::FRequestOwner putOwner(UE::DerivedData::EPriority::Blocking);
    UE::DerivedData::GetCache().PutValue(
        {
            {
                UE::FSharedString(), //Not needed
                UE::DerivedData::ConvertLegacyCacheKey(CacheKey),
                MoveTemp(derivedDataValue),
                UE::DerivedData::ECachePolicy::StoreLocal //Use "StoreLocal" instead of "Local" to store and override existing value
            }
        },
        putOwner
    );
    putOwner.Wait();
}
