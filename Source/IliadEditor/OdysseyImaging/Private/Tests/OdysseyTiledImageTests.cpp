// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyTiledImage.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformProcess.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "RenderingThread.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

#include "OdysseyTileManager.h"
#include "OdysseyTileUtils.h"

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

#endif // WITH_DEV_AUTOMATION_TESTS
