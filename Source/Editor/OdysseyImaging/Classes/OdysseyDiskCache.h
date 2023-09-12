// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

/*
struct FBlockCleanupInfo
{
    FGuid mId;
    int mWidth;
    int mHeight;
    int mFormat;
    bool mIsCacheInvalid;
    TSharedPtr<FThreadSafeCounter> mAvailableCounter;
};
*/

class ODYSSEYIMAGING_API FOdysseyDiskCache
{
public:
    // Construction / Destruction
    ~FOdysseyDiskCache();
    FOdysseyDiskCache(const FString& iCacheName, const FString& iCacheVersion);

public:
    //--- Block Caching / Loading
    
    //Loads a memory from cache
    bool Load(const FString& iId, FUniqueBuffer& oBuffer);

    //Saves the given Memory to cache
    void Save(const FString& iId, const FSharedBuffer& iBuffer);

    void Remove(const FString& iId);

private:
    FString mCacheName;
    FString mCacheVersion;
};
