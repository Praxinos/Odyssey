// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyTiledImage.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HAL/FileManager.h"
#include "HAL/Event.h"
#include "HAL/PlatformTime.h"
#include "HAL/PlatformProcess.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "RenderingThread.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

#include "OdysseyTileManager.h"
#include "OdysseyTileUtils.h"
#include "OdysseyTileManagerSettings.h"

struct FOdysseyTileManagerTestAccess
{
    static uint64 UploadCount() { return FOdysseyTileManager::Get().GPUUploadCount; }
    static void Budget(uint64 Bytes)
    {
        FOdysseyTileManager::Get().SetGPUCacheBudget(Bytes);
        FlushRenderingCommands();
    }
    static void EvictCPU() { FOdysseyTileManager::FTileData::EvictTiles(0, 0); }
    static int32 AtlasCount() { return FOdysseyTileManager::Get().GPUAtlases.Num(); }
    static bool IsCompacted()
    {
        FOdysseyTileManager& Manager = FOdysseyTileManager::Get();
        FScopeLock Lock(&Manager.GPUCacheMutex);
        for (const auto& Pair : Manager.GPUAtlases)
        {
            if (!Pair.Value->IsCacheAtlas())
            {
                return false;
            }
        }
        return true;
    }
    static bool IsCached(FOdysseyTileId Id, uint32 Size = 32, EPixelFormat Format = PF_R8G8B8A8)
    {
        FOdysseyTileManager& Manager = FOdysseyTileManager::Get();
        const TSharedPtr<FOdysseyTileManager::FTile> Tile = Manager.Tiles[Id.Index];
        FScopeLock TileLock(&Tile->Mutex);
        const TSharedPtr<FOdysseyTileManager::FTileData> Data = Tile->TileData.IsReady() ? Tile->TileData.Get() : nullptr;
        FScopeLock Lock(&Manager.GPUCacheMutex);
        return Tile->Id == Id && Data && Manager.GPUTextures.Contains({Data.Get(), Size, Format});
    }
    static void Poll(FOdysseyTileManager& Manager, FOdysseyTileId Id)
    {
        Manager.TryLoadTileFromReadBack(Manager.Tiles[Id.Index]);
    }
};

namespace Odyssey::TiledImageTests
{
constexpr EAutomationTestFlags TestFlags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;

FColor GetPixel(int32 X, int32 Y, uint8 Seed)
{
    return FColor((X * 7 + Y * 3 + Seed) % 251, (X * 5 + Y * 11 + Seed) % 251, (X + Y * 13 + Seed) % 251, 255);
}

TStrongObjectPtr<UTexture2D> MakeSource(int32 Width, int32 Height, uint8 Seed, bool bEmpty = false)
{
    TStrongObjectPtr<UTexture2D> Texture(UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8));
    Texture->SRGB = false;
    uint8* Bytes = static_cast<uint8*>(Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            const FColor Pixel = bEmpty ? FColor::Transparent : GetPixel(X, Y, Seed);
            const int32 Offset = (Y * Width + X) * 4;
            Bytes[Offset] = Pixel.R;
            Bytes[Offset + 1] = Pixel.G;
            Bytes[Offset + 2] = Pixel.B;
            Bytes[Offset + 3] = Pixel.A;
        }
    }
    Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
    Texture->UpdateResource();
    FlushRenderingCommands();
    return Texture;
}

TStrongObjectPtr<UTextureRenderTarget2D> MakeTarget(int32 Width, int32 Height)
{
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->ClearColor = FLinearColor::Transparent;
    Target->InitCustomFormat(Width, Height, PF_R8G8B8A8, true);
    Target->UpdateResourceImmediate(true);
    return Target;
}

bool CheckPixels(FAutomationTestBase& Test, UTextureRenderTarget2D* Target, int32 SourceX, int32 SourceY, uint8 Seed, bool bEmpty = false)
{
    FlushRenderingCommands();
    TArray<FColor> Pixels;
    FReadSurfaceDataFlags Flags(RCM_UNorm);
    Flags.SetLinearToGamma(false);
    if (!Test.TestTrue(TEXT("Read rendered pixels"), Target->GameThread_GetRenderTargetResource()->ReadPixels(Pixels, Flags)))
    {
        return false;
    }
    for (int32 Y = 0; Y < Target->SizeY; ++Y)
    {
        for (int32 X = 0; X < Target->SizeX; ++X)
        {
            const FColor Expected = bEmpty ? FColor::Transparent : GetPixel(SourceX + X, SourceY + Y, Seed);
            const FColor Actual = Pixels[Y * Target->SizeX + X];
            if (Actual != Expected)
            {
                Test.AddError(FString::Printf(TEXT("Pixel (%d, %d): expected %s, got %s"), X, Y, *Expected.ToString(), *Actual.ToString()));
                return false;
            }
        }
    }
    return true;
}

bool SaveImage(UOdysseyTiledImage* Image, TArray<uint8>& Bytes, FCustomVersionContainer& Versions)
{
    FMemoryWriter Writer(Bytes, true);
    FObjectAndNameAsStringProxyArchive Archive(Writer, false);
    Image->Serialize(Archive);
    Versions = Archive.GetCustomVersions();
    return !Archive.IsError();
}

bool LoadImage(UOdysseyTiledImage* Image, const TArray<uint8>& Bytes, const FCustomVersionContainer& Versions)
{
    FMemoryReader Reader(Bytes, true);
    FObjectAndNameAsStringProxyArchive Archive(Reader, false);
    Archive.SetCustomVersions(Versions);
    Image->Serialize(Archive);
    return !Archive.IsError();
}
}

