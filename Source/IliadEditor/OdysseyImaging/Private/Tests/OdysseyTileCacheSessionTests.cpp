// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyTileCacheSession.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "HAL/FileManager.h"
#include "HAL/PlatformMisc.h"
#include "HAL/PlatformProcess.h"
#include "HAL/PlatformTime.h"
#include "Misc/AutomationTest.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"

struct FOdysseyTileCacheSessionTestAccess
{
    static void DisableLocking(FOdysseyTileCacheSession& Session)
    {
        Session.AcquireLock = [](const FString&) -> TUniquePtr<UE::FPlatformSystemWideMutex>
        {
            return nullptr;
        };
    }
};

namespace Odyssey::TileCacheTests
{
constexpr EAutomationTestFlags TestFlags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;

// Test-only subprocess mode runs before the editor starts ticking. The parent can kill it without running destructors.
bool RunWorkerIfRequested()
{
    FString Root;
    if (!FParse::Value(FCommandLine::Get(), TEXT("OdysseyTileCacheWorkerRoot="), Root))
    {
        return false;
    }

    FOdysseyTileCacheSession Session(Root);
    Session.Initialize();
    if (!Session.HasSessionLock())
    {
        FPlatformMisc::RequestExitWithStatus(true, 2);
        return true;
    }

    const FString ReadyFile = FPaths::Combine(Root, TEXT("Worker.ready"));
    const FString PendingReadyFile = ReadyFile + TEXT(".pending");
    const FString StopFile = FPaths::Combine(Root, TEXT("Worker.stop"));
    if (!FFileHelper::SaveStringToFile(TEXT("cache payload"), *Session.GetCacheFilePath())
        || !FFileHelper::SaveStringToFile(Session.GetCacheFilePath(), *PendingReadyFile)
        || !IFileManager::Get().Move(*ReadyFile, *PendingReadyFile))
    {
        FPlatformMisc::RequestExitWithStatus(true, 3);
        return true;
    }

    const double Deadline = FPlatformTime::Seconds() + 120.0;
    while (!IFileManager::Get().FileExists(*StopFile) && FPlatformTime::Seconds() < Deadline)
    {
        FPlatformProcess::Sleep(0.02f);
    }
    // The graceful test explicitly exercises the same shutdown operation used by the tile manager.
    Session.Finalize();
    FPlatformMisc::RequestExitWithStatus(true, 0);
    return true;
}

class FSessionFixture
{
public:
    FSessionFixture()
        : Root(FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectIntermediateDir(),
            TEXT("OdysseyTileCacheTests"), FGuid::NewGuid().ToString(EGuidFormats::Digits))))
    {
        IFileManager::Get().MakeDirectory(*Root, true);
    }

    ~FSessionFixture()
    {
        StopWorker();
        IFileManager::Get().DeleteDirectory(*Root, false, true);
    }

    bool StartWorker(FAutomationTestBase& Test)
    {
        const FString Args = FString::Printf(
            TEXT("\"%s\" -unattended -nullrhi -nosound -nosplash -nop4 -NoAssetRegistryCache -OdysseyTileCacheWorkerRoot=\"%s\" -abslog=\"%s\""),
            *FPaths::ConvertRelativePathToFull(FPaths::GetProjectFilePath()), *Root, *FPaths::Combine(Root, TEXT("Worker.log")));
        Worker = FPlatformProcess::CreateProc(FPlatformProcess::ExecutablePath(), *Args, false, true, true,
            nullptr, 0, nullptr, nullptr);
        if (!Test.TestTrue(TEXT("Start cache worker"), Worker.IsValid()))
        {
            return false;
        }

        const FString ReadyFile = FPaths::Combine(Root, TEXT("Worker.ready"));
        const double Deadline = FPlatformTime::Seconds() + 45.0;
        bool bReady = false;
        while (FPlatformProcess::IsProcRunning(Worker) && FPlatformTime::Seconds() < Deadline)
        {
            if (IFileManager::Get().FileExists(*ReadyFile)
                && FFileHelper::LoadFileToString(WorkerCachePath, *ReadyFile) && !WorkerCachePath.IsEmpty())
            {
                bReady = true;
                break;
            }
            FPlatformProcess::Sleep(0.02f);
        }
        if (!bReady)
        {
            FString WorkerLog;
            FFileHelper::LoadFileToString(WorkerLog, *FPaths::Combine(Root, TEXT("Worker.log")));
            Test.AddError(TEXT("Cache worker failed to become ready. Log tail:\n") + WorkerLog.Right(6000));
        }
        return Test.TestTrue(TEXT("Worker acquired its lock and published its cache"), bReady);
    }

    void StopWorker()
    {
        if (Worker.IsValid())
        {
            FPlatformProcess::TerminateProc(Worker);
            FPlatformProcess::WaitForProc(Worker);
            FPlatformProcess::CloseProc(Worker);
            Worker.Reset();
        }
    }

    FString Root;
    FString WorkerCachePath;
    FProcHandle Worker;
};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTileCacheCrashTest, "Odyssey.Imaging.TileCacheSession.CrashAndLiveOwner", Odyssey::TileCacheTests::TestFlags)
bool FOdysseyTileCacheCrashTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TileCacheTests;
    FSessionFixture Fixture;
    if (!Fixture.StartWorker(*this))
    {
        return false;
    }

    FOdysseyTileCacheSession FirstCleaner(Fixture.Root);
    FirstCleaner.Initialize();
    if (!TestTrue(TEXT("Parent acquired a session lock"), FirstCleaner.HasSessionLock()))
    {
        return false;
    }
    TestTrue(TEXT("A running process's cache survives cleanup"), IFileManager::Get().FileExists(*Fixture.WorkerCachePath));
    TestTrue(TEXT("Create parent cache payload"), FFileHelper::SaveStringToFile(TEXT("live parent"), *FirstCleaner.GetCacheFilePath()));

    Fixture.StopWorker(); // No Finalize, destructors or crash callbacks execute in the child.
    TestTrue(TEXT("Crash leaves a cache to reclaim"), IFileManager::Get().FileExists(*Fixture.WorkerCachePath));
    FOdysseyTileCacheSession NextSession(Fixture.Root);
    NextSession.Initialize();
    TestFalse(TEXT("Next launch removes the crashed process's directory"), IFileManager::Get().DirectoryExists(*FPaths::GetPath(Fixture.WorkerCachePath)));
    TestTrue(TEXT("Cleanup preserves another live session"), IFileManager::Get().FileExists(*FirstCleaner.GetCacheFilePath()));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTileCacheNormalExitTest, "Odyssey.Imaging.TileCacheSession.NormalExit", Odyssey::TileCacheTests::TestFlags)
bool FOdysseyTileCacheNormalExitTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TileCacheTests;
    FSessionFixture Fixture;
    if (!Fixture.StartWorker(*this))
    {
        return false;
    }
    TestTrue(TEXT("Request graceful cache shutdown"), FFileHelper::SaveStringToFile(TEXT("stop"), *FPaths::Combine(Fixture.Root, TEXT("Worker.stop"))));
    const double Deadline = FPlatformTime::Seconds() + 15.0;
    while (FPlatformProcess::IsProcRunning(Fixture.Worker) && FPlatformTime::Seconds() < Deadline)
    {
        FPlatformProcess::Sleep(0.02f);
    }
    TestFalse(TEXT("Worker exited"), FPlatformProcess::IsProcRunning(Fixture.Worker));
    TestFalse(TEXT("Normal shutdown removed its directory"), IFileManager::Get().DirectoryExists(*FPaths::GetPath(Fixture.WorkerCachePath)));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTileCacheLockFailureTest, "Odyssey.Imaging.TileCacheSession.LockFailureAndLegacy", Odyssey::TileCacheTests::TestFlags)
bool FOdysseyTileCacheLockFailureTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TileCacheTests;
    FSessionFixture Fixture;
    // An unlocked but well-formed directory represents an abandoned managed session.
    const FString AbandonedDirectory = FPaths::Combine(Fixture.Root, TEXT("Session-v1-0-") + FGuid::NewGuid().ToString(EGuidFormats::Digits));
    const FString LegacyDirectory = FPaths::Combine(Fixture.Root, TEXT("old-session-without-ownership-protocol"));
    IFileManager::Get().MakeDirectory(*AbandonedDirectory, true);
    IFileManager::Get().MakeDirectory(*LegacyDirectory, true);

    FOdysseyTileCacheSession Unmanaged(Fixture.Root);
    FOdysseyTileCacheSessionTestAccess::DisableLocking(Unmanaged);
    AddExpectedMessage(TEXT("Interprocess locking unavailable"), ELogVerbosity::Warning);
    Unmanaged.Initialize();
    TestFalse(TEXT("Failed locking does not claim ownership"), Unmanaged.HasSessionLock());
    TestTrue(TEXT("Failed locking preserves abandoned directories"), IFileManager::Get().DirectoryExists(*AbandonedDirectory));
    TestTrue(TEXT("Unmanaged session is explicitly marked"), FPaths::GetCleanFilename(FPaths::GetPath(Unmanaged.GetCacheFilePath())).StartsWith(TEXT("Unmanaged-v1-")));

    FOdysseyTileCacheSession Cleaner(Fixture.Root);
    Cleaner.Initialize();
    if (!TestTrue(TEXT("Platform supports interprocess locking"), Cleaner.HasSessionLock()))
    {
        return false;
    }
    TestFalse(TEXT("Owned cleanup removes abandoned managed directories"), IFileManager::Get().DirectoryExists(*AbandonedDirectory));
    TestTrue(TEXT("Cleanup preserves legacy directories"), IFileManager::Get().DirectoryExists(*LegacyDirectory));
    TestTrue(TEXT("Cleanup preserves unlocked unmanaged sessions"), IFileManager::Get().DirectoryExists(*FPaths::GetPath(Unmanaged.GetCacheFilePath())));
    Cleaner.Finalize();
    TestFalse(TEXT("Normal cleanup removes its own directory"), IFileManager::Get().DirectoryExists(*FPaths::GetPath(Cleaner.GetCacheFilePath())));
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
