// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Memory/SharedBuffer.h"

class ODYSSEYCOREEDITOR_API FOdysseyDiskCache
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