using namespace Odyssey::TiledImageTests;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTiledImagePersistenceTest, "Odyssey.Imaging.TiledImage.ReadbackAndPersistence", TestFlags)
bool FOdysseyTiledImagePersistenceTest::RunTest(const FString& Parameters)
{
    const EOdysseyTiledImageFormat Formats[] = { EOdysseyTiledImageFormat::RGBA8, EOdysseyTiledImageFormat::RGBA16F, EOdysseyTiledImageFormat::RGBA32F };
    for (EOdysseyTiledImageFormat Format : Formats)
    {
        // A single 32-pixel RGBA8 tile has a padded D3D12 readback row.
        const int32 Size = Format == EOdysseyTiledImageFormat::RGBA8 ? 32 : 128;
        TStrongObjectPtr<UTexture2D> Source = MakeSource(Size, Size, 23);
        TStrongObjectPtr<UOdysseyTiledImage> Image(NewObject<UOdysseyTiledImage>());
        Image->Initialize(Size, Format);
        Image->CopyFromTexture(Source.Get(), FIntRect(0, 0, Size, Size), FIntPoint(-Size, -Size));
        // Deliberately wait before Tick has had a chance to start the readback.
        FOdysseyTileManager::Get().WaitUntilAllTilesAreCached(false);

        TArray<uint8> Bytes;
        FCustomVersionContainer Versions;
        TestTrue(TEXT("Save image"), SaveImage(Image.Get(), Bytes, Versions));
        TStrongObjectPtr<UOdysseyTiledImage> Loaded(NewObject<UOdysseyTiledImage>());
        TestTrue(TEXT("Load image"), LoadImage(Loaded.Get(), Bytes, Versions));
        TestEqual(TEXT("Pixel format survives serialization"), Loaded->GetFormat(), Format);
        const FUInt32Property* TileSizeProperty = FindFProperty<FUInt32Property>(UOdysseyTiledImage::StaticClass(), TEXT("TileSize"));
        TestNotNull(TEXT("Tile size is serialized by Unreal"), TileSizeProperty);
        if (TileSizeProperty)
        {
            TestEqual(TEXT("Tile size survives serialization"), TileSizeProperty->GetPropertyValue_InContainer(Loaded.Get()), uint32(Size));
        }
        FOdysseyTileManager::Get().WaitUntilAllTilesAreCached();
        FOdysseyTileManager::Get().EvictAllTiles();
        TStrongObjectPtr<UTextureRenderTarget2D> Target = MakeTarget(Size, Size);
        Loaded->Render(Target.Get(), FIntRect(-Size, -Size, 0, 0), FIntPoint::ZeroValue);
        CheckPixels(*this, Target.Get(), 0, 0, 23);
    }

    TStrongObjectPtr<UTexture2D> EmptySource = MakeSource(32, 32, 0, true);
    TStrongObjectPtr<UOdysseyTiledImage> EmptyImage(NewObject<UOdysseyTiledImage>());
    EmptyImage->Initialize(32, EOdysseyTiledImageFormat::RGBA8);
    EmptyImage->CopyFromTexture(EmptySource.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue);
    FOdysseyTileManager::Get().WaitUntilAllTilesAreCached();
    TArray<uint8> Bytes;
    FCustomVersionContainer Versions;
    TestTrue(TEXT("Empty tiles do not cause a save error"), SaveImage(EmptyImage.Get(), Bytes, Versions));
    TStrongObjectPtr<UOdysseyTiledImage> LoadedEmpty(NewObject<UOdysseyTiledImage>());
    TestTrue(TEXT("Load empty image"), LoadImage(LoadedEmpty.Get(), Bytes, Versions));
    TStrongObjectPtr<UTextureRenderTarget2D> Target = MakeTarget(32, 32);
    LoadedEmpty->Render(Target.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue);
    CheckPixels(*this, Target.Get(), 0, 0, 0, true);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTiledImageClippingTest, "Odyssey.Imaging.TiledImage.Clipping", TestFlags)
bool FOdysseyTiledImageClippingTest::RunTest(const FString& Parameters)
{
    TStrongObjectPtr<UTexture2D> Source = MakeSource(96, 80, 41);
    TStrongObjectPtr<UOdysseyTiledImage> Image(NewObject<UOdysseyTiledImage>());
    Image->Initialize(32, EOdysseyTiledImageFormat::RGBA8);
    Image->CopyFromTexture(Source.Get(), FIntRect(0, 0, 96, 80), FIntPoint::ZeroValue);
    FOdysseyTileManager::Get().WaitUntilAllTilesAreCached();
    TStrongObjectPtr<UTextureRenderTarget2D> Target = MakeTarget(48, 40);
    Image->Render(Target.Get(), FIntRect(3, 5, 96, 80), FIntPoint(-16, -11));
    CheckPixels(*this, Target.Get(), 19, 16, 41);
    Image->Render(Target.Get(), FIntRect(0, 0, 96, 80), FIntPoint(100, 100));
    CheckPixels(*this, Target.Get(), 19, 16, 41);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTiledImageValidationTest, "Odyssey.Imaging.TiledImage.Validation", TestFlags)
