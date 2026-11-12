// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "Containers/UnrealString.h"
#include "HAL/PlatformMutex.h"
#include "Templates/Function.h"
#include "Templates/UniquePtr.h"

/** Owns one disk-cache directory and reclaims abandoned sessions through an interprocess lock. */
class FOdysseyTileCacheSession
{
public:
    explicit FOdysseyTileCacheSession(const FString& InRootDirectory);
    ~FOdysseyTileCacheSession();

    FOdysseyTileCacheSession(const FOdysseyTileCacheSession&) = delete;
    FOdysseyTileCacheSession& operator=(const FOdysseyTileCacheSession&) = delete;

    /** Acquire ownership before publishing the directory. Failed locking disables abandoned-session cleanup. */
    void Initialize();
    void Finalize();

    const FString& GetCacheFilePath() const { return CacheFilePath; }
    bool HasSessionLock() const { return SessionLock.IsValid(); }

private:
    static TUniquePtr<UE::FPlatformSystemWideMutex> AcquirePlatformLock(const FString& SessionName);
    static bool IsManagedSessionName(const FString& SessionName);
    void CleanupAbandonedSessions();
    void DeleteSessionDirectory(const FString& Directory) const;

    FString RootDirectory;
    FString SessionDirectory;
    FString CacheFilePath;
    TUniquePtr<UE::FPlatformSystemWideMutex> SessionLock;
    TFunction<TUniquePtr<UE::FPlatformSystemWideMutex>(const FString&)> AcquireLock;
    bool bInitialized = false;

#if WITH_DEV_AUTOMATION_TESTS
    friend struct FOdysseyTileCacheSessionTestAccess;
#endif
};
