// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyTileCacheSession.h"

#include "GenericPlatform/GenericPlatformFile.h"
#include "GenericPlatform/GenericPlatformMutex.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/PlatformProcess.h"
#include "Logging/LogMacros.h"
#include "Misc/Guid.h"
#include "Misc/Paths.h"
#include "Misc/Timespan.h"

#include <type_traits>

DEFINE_LOG_CATEGORY_STATIC(LogOdysseyTileCacheSession, Log, All);

namespace
{
constexpr TCHAR ManagedSessionPrefix[] = TEXT("Session-v1-");
}

FOdysseyTileCacheSession::FOdysseyTileCacheSession(const FString& InRootDirectory)
    : RootDirectory(FPaths::ConvertRelativePathToFull(InRootDirectory))
    , AcquireLock(&FOdysseyTileCacheSession::AcquirePlatformLock)
{
    FPaths::NormalizeDirectoryName(RootDirectory);
}

FOdysseyTileCacheSession::~FOdysseyTileCacheSession()
{
    Finalize();
}

TUniquePtr<UE::FPlatformSystemWideMutex> FOdysseyTileCacheSession::AcquirePlatformLock(const FString& SessionName)
{
    if constexpr (!std::is_same_v<UE::FPlatformSystemWideMutex, UE::FPlatformSystemWideMutexNotImplemented>)
    {
        TUniquePtr<UE::FPlatformSystemWideMutex> Lock = MakeUnique<UE::FPlatformSystemWideMutex>(
            TEXT("OdysseyTileCache-") + SessionName, FTimespan::Zero());
        if (Lock->IsValid())
        {
            return Lock;
        }
    }
    return nullptr;
}

bool FOdysseyTileCacheSession::IsManagedSessionName(const FString& SessionName)
{
    if (!SessionName.StartsWith(ManagedSessionPrefix, ESearchCase::CaseSensitive))
    {
        return false;
    }
    const FString Identity = SessionName.RightChop(UE_ARRAY_COUNT(ManagedSessionPrefix) - 1);
    FString ProcessId;
    FString GuidString;
    FGuid Guid;
    return Identity.Split(TEXT("-"), &ProcessId, &GuidString)
        && !ProcessId.IsEmpty() && ProcessId.IsNumeric()
        && FGuid::ParseExact(GuidString, EGuidFormats::Digits, Guid);
}

void FOdysseyTileCacheSession::Initialize()
{
    if (bInitialized)
    {
        return;
    }

    FString SessionName = FString::Printf(TEXT("%s%u-%s"), ManagedSessionPrefix,
        FPlatformProcess::GetCurrentProcessId(), *FGuid::NewGuid().ToString(EGuidFormats::Digits));
    SessionLock = AcquireLock(SessionName);
    if (SessionLock)
    {
        CleanupAbandonedSessions();
    }
    else
    {
        // A directory without verified ownership must never be reclaimed by another session.
        SessionName = TEXT("Unmanaged-v1-") + SessionName.RightChop(UE_ARRAY_COUNT(ManagedSessionPrefix) - 1);
        UE_LOG(LogOdysseyTileCacheSession, Warning, TEXT("Interprocess locking unavailable; abandoned tile caches will be preserved."));
    }

    SessionDirectory = FPaths::Combine(RootDirectory, SessionName);
    CacheFilePath = FPaths::Combine(SessionDirectory, TEXT("TempCache.dat"));
    bInitialized = true;
    if (!IFileManager::Get().MakeDirectory(*SessionDirectory, true))
    {
        UE_LOG(LogOdysseyTileCacheSession, Warning, TEXT("Could not create tile cache directory: %s"), *SessionDirectory);
    }
}

void FOdysseyTileCacheSession::CleanupAbandonedSessions()
{
    TArray<FString> SessionNames;
    IFileManager::Get().FindFiles(SessionNames, *FPaths::Combine(RootDirectory, TEXT("Session-v1-*")), false, true);
    for (const FString& SessionName : SessionNames)
    {
        if (!IsManagedSessionName(SessionName))
        {
            continue;
        }
        // Failure includes both a live owner and an unavailable lock backend. Neither permits deletion.
        TUniquePtr<UE::FPlatformSystemWideMutex> Lock = AcquireLock(SessionName);
        if (Lock)
        {
            DeleteSessionDirectory(FPaths::Combine(RootDirectory, SessionName));
        }
    }
}

void FOdysseyTileCacheSession::DeleteSessionDirectory(const FString& Directory) const
{
    const FString AbsoluteDirectory = FPaths::ConvertRelativePathToFull(Directory);
    if (!FPaths::IsSamePath(FPaths::GetPath(AbsoluteDirectory), RootDirectory)
        || FPlatformFileManager::Get().GetPlatformFile().IsSymlink(*AbsoluteDirectory) == ESymlinkResult::Symlink)
    {
        return;
    }
    if (!IFileManager::Get().DeleteDirectory(*AbsoluteDirectory, false, true))
    {
        UE_LOG(LogOdysseyTileCacheSession, Warning, TEXT("Could not remove tile cache directory: %s"), *AbsoluteDirectory);
    }
}

void FOdysseyTileCacheSession::Finalize()
{
    if (!bInitialized)
    {
        return;
    }
    // Keep ownership until deletion has finished, including on normal shutdown.
    DeleteSessionDirectory(SessionDirectory);
    SessionLock.Reset();
    bInitialized = false;
}