bool FOdysseyTiledImageValidationTest::RunTest(const FString& Parameters)
{
    TStrongObjectPtr<UOdysseyTiledImage> Image(NewObject<UOdysseyTiledImage>());
    Image->Initialize(32, EOdysseyTiledImageFormat::RGBA16F);
    AddExpectedMessage(TEXT("Invalid tiled image size or format"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 4);
    Image->Initialize(0, EOdysseyTiledImageFormat::RGBA8);
    Image->Initialize(-1, EOdysseyTiledImageFormat::RGBA8);
    Image->Initialize(MAX_int32, EOdysseyTiledImageFormat::RGBA8);
    Image->Initialize(32, static_cast<EOdysseyTiledImageFormat>(255));
    TestEqual(TEXT("Invalid initialization preserves valid format"), Image->GetFormat(), EOdysseyTiledImageFormat::RGBA16F);
    TestTrue(TEXT("Zero tile size is rejected by the tile helper"), Odyssey::TileUtils::GetTilePositionsFromRect(0, FIntRect(0, 0, 64, 64)).IsEmpty());
    TStrongObjectPtr<UTexture2D> Source = MakeSource(32, 32, 73);
    TestTrue(TEXT("Manager rejects invalid tile size"), FOdysseyTileManager::Get().CreateOrUpdateTiles(Source.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue, 0, PF_R8G8B8A8).IsEmpty());
    Image->CopyFromTexture(Source.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue);
    FOdysseyTileManager::Get().WaitUntilAllTilesAreCached();
    TStrongObjectPtr<UTextureRenderTarget2D> Target = MakeTarget(32, 32);
    Image->Render(Target.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue);
    CheckPixels(*this, Target.Get(), 0, 0, 73);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTileStatsTest, "Odyssey.Imaging.TiledImage.LRUStatistics", TestFlags)
bool FOdysseyTileStatsTest::RunTest(const FString& Parameters)
{
    FOdysseyTileManager& Manager = FOdysseyTileManager::Get();
    Manager.WaitUntilAllTilesAreCached();
    Manager.EvictAllTiles();
    const int64 BaselineUncompressed = Manager.GetStats().GetNumTilesUncompressed();
    const int64 BaselineCompressed = Manager.GetStats().GetNumTilesCompressed();
    FSharedBuffer Raw = FUniqueBuffer::Alloc(32 * 32 * 4).MoveToShared();
    FMemory::Memset(const_cast<void*>(Raw.GetData()), 117, Raw.GetSize());
    const FCompressedBuffer Compressed = FCompressedBuffer::Compress(Raw);
    const FOdysseyTileId Tile = Manager.CreateTile(FIoHashBuilder::HashBuffer(Raw.GetView()), Compressed);
    for (uint8 Seed : { uint8(89), uint8(91) })
    {
        FCompressedBuffer Reloaded;
        TestTrue(TEXT("Retrieve compressed-only tile"), Manager.GetTileCompressedBuffer(Tile, Reloaded));
        TStrongObjectPtr<UTexture2D> Source = MakeSource(32, 32, Seed);
        TStrongObjectPtr<UOdysseyTiledImage> Image(NewObject<UOdysseyTiledImage>());
        Image->Initialize(32, EOdysseyTiledImageFormat::RGBA8);
        Image->CopyFromTexture(Source.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue);
        Manager.WaitUntilAllTilesAreCached();
        Manager.EvictAllTiles();
        TestEqual(TEXT("Uncompressed count returns to baseline"), Manager.GetStats().GetNumTilesUncompressed(), BaselineUncompressed);
        TestEqual(TEXT("Compressed count returns to baseline"), Manager.GetStats().GetNumTilesCompressed(), BaselineCompressed);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTileCacheFailureTest, "Odyssey.Imaging.TiledImage.CacheFailures", TestFlags)
bool FOdysseyTileCacheFailureTest::RunTest(const FString& Parameters)
{
    FOdysseyTileManager& Manager = FOdysseyTileManager::Get();
    Manager.WaitUntilAllTilesAreCached();
    TStrongObjectPtr<UTexture2D> Source = MakeSource(32, 32, 151);
    TStrongObjectPtr<UOdysseyTiledImage> Image(NewObject<UOdysseyTiledImage>());
    Image->Initialize(32, EOdysseyTiledImageFormat::RGBA8);
    Image->CopyFromTexture(Source.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue);
    Manager.WaitUntilAllTilesAreCached();
    Manager.EvictAllTiles();

    IFileManager& Files = IFileManager::Get();
    const FString CachePath = Manager.GetCacheOnDiskPath();
    const FString BackupPath = CachePath + TEXT(".test-backup");
    if (!TestTrue(TEXT("Temporarily hide this session's cache"), Files.Move(*BackupPath, *CachePath)))
    {
        return false;
    }
    ON_SCOPE_EXIT
    {
        FPlatformFileManager::Get().GetPlatformFile().SetReadOnly(*CachePath, false);
        if (Files.FileExists(*BackupPath))
        {
            Files.Move(*CachePath, *BackupPath, true, true);
        }
    };
    TArray<uint8> Bytes;
    FCustomVersionContainer Versions;
    AddExpectedError(TEXT("Cannot save tiled image"), EAutomationExpectedErrorFlags::Contains, 1);
    TestFalse(TEXT("A missing cached tile fails the save instead of dropping pixels"), SaveImage(Image.Get(), Bytes, Versions));
    TestTrue(TEXT("Restore cache"), Files.Move(*CachePath, *BackupPath));
    Bytes.Reset();
    TestTrue(TEXT("Save recovers after restoring the cache"), SaveImage(Image.Get(), Bytes, Versions));

    if (!TestTrue(TEXT("Make cache read-only"), FPlatformFileManager::Get().GetPlatformFile().SetReadOnly(*CachePath, true)))
    {
        return false;
    }
    AddExpectedMessage(TEXT("Failed to cache tile"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 1);
    FSharedBuffer Raw = FUniqueBuffer::Alloc(32 * 32 * 4).MoveToShared();
    FMemory::Memset(const_cast<void*>(Raw.GetData()), 173, Raw.GetSize());
    const FOdysseyTileId Tile = Manager.CreateTile(FIoHashBuilder::HashBuffer(Raw.GetView()), FCompressedBuffer::Compress(Raw));
    Manager.WaitUntilAllTilesAreCached();
    TestTrue(TEXT("Restore cache write access"), FPlatformFileManager::Get().GetPlatformFile().SetReadOnly(*CachePath, false));
    Manager.EvictAllTiles();
    FCompressedBuffer Retained;
    TestTrue(TEXT("A failed disk write keeps the tile resident"), Manager.GetTileCompressedBuffer(Tile, Retained));
    TestTrue(TEXT("Retained tile preserves its bytes"), Retained.Decompress().GetView().EqualBytes(Raw.GetView()));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTileCacheSessionTest, "Odyssey.Imaging.TiledImage.SessionIsolation", TestFlags)
bool FOdysseyTileCacheSessionTest::RunTest(const FString& Parameters)
{
    const FString CachePath = FOdysseyTileManager::GetCacheOnDiskPath();
    const FString SessionDirectory = FPaths::GetPath(CachePath);
    const FString SharedDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Temp"), TEXT("OdysseyTileManager"));
    TestTrue(TEXT("Cache is in a session subdirectory"), FPaths::IsSamePath(FPaths::GetPath(SessionDirectory), SharedDirectory));
    TestTrue(TEXT("Session name includes process id"), FPaths::GetCleanFilename(SessionDirectory).Contains(FString::Printf(TEXT("-%u-"), FPlatformProcess::GetCurrentProcessId())));
    TestEqual(TEXT("Cache path remains stable"), FOdysseyTileManager::GetCacheOnDiskPath(), CachePath);
    FString SentinelPath;
    if (FParse::Value(FCommandLine::Get(), TEXT("OdysseyOtherSessionSentinel="), SentinelPath))
    {
        TestTrue(TEXT("Other session survived initialization"), IFileManager::Get().FileExists(*SentinelPath));
    }
    AddInfo(FString::Printf(TEXT("Session cache: %s"), *CachePath));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTiledImagePendingTest, "Odyssey.Imaging.TiledImage.PendingReadbackDoesNotBlock", TestFlags)
bool FOdysseyTiledImagePendingTest::RunTest(const FString& Parameters)
{
    TStrongObjectPtr<UTexture2D> Source = MakeSource(32, 32, 211);
    FOdysseyTileManager& Manager = FOdysseyTileManager::Get();
    FEvent* Gate = FPlatformProcess::GetSynchEventFromPool(true);
    ON_SCOPE_EXIT
    {
        Gate->Trigger();
        FlushRenderingCommands();
        Manager.WaitUntilAllTilesAreCached();
        FPlatformProcess::ReturnSynchEventToPool(Gate);
    };
    ENQUEUE_RENDER_COMMAND(OdysseyTileTest_HoldProducer)([Gate](FRHICommandListImmediate&)
    {
        Gate->Wait(2000); // Bound a regression's stall rather than hanging the automation process.
    });

    const TArray<FOdysseyTileManager::FCreatedTile> Tiles = Manager.CreateOrUpdateTiles(
        Source.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue, 32, PF_R8G8B8A8);
    if (!TestEqual(TEXT("One tile created"), Tiles.Num(), 1))
    {
        return false;
    }
    const double Start = FPlatformTime::Seconds();
    TArray<TSharedFuture<TSharedPtr<FOdysseyTileManager::FTileTextureHandle>>> First = Manager.GetTileTextures({Tiles[0].Id}, 32, PF_R8G8B8A8);
    TArray<TSharedFuture<TSharedPtr<FOdysseyTileManager::FTileTextureHandle>>> Second = Manager.GetTileTextures({Tiles[0].Id}, 32, PF_R8G8B8A8);
    FOdysseyTileManagerTestAccess::Poll(Manager, Tiles[0].Id);
    TestTrue(TEXT("Texture requests and readback polling return before the producer"), FPlatformTime::Seconds() - Start < 1.0);
    TestFalse(TEXT("First request is pending"), First[0].IsReady());
    TestFalse(TEXT("Second request is independently pending"), Second[0].IsReady());
    Gate->Trigger();
    FlushRenderingCommands();
    TestTrue(TEXT("First texture becomes available"), First[0].IsReady() && First[0].Get().IsValid());
    TestTrue(TEXT("Second texture becomes available"), Second[0].IsReady() && Second[0].Get().IsValid());
    TStrongObjectPtr<UOdysseyTiledImage> Image(NewObject<UOdysseyTiledImage>());
    Image->Initialize(32, EOdysseyTiledImageFormat::RGBA8);
    Image->CopyFromTexture(Source.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue);
    Image->CopyFromTexture(Source.Get(), FIntRect(0, 0, 16, 16), FIntPoint::ZeroValue);
    TStrongObjectPtr<UTextureRenderTarget2D> Target = MakeTarget(32, 32);
    Image->Render(Target.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue);
    CheckPixels(*this, Target.Get(), 0, 0, 211);
    return true;
}

namespace Odyssey::TiledImageTests
{
using FGPUHandle = TSharedPtr<FOdysseyTileManager::FTileTextureHandle>;

struct FGPUCacheTestScope
{
    uint64 PreviousBudget = GetMutableDefault<UOdysseyTileManagerSettings>()->MaxGPUSize;
    FGPUCacheTestScope()
    {
        FlushRenderingCommands();
        GetMutableDefault<UOdysseyTileManagerSettings>()->MaxGPUSize = 4;
        FOdysseyTileManager::Get().EvictAllTiles();
        FOdysseyTileManager::Get().EvictTiles();
    }
    ~FGPUCacheTestScope()
    {
        FOdysseyTileManager::Get().WaitUntilAllTilesAreCached(false);
        FlushRenderingCommands();
        GetMutableDefault<UOdysseyTileManagerSettings>()->MaxGPUSize = PreviousBudget;
        FOdysseyTileManager::Get().EvictAllTiles();
        FOdysseyTileManager::Get().EvictTiles();
    }
};

FGPUHandle GetHandle(FOdysseyTileId Id, uint32 Size = 32, EPixelFormat Format = PF_R8G8B8A8)
{
    TArray<TSharedFuture<FGPUHandle>> Handles = FOdysseyTileManager::Get().GetTileTextures({Id}, Size, Format);
    FlushRenderingCommands();
    return Handles[0].Get();
}

void DrawHandle(UTextureRenderTarget2D* Target, FGPUHandle Handle)
{
    FTextureRenderTargetResource* Resource = Target->GameThread_GetRenderTargetResource();
    ENQUEUE_RENDER_COMMAND(OdysseyGPUCacheTest_Draw)([Resource, Handle](FRHICommandListImmediate& RHICmdList)
    {
        FRDGBuilder GraphBuilder(RHICmdList);
        FRDGTextureRef Source = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(Handle->GetTextureRHI(), TEXT("OdysseyGPUCacheTestSource")));
        FRDGTextureRef Destination = Resource->GetRenderTargetTexture(GraphBuilder);
        const FIntRect Rect = Handle->GetRectInTexture();
        AddDrawTexturePass(GraphBuilder, FScreenPassViewInfo(), Source, Destination,
            Rect.Min, Rect.Size(), FIntPoint::ZeroValue, Rect.Size());
        GraphBuilder.Execute();
    });
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyGPUCacheReuseTest, "Odyssey.Imaging.GPUCache.ReuseAfterReadback", TestFlags)
bool FOdysseyGPUCacheReuseTest::RunTest(const FString& Parameters)
{
    FGPUCacheTestScope Scope;
    FOdysseyTileManager& Manager = FOdysseyTileManager::Get();
    TStrongObjectPtr<UTexture2D> Source = MakeSource(96, 64, 47);
    TStrongObjectPtr<UOdysseyTiledImage> Image(NewObject<UOdysseyTiledImage>());
    Image->Initialize(32, EOdysseyTiledImageFormat::RGBA8);
    Image->CopyFromTexture(Source.Get(), FIntRect(0, 0, 96, 64), FIntPoint::ZeroValue);
    Manager.WaitUntilAllTilesAreCached(false);
    FlushRenderingCommands();
    TestEqual(TEXT("Six tiles remain cached after readback"), Manager.GetStats().GetNumTilesOnGPU(), int64(6));
    TestEqual(TEXT("The six tiles share one cached atlas"), FOdysseyTileManagerTestAccess::AtlasCount(), 1);
    TestTrue(TEXT("GPU accounting includes at least all atlas texels"), Manager.GetStats().GetSizeOnGPU() >= 96 * 64 * 4);
    FOdysseyTileManagerTestAccess::EvictCPU();
    const uint64 Before = FOdysseyTileManagerTestAccess::UploadCount();
    TStrongObjectPtr<UTextureRenderTarget2D> Target = MakeTarget(96, 64);
    for (int32 RenderIndex = 0; RenderIndex < 3; ++RenderIndex)
    {
        Image->Render(Target.Get(), FIntRect(0, 0, 96, 64), FIntPoint::ZeroValue);
        CheckPixels(*this, Target.Get(), 0, 0, 47);
    }
    TestEqual(TEXT("Repeated renders do not upload CPU pixels"), FOdysseyTileManagerTestAccess::UploadCount(), Before);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyGPUCachePendingTest, "Odyssey.Imaging.GPUCache.SharedUploadsAndFormats", TestFlags)
bool FOdysseyGPUCachePendingTest::RunTest(const FString& Parameters)
{
    FGPUCacheTestScope Scope;
    FOdysseyTileManager& Manager = FOdysseyTileManager::Get();
    FSharedBuffer Raw = FUniqueBuffer::Alloc(32 * 32 * 4).MoveToShared();
    FMemory::Memset(const_cast<void*>(Raw.GetData()), 63, Raw.GetSize());
    FOdysseyTileId Id = Manager.CreateTile(FIoHashBuilder::HashBuffer(Raw.GetView()), FCompressedBuffer::Compress(Raw));
    FOdysseyTileId OtherId = Manager.CreateTile(FIoHashBuilder::HashBuffer(Raw.GetView()), FCompressedBuffer::Compress(Raw));
    TestTrue(TEXT("Identical content still has distinct tile IDs"), Id != OtherId);
    Manager.WaitUntilAllTilesAreCached(false);
    FEvent* Gate = FPlatformProcess::GetSynchEventFromPool(true);
    ON_SCOPE_EXIT
    {
        Gate->Trigger();
        FlushRenderingCommands();
        FPlatformProcess::ReturnSynchEventToPool(Gate);
    };
    ENQUEUE_RENDER_COMMAND(OdysseyGPUCacheTest_HoldUpload)([Gate](FRHICommandListImmediate&) { Gate->Wait(2000); });
    const uint64 Before = FOdysseyTileManagerTestAccess::UploadCount();
    TArray<TSharedFuture<FGPUHandle>> First = Manager.GetTileTextures({Id, OtherId}, 32, PF_R8G8B8A8);
    TArray<TSharedFuture<FGPUHandle>> Second = Manager.GetTileTextures({OtherId}, 32, PF_R8G8B8A8);
    TestFalse(TEXT("Cold upload stays asynchronous"), First[0].IsReady());
    Gate->Trigger();
    FlushRenderingCommands();
    TestEqual(TEXT("Concurrent requests upload the tile once"), FOdysseyTileManagerTestAccess::UploadCount(), Before + 1);
    TestTrue(TEXT("Different IDs with identical content share the uploaded handle"), First[0].Get() == First[1].Get() && First[0].Get() == Second[0].Get());
    const int64 FirstAtlasBytes = Manager.GetStats().GetSizeOnGPU();
    TestEqual(TEXT("A repeated tile is accounted once"), Manager.GetStats().GetNumTilesOnGPU(), int64(1));
    FGPUHandle OtherFormat = GetHandle(Id, 32, PF_B8G8R8A8);
    FGPUHandle OtherSize = GetHandle(Id, 16, PF_A32B32G32R32F);
    TestTrue(TEXT("Different formats do not alias"), OtherFormat != First[0].Get());
    TestEqual(TEXT("Requested pixel format is preserved"), OtherFormat->GetTextureRHI()->GetFormat(), PF_B8G8R8A8);
    TestEqual(TEXT("Requested tile size is preserved"), OtherSize->GetRectInTexture().Size(), FIntPoint(16, 16));
    TestTrue(TEXT("Each atlas allocation is charged"), Manager.GetStats().GetSizeOnGPU() >= FirstAtlasBytes * 2);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyGPUCacheIdenticalCopiesTest, "Odyssey.Imaging.GPUCache.IdenticalCopies", TestFlags)
bool FOdysseyGPUCacheIdenticalCopiesTest::RunTest(const FString& Parameters)
{
    FGPUCacheTestScope Scope;
    FOdysseyTileManager& Manager = FOdysseyTileManager::Get();
    TStrongObjectPtr<UTexture2D> Source = MakeSource(96, 64, 61);
    TStrongObjectPtr<UOdysseyTiledImage> Image(NewObject<UOdysseyTiledImage>());
    Image->Initialize(32, EOdysseyTiledImageFormat::RGBA8);
    const FIntRect Rect(0, 0, 96, 64);
    Image->CopyFromTexture(Source.Get(), Rect, FIntPoint::ZeroValue);
    Manager.WaitUntilAllTilesAreCached(false);
    FlushRenderingCommands();
    const int64 InitialBytes = Manager.GetStats().GetSizeOnGPU();
    const uint64 InitialUploads = FOdysseyTileManagerTestAccess::UploadCount();
    TStrongObjectPtr<UTextureRenderTarget2D> Target = MakeTarget(96, 64);
    for (int32 CopyIndex = 0; CopyIndex < 4; ++CopyIndex)
    {
        Image->CopyFromTexture(Source.Get(), Rect, FIntPoint::ZeroValue);
        Manager.WaitUntilAllTilesAreCached(false);
        FlushRenderingCommands();
        TestEqual(TEXT("Identical copies retain six unique GPU tiles"), Manager.GetStats().GetNumTilesOnGPU(), int64(6));
        TestEqual(TEXT("Identical copies do not grow GPU memory"), Manager.GetStats().GetSizeOnGPU(), InitialBytes);
        TestEqual(TEXT("Duplicate atlases are released"), FOdysseyTileManagerTestAccess::AtlasCount(), 1);
        Image->Render(Target.Get(), Rect, FIntPoint::ZeroValue);
        CheckPixels(*this, Target.Get(), 0, 0, 61);
    }

    TStrongObjectPtr<UOdysseyTiledImage> OtherImage(NewObject<UOdysseyTiledImage>());
    OtherImage->Initialize(32, EOdysseyTiledImageFormat::RGBA8);
    OtherImage->CopyFromTexture(Source.Get(), Rect, FIntPoint::ZeroValue);
    Manager.WaitUntilAllTilesAreCached(false);
    FlushRenderingCommands();
    TestEqual(TEXT("Identical images share GPU memory"), Manager.GetStats().GetSizeOnGPU(), InitialBytes);

    TStrongObjectPtr<UTexture2D> Changed = MakeSource(32, 32, 63);
    Image->CopyFromTexture(Changed.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue);
    Manager.WaitUntilAllTilesAreCached(false);
    FlushRenderingCommands();
    TestEqual(TEXT("Changed content keeps its own GPU entry"), Manager.GetStats().GetNumTilesOnGPU(), int64(7));
    OtherImage->Render(Target.Get(), Rect, FIntPoint::ZeroValue);
    CheckPixels(*this, Target.Get(), 0, 0, 61);
    TStrongObjectPtr<UTextureRenderTarget2D> ChangedTarget = MakeTarget(32, 32);
    Image->Render(ChangedTarget.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue);
    CheckPixels(*this, ChangedTarget.Get(), 0, 0, 63);
    TestEqual(TEXT("Shared contents do not require CPU uploads"), FOdysseyTileManagerTestAccess::UploadCount(), InitialUploads);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyGPUCachePendingCopiesTest, "Odyssey.Imaging.GPUCache.PendingIdenticalCopies", TestFlags)
bool FOdysseyGPUCachePendingCopiesTest::RunTest(const FString& Parameters)
{
    FGPUCacheTestScope Scope;
    FOdysseyTileManager& Manager = FOdysseyTileManager::Get();
    TStrongObjectPtr<UTexture2D> Source = MakeSource(64, 32, 67);
    TArray<FOdysseyTileId> FirstTileIds;
    TArray<TSharedFuture<FGPUHandle>> Pinned;
    for (int32 CopyIndex = 0; CopyIndex < 8; ++CopyIndex)
    {
        const TArray<FOdysseyTileManager::FCreatedTile> Created = Manager.CreateOrUpdateTiles(
            Source.Get(), FIntRect(0, 0, 64, 32), FIntPoint::ZeroValue, 32, PF_R8G8B8A8);
        FirstTileIds.Add(Created[0].Id);
        if (CopyIndex == 1)
        {
            Pinned = Manager.GetTileTextures({Created[0].Id}, 32, PF_R8G8B8A8);
        }
    }
    Manager.WaitUntilAllTilesAreCached(false);
    FlushRenderingCommands();
    TestEqual(TEXT("Pending copies resolve to two unique tiles"), Manager.GetStats().GetNumTilesOnGPU(), int64(2));
    TestEqual(TEXT("Only one atlas remains cached after all readbacks"), FOdysseyTileManagerTestAccess::AtlasCount(), 1);
    const FGPUHandle SharedHandle = GetHandle(FirstTileIds[0]);
    for (FOdysseyTileId Id : FirstTileIds)
    {
        TestTrue(TEXT("All versions reuse the same content handle"), GetHandle(Id) == SharedHandle);
    }
    const FGPUHandle OldHandle = Pinned[0].Get();
    TestTrue(TEXT("A duplicate atlas can remain pinned outside the cache"), OldHandle->GetTextureRHI() != SharedHandle->GetTextureRHI());
    TStrongObjectPtr<UTextureRenderTarget2D> Target = MakeTarget(32, 32);
    DrawHandle(Target.Get(), OldHandle);
    CheckPixels(*this, Target.Get(), 0, 0, 67);

    // The first tile is already cached; the second has different pixels in the same new atlas.
    TStrongObjectPtr<UTexture2D> OtherSource = MakeSource(32, 32, 69);
    TStrongObjectPtr<UTextureRenderTarget2D> MixedSource = MakeTarget(64, 32);
    ENQUEUE_RENDER_COMMAND(OdysseyGPUCacheTest_MixedSource)(
        [MixedResource = MixedSource->GameThread_GetRenderTargetResource(), SourceTexture = Source->GetResource()->TextureRHI,
            OtherTexture = OtherSource->GetResource()->TextureRHI](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder GraphBuilder(RHICmdList);
            FRDGTextureRef Destination = MixedResource->GetRenderTargetTexture(GraphBuilder);
            FRDGTextureRef First = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(SourceTexture, TEXT("OdysseyGPUCacheMixedFirst")));
            FRDGTextureRef Second = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(OtherTexture, TEXT("OdysseyGPUCacheMixedSecond")));
            AddDrawTexturePass(GraphBuilder, FScreenPassViewInfo(), First, Destination,
                FIntPoint::ZeroValue, FIntPoint(32, 32), FIntPoint::ZeroValue, FIntPoint(32, 32));
            AddDrawTexturePass(GraphBuilder, FScreenPassViewInfo(), Second, Destination,
                FIntPoint::ZeroValue, FIntPoint(32, 32), FIntPoint(32, 0), FIntPoint(32, 32));
            GraphBuilder.Execute();
        });
    const TArray<FOdysseyTileManager::FCreatedTile> Mixed = Manager.CreateOrUpdateTiles(
        MixedSource.Get(), FIntRect(0, 0, 64, 32), FIntPoint::ZeroValue, 32, PF_R8G8B8A8);
    Manager.WaitUntilAllTilesAreCached(false);
    FlushRenderingCommands();
    TestEqual(TEXT("New content is compacted into the shared atlas"), FOdysseyTileManagerTestAccess::AtlasCount(), 1);
    TestEqual(TEXT("Mixed atlas deduplicates only identical pixels"), Manager.GetStats().GetNumTilesOnGPU(), int64(3));
    TestTrue(TEXT("Different IDs still share their relocated content"), GetHandle(Mixed[0].Id) == GetHandle(FirstTileIds[0]));
    DrawHandle(Target.Get(), SharedHandle);
    CheckPixels(*this, Target.Get(), 0, 0, 67);
    DrawHandle(Target.Get(), GetHandle(Mixed[1].Id));
    CheckPixels(*this, Target.Get(), 0, 0, 69);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyGPUCacheSmallUpdatesTest, "Odyssey.Imaging.GPUCache.SmallUpdatesMemoryAccounting", TestFlags)
bool FOdysseyGPUCacheSmallUpdatesTest::RunTest(const FString& Parameters)
{
    FGPUCacheTestScope Scope;
    FOdysseyTileManager& Manager = FOdysseyTileManager::Get();
    constexpr int64 TileBytes = 64 * 64 * 4;
    TStrongObjectPtr<UOdysseyTiledImage> Image(NewObject<UOdysseyTiledImage>());
    Image->Initialize(64, EOdysseyTiledImageFormat::RGBA8);
    TStrongObjectPtr<UTextureRenderTarget2D> FloatSource(NewObject<UTextureRenderTarget2D>());
    FloatSource->InitCustomFormat(256, 64, PF_FloatRGBA, true);
    FloatSource->UpdateResourceImmediate(true);
    TStrongObjectPtr<UTextureRenderTarget2D> Target = MakeTarget(64, 64);

    auto UpdateFloatSource = [&FloatSource](UTexture2D* Source)
    {
        ENQUEUE_RENDER_COMMAND(OdysseyGPUCacheTest_FloatSource)(
            [SourceTexture = Source->GetResource()->TextureRHI,
                DestinationResource = FloatSource->GameThread_GetRenderTargetResource()](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder GraphBuilder(RHICmdList);
                FRDGTextureRef SourceTextureRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(SourceTexture, TEXT("OdysseyGPUCacheFloatSource")));
                FRDGTextureRef Destination = DestinationResource->GetRenderTargetTexture(GraphBuilder);
                AddDrawTexturePass(GraphBuilder, FScreenPassViewInfo(), SourceTextureRDG, Destination,
                    FIntPoint::ZeroValue, SourceTextureRDG->Desc.Extent, FIntPoint::ZeroValue, SourceTextureRDG->Desc.Extent);
                GraphBuilder.Execute();
            });
    };

    int64 SeparateAtlasBytes = 0;
    for (uint8 UpdateIndex = 0; UpdateIndex < 4; ++UpdateIndex)
    {
        const uint8 Seed = 197 + UpdateIndex;
        TStrongObjectPtr<UTexture2D> Source = MakeSource(64, 64, Seed);
        UpdateFloatSource(Source.Get());
        const int64 BeforeCPU = Manager.GetStats().GetSizeUncompressed();
        const int64 BeforeGPU = Manager.GetStats().GetSizeOnGPU();
        Image->CopyFromTexture(FloatSource.Get(), FIntRect(0, 0, 64, 64), FIntPoint::ZeroValue);
        Manager.WaitUntilAllTilesAreCached(false);
        TestTrue(TEXT("Explicit synchronization completes GPU cache compaction"), FOdysseyTileManagerTestAccess::IsCompacted());
        const int64 AddedCPU = Manager.GetStats().GetSizeUncompressed() - BeforeCPU;
        const int64 AddedGPU = Manager.GetStats().GetSizeOnGPU() - BeforeGPU;
        SeparateAtlasBytes += AddedGPU;
        TestEqual(TEXT("A new RGBA8 tile stores four bytes per pixel after conversion"), AddedCPU, TileBytes);
        TestTrue(TEXT("GPU accounting includes all retained pixels"), Manager.GetStats().GetSizeOnGPU() >= TileBytes * (UpdateIndex + 1));
        Image->Render(Target.Get(), FIntRect(0, 0, 64, 64), FIntPoint::ZeroValue);
        CheckPixels(*this, Target.Get(), 0, 0, Seed);
        AddInfo(FString::Printf(TEXT("64x64 RGBA16F -> RGBA8 update: CPU +%lld bytes; GPU cache +%lld bytes"), AddedCPU, AddedGPU));
    }
    TestEqual(TEXT("Each modified version keeps one distinct GPU tile"), Manager.GetStats().GetNumTilesOnGPU(), int64(4));

    TStrongObjectPtr<UTexture2D> GroupedSource = MakeSource(256, 64, 207);
    UpdateFloatSource(GroupedSource.Get());
    const int64 BeforeGrouped = Manager.GetStats().GetSizeOnGPU();
    Image->CopyFromTexture(FloatSource.Get(), FIntRect(0, 0, 256, 64), FIntPoint::ZeroValue);
    Manager.WaitUntilAllTilesAreCached(false);
    FlushRenderingCommands();
    const int64 GroupedAtlasBytes = Manager.GetStats().GetSizeOnGPU() - BeforeGrouped;
    TestEqual(TEXT("Four newly modified tiles retain four distinct entries"), Manager.GetStats().GetNumTilesOnGPU(), int64(8));
    TestTrue(TEXT("All versions fit in one shared atlas"), FOdysseyTileManagerTestAccess::AtlasCount() == 1);
    TestTrue(TEXT("Packed allocation contains all eight tiles"), Manager.GetStats().GetSizeOnGPU() >= TileBytes * 8);
    TStrongObjectPtr<UTextureRenderTarget2D> GroupedTarget = MakeTarget(256, 64);
    Image->Render(GroupedTarget.Get(), FIntRect(0, 0, 256, 64), FIntPoint::ZeroValue);
    CheckPixels(*this, GroupedTarget.Get(), 0, 0, 207);
    AddInfo(FString::Printf(TEXT("Four 64x64 RGBA8 tiles: separate atlas allocations = %lld bytes; grouped atlas = %lld bytes"), SeparateAtlasBytes, GroupedAtlasBytes));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyGPUCacheCompactionTest, "Odyssey.Imaging.GPUCache.PackingAndSparseAtlasReclamation", TestFlags)
