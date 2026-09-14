// IDDN.FR.001.060015.015.S.X.2019.000.00000
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
    bool ProbablyExists(const FString& iId) const;

    //Loads a memory from cache and creates a new buffer
    bool Load(const FString& iId, FUniqueBuffer& oBuffer) const;

    //Loads a memory from cache into an existing buffer
    bool LoadInto(const FString& iId, FUniqueBuffer& oBuffer) const;

    //Saves the given Memory to cache
    void Save(const FString& iId, const FSharedBuffer& iBuffer) const;

    void Remove(const FString& iId) const;

private:
    FString mCacheName;
    FString mCacheVersion;
};