bool FOdysseyGPUCacheCompactionTest::RunTest(const FString& Parameters)
{
    FGPUCacheTestScope Scope;
    FOdysseyTileManager& Manager = FOdysseyTileManager::Get();
    GetMutableDefault<UOdysseyTileManagerSettings>()->MaxGPUSize = 16;
    Manager.EvictTiles();
    FlushRenderingCommands();
    constexpr int32 NumTiles = 717;
    constexpr int32 TileSize = 64;
    constexpr int64 TileBytes = TileSize * TileSize * 4;
    TArray<FOdysseyTileId> Ids;
    for (int32 Start = 0; Start < NumTiles; Start += 17)
    {
        TArray<FOdysseyTileId> Batch;
        for (int32 Index = Start; Index < FMath::Min(Start + 17, NumTiles); ++Index)
        {
            FSharedBuffer Raw = FUniqueBuffer::Alloc(TileBytes).MoveToShared();
            uint8* Data = static_cast<uint8*>(const_cast<void*>(Raw.GetData()));
            for (int32 Pixel = 0; Pixel < TileSize * TileSize; ++Pixel)
            {
                Data[Pixel * 4] = uint8(Index & 255);
                Data[Pixel * 4 + 1] = uint8(Index >> 8);
                Data[Pixel * 4 + 2] = 173;
                Data[Pixel * 4 + 3] = 255;
            }
            const FOdysseyTileId Id = Manager.CreateTile(FIoHashBuilder::HashBuffer(Raw.GetView()), FCompressedBuffer::Compress(Raw));
            Ids.Add(Id);
            Batch.Add(Id);
        }
        // Distinct calls model successive small updates; no image-wide upload is needed for sharing.
        TArray<TSharedFuture<FGPUHandle>> Uploaded = Manager.GetTileTextures(Batch, TileSize, PF_R8G8B8A8);
        FlushRenderingCommands();
        for (const auto& Future : Uploaded)
        {
            TestTrue(TEXT("Packed upload resolves a valid handle"), Future.Get().IsValid());
        }
    }
    const int64 PackedBytes = Manager.GetStats().GetSizeOnGPU();
    TestEqual(TEXT("All 717 unique tiles remain cached"), Manager.GetStats().GetNumTilesOnGPU(), int64(NumTiles));
    TestEqual(TEXT("717 updates occupy three compact atlases"), FOdysseyTileManagerTestAccess::AtlasCount(), 3);
    TestTrue(TEXT("Packing overhead stays below ten percent of useful pixel bytes"), PackedBytes <= NumTiles * TileBytes * 11 / 10);
    AddInfo(FString::Printf(TEXT("717 RGBA8 64x64 tiles: useful pixels = %lld bytes; packed GPU cache = %lld bytes"), NumTiles * TileBytes, PackedBytes));

    // Keep a hot tile from each page. Shrinking must move them, rather than pin all three pages.
    const FGPUHandle OldA = GetHandle(Ids[0], TileSize);
    const FGPUHandle OldB = GetHandle(Ids[256], TileSize);
    const FGPUHandle OldC = GetHandle(Ids[512], TileSize);
    FOdysseyTileManagerTestAccess::Budget(32 * TileBytes);
    TestEqual(TEXT("Eviction is per tile across all source pages"), Manager.GetStats().GetNumTilesOnGPU(), int64(32));
    TestEqual(TEXT("Sparse pages are replaced by one smaller atlas"), FOdysseyTileManagerTestAccess::AtlasCount(), 1);
    TestTrue(TEXT("Actual retained allocation respects the reduced budget"), Manager.GetStats().GetSizeOnGPU() <= 32 * TileBytes);
    TestTrue(TEXT("Hot tile from the first page survives"), FOdysseyTileManagerTestAccess::IsCached(Ids[0], TileSize));
    TestTrue(TEXT("Hot tile from the second page survives"), FOdysseyTileManagerTestAccess::IsCached(Ids[256], TileSize));
    TestTrue(TEXT("Hot tile from the third page survives"), FOdysseyTileManagerTestAccess::IsCached(Ids[512], TileSize));
    TestFalse(TEXT("Cold contents are evicted individually"), FOdysseyTileManagerTestAccess::IsCached(Ids[100], TileSize));
    const uint64 Before = FOdysseyTileManagerTestAccess::UploadCount();
    TStrongObjectPtr<UTextureRenderTarget2D> Target = MakeTarget(TileSize, TileSize);
    auto CheckSolid = [this, &Target](const FGPUHandle& Handle, int32 Index)
    {
        DrawHandle(Target.Get(), Handle);
        FlushRenderingCommands();
        TArray<FColor> Pixels;
        FReadSurfaceDataFlags Flags(RCM_UNorm);
        Flags.SetLinearToGamma(false);
        if (TestTrue(TEXT("Read relocated tile pixels"), Target->GameThread_GetRenderTargetResource()->ReadPixels(Pixels, Flags)))
        {
            const FColor Expected(uint8(Index & 255), uint8(Index >> 8), 173, 255);
            for (const FColor& Pixel : Pixels)
            {
                if (Pixel != Expected)
                {
                    AddError(TEXT("Relocation changed tile pixels"));
                    break;
                }
            }
        }
    };
    CheckSolid(OldA, 0);
    CheckSolid(OldB, 256);
    CheckSolid(OldC, 512);
    CheckSolid(GetHandle(Ids[0], TileSize), 0);
    CheckSolid(GetHandle(Ids[256], TileSize), 256);
    CheckSolid(GetHandle(Ids[512], TileSize), 512);
    TestEqual(TEXT("Compaction never reuploads pixels from the CPU"), FOdysseyTileManagerTestAccess::UploadCount(), Before);
    FOdysseyTileManagerTestAccess::Budget(TileBytes);
    TestEqual(TEXT("One hot tile does not retain a large atlas"), Manager.GetStats().GetNumTilesOnGPU(), int64(1));
    TestTrue(TEXT("The final atlas is sized for one tile"), Manager.GetStats().GetSizeOnGPU() <= TileBytes);
    CheckSolid(GetHandle(Ids[512], TileSize), 512);
    CheckSolid(OldA, 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyGPUCacheBudgetTest, "Odyssey.Imaging.GPUCache.BudgetLRUAndPinnedHandles", TestFlags)
bool FOdysseyGPUCacheBudgetTest::RunTest(const FString& Parameters)
{
    FGPUCacheTestScope Scope;
    FOdysseyTileManager& Manager = FOdysseyTileManager::Get();
    TStrongObjectPtr<UTexture2D> A = MakeSource(32, 32, 51);
    TStrongObjectPtr<UTexture2D> B = MakeSource(32, 32, 53);
    TStrongObjectPtr<UTexture2D> C = MakeSource(32, 32, 55);
    const FOdysseyTileId IdA = Manager.CreateOrUpdateTiles(A.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue, 32, PF_R8G8B8A8)[0].Id;
    Manager.WaitUntilAllTilesAreCached(false);
    const FGPUHandle HandleA = GetHandle(IdA);
    const uint64 AtlasBytes = Manager.GetStats().GetSizeOnGPU();
    FOdysseyTileManagerTestAccess::Budget(AtlasBytes * 2);
    const FOdysseyTileId IdB = Manager.CreateOrUpdateTiles(B.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue, 32, PF_R8G8B8A8)[0].Id;
    Manager.WaitUntilAllTilesAreCached(false);
    const FGPUHandle HandleB = GetHandle(IdB);
    GetHandle(IdA); // A is now more recent than B.
    const FOdysseyTileId IdC = Manager.CreateOrUpdateTiles(C.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue, 32, PF_R8G8B8A8)[0].Id;
    Manager.WaitUntilAllTilesAreCached(false);
    FlushRenderingCommands();
    TestTrue(TEXT("Recently used atlas survives"), FOdysseyTileManagerTestAccess::IsCached(IdA));
    TestFalse(TEXT("Least recently used atlas is evicted"), FOdysseyTileManagerTestAccess::IsCached(IdB));
    TestTrue(TEXT("New atlas is cached"), FOdysseyTileManagerTestAccess::IsCached(IdC));
    TestTrue(TEXT("Cache respects its atlas budget"), uint64(Manager.GetStats().GetSizeOnGPU()) <= AtlasBytes * 2);
    TStrongObjectPtr<UTextureRenderTarget2D> Target = MakeTarget(32, 32);
    DrawHandle(Target.Get(), HandleB);
    CheckPixels(*this, Target.Get(), 0, 0, 53);
    const uint64 Before = FOdysseyTileManagerTestAccess::UploadCount();
    const FGPUHandle ReloadedB = GetHandle(IdB);
    TestEqual(TEXT("An evicted tile is uploaded again"), FOdysseyTileManagerTestAccess::UploadCount(), Before + 1);
    TestTrue(TEXT("Reload uses a new atlas while the old handle is pinned"), ReloadedB->GetTextureRHI() != HandleB->GetTextureRHI());
    DrawHandle(Target.Get(), ReloadedB);
    CheckPixels(*this, Target.Get(), 0, 0, 53);

    UOdysseyTileManagerSettings* Settings = GetMutableDefault<UOdysseyTileManagerSettings>();
    Settings->MaxGPUSize = 0;
    FPropertyChangedEvent Change(FindFProperty<FProperty>(Settings->GetClass(), TEXT("MaxGPUSize")));
    Settings->PostEditChangeProperty(Change);
    TestEqual(TEXT("Disabling the cache immediately releases its entries"), Manager.GetStats().GetNumTilesOnGPU(), int64(0));
    TestEqual(TEXT("Disabling the cache clears the byte budget usage"), Manager.GetStats().GetSizeOnGPU(), int64(0));
    const uint64 DisabledBefore = FOdysseyTileManagerTestAccess::UploadCount();
    GetHandle(IdC);
    GetHandle(IdC);
    TestEqual(TEXT("Disabled cache does not retain uploads"), FOdysseyTileManagerTestAccess::UploadCount(), DisabledBefore + 2);
    DrawHandle(Target.Get(), HandleA);
    CheckPixels(*this, Target.Get(), 0, 0, 51);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyGPUCacheEmptyTest, "Odyssey.Imaging.GPUCache.EmptyTilesAndGenerations", TestFlags)
bool FOdysseyGPUCacheEmptyTest::RunTest(const FString& Parameters)
{
    FGPUCacheTestScope Scope;
    FOdysseyTileManager& Manager = FOdysseyTileManager::Get();
    TStrongObjectPtr<UTexture2D> Empty = MakeSource(32, 32, 0, true);
    const FOdysseyTileId OldId = Manager.CreateOrUpdateTiles(Empty.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue, 32, PF_R8G8B8A8)[0].Id;
    Manager.WaitUntilAllTilesAreCached(false);
    FlushRenderingCommands();
    TestEqual(TEXT("Empty tile does not retain a GPU atlas"), Manager.GetStats().GetSizeOnGPU(), int64(0));
    TStrongObjectPtr<UTexture2D> Source = MakeSource(32, 32, 57);
    const FOdysseyTileId NewId = Manager.CreateOrUpdateTiles(Source.Get(), FIntRect(0, 0, 32, 32), FIntPoint::ZeroValue, 32, PF_R8G8B8A8)[0].Id;
    Manager.WaitUntilAllTilesAreCached(false);
    TestFalse(TEXT("Old empty identity cannot retrieve a reused tile"), GetHandle(OldId).IsValid());
    const FGPUHandle NewHandle = GetHandle(NewId);
    TestTrue(TEXT("New generation retrieves its own pixels"), NewHandle.IsValid());
    TStrongObjectPtr<UTextureRenderTarget2D> Target = MakeTarget(32, 32);
    DrawHandle(Target.Get(), NewHandle);
    CheckPixels(*this, Target.Get(), 0, 0, 57);
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
