// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyTiledImageBlock.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Editor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Misc/AutomationTest.h"
#include "RenderingThread.h"
#include "ScopedTransaction.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"

namespace Odyssey::TiledImageBlockTests
{
constexpr EAutomationTestFlags TestFlags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;

/** A block format and the tile format storing it, with the tolerance of a round trip, in source units. */
struct FFormatCase
{
    ::ULIS::eFormat BlockFormat;
    EOdysseyTiledImageFormat TileFormat;
    double Tolerance;
    const TCHAR* Name;
};

const FFormatCase FormatCases[] = {
    { ::ULIS::Format_BGRA8, EOdysseyTiledImageFormat::RGBA8, 0.0, TEXT("BGRA8 in RGBA8") },
    { ::ULIS::Format_RGBA8, EOdysseyTiledImageFormat::RGBA8, 0.0, TEXT("RGBA8 in RGBA8") },
    { ::ULIS::Format_RGBA16, EOdysseyTiledImageFormat::RGBA32F, 1.0, TEXT("RGBA16 in RGBA32F") },
    { ::ULIS::Format_RGBAF, EOdysseyTiledImageFormat::RGBA32F, 0.0, TEXT("RGBAF in RGBA32F") },
    // Multiples of 1/256 are exact in half floats.
    { ::ULIS::Format_RGBAF, EOdysseyTiledImageFormat::RGBA16F, 0.0, TEXT("RGBAF in RGBA16F") },
};

/** Returns the raw value of a channel, in memory order. */
double GetValue(const ::ULIS::FBlock& Block, int32 X, int32 Y, int32 Channel)
{
    const uint8* Pixel = Block.PixelBits(static_cast<uint16>(X), static_cast<uint16>(Y));
    const ::ULIS::eFormat Format = Block.Format();
    if (ULIS_R_FLOATING(Format))
    {
        return reinterpret_cast<const float*>(Pixel)[Channel];
    }
    if (ULIS_R_DEPTH(Format) == 2)
    {
        return reinterpret_cast<const uint16*>(Pixel)[Channel];
    }
    return Pixel[Channel];
}

/** Fills a block with a pattern depending on Seed. Every pixel is opaque enough to be kept in its tile. */
TSharedPtr<::ULIS::FBlock> MakeBlock(int32 Width, int32 Height, ::ULIS::eFormat Format, int32 Seed)
{
    TSharedPtr<::ULIS::FBlock> Block = MakeShared<::ULIS::FBlock>(Width, Height, Format);
    const bool bFloating = ULIS_R_FLOATING(Format) != 0;
    const bool b16Bits = ULIS_R_DEPTH(Format) == 2;
    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            uint8* Pixel = Block->PixelBits(static_cast<uint16>(X), static_cast<uint16>(Y));
            for (int32 Channel = 0; Channel < 4; ++Channel)
            {
                const int32 Value = 32 + (X * 7 + Y * 13 + Channel * 31 + Seed) % 224;
                if (bFloating)
                {
                    reinterpret_cast<float*>(Pixel)[Channel] = Value / 256.f;
                }
                else if (b16Bits)
                {
                    reinterpret_cast<uint16*>(Pixel)[Channel] = static_cast<uint16>(Value * 256 + (X + Y) % 200);
                }
                else
                {
                    Pixel[Channel] = static_cast<uint8>(Value);
                }
            }
        }
    }
    return Block;
}

/**
 * Checks that Actual, read at ActualOrigin, matches Expected (at ExpectedOrigin) over Size.
 * A null Expected means transparent pixels.
 */
bool CheckBlock(FAutomationTestBase& Test, const TCHAR* What, const ::ULIS::FBlock* Actual, FIntPoint ActualOrigin,
    const ::ULIS::FBlock* Expected, FIntPoint ExpectedOrigin, FIntPoint Size, double Tolerance)
{
    if (!Test.TestNotNull(What, Actual))
    {
        return false;
    }
    for (int32 Y = 0; Y < Size.Y; ++Y)
    {
        for (int32 X = 0; X < Size.X; ++X)
        {
            for (int32 Channel = 0; Channel < 4; ++Channel)
            {
                const double ExpectedValue = Expected ? GetValue(*Expected, ExpectedOrigin.X + X, ExpectedOrigin.Y + Y, Channel) : 0.0;
                const double ActualValue = GetValue(*Actual, ActualOrigin.X + X, ActualOrigin.Y + Y, Channel);
                if (FMath::Abs(ActualValue - ExpectedValue) > Tolerance)
                {
                    Test.AddError(FString::Printf(TEXT("%s: pixel (%d, %d) channel %d: expected %f, got %f"),
                        What, X, Y, Channel, ExpectedValue, ActualValue));
                    return false;
                }
            }
        }
    }
    return true;
}

TStrongObjectPtr<UOdysseyTiledImage> MakeImage(EOdysseyTiledImageFormat Format)
{
    TStrongObjectPtr<UOdysseyTiledImage> Image(NewObject<UOdysseyTiledImage>());
    Image->Initialize(32, Format);
    return Image;
}
}

using namespace Odyssey::TiledImageBlockTests;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTiledImageBlockFormatsTest, "Odyssey.Imaging.TiledImage.Block.FormatRoundTrip", TestFlags)
bool FOdysseyTiledImageBlockFormatsTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TiledImage;
    TestEqual(TEXT("8-bit blocks use RGBA8 tiles"), TiledImageFormatForULISFormat(::ULIS::Format_BGRA8), EOdysseyTiledImageFormat::RGBA8);
    TestEqual(TEXT("Linear 8-bit blocks use RGBA8 tiles"), TiledImageFormatForULISFormat(::ULIS::Format_Linear_RGBA8), EOdysseyTiledImageFormat::RGBA8);
    TestEqual(TEXT("16-bit blocks use RGBA32F tiles"), TiledImageFormatForULISFormat(::ULIS::Format_RGBA16), EOdysseyTiledImageFormat::RGBA32F);
    TestEqual(TEXT("Float blocks use RGBA32F tiles"), TiledImageFormatForULISFormat(::ULIS::Format_RGBAF), EOdysseyTiledImageFormat::RGBA32F);

    // Not aligned on tiles, partly at negative coordinates.
    const FIntRect SourceRect(5, 3, 95, 67);
    const FIntPoint Position(-37, 21);
    const FIntPoint Size = SourceRect.Size();
    for (const FFormatCase& Case : FormatCases)
    {
        TSharedPtr<::ULIS::FBlock> Source = MakeBlock(100, 70, Case.BlockFormat, 11);
        TStrongObjectPtr<UOdysseyTiledImage> Image = MakeImage(Case.TileFormat);
        TestTrue(FString::Printf(TEXT("%s: copy"), Case.Name), CopyFromBlock(*Image, *Source, SourceRect, Position));

        // Read a margin around the copy, which must stay transparent.
        const FIntRect ReadRect(Position - FIntPoint(4, 4), Position + Size + FIntPoint(4, 4));
        TSharedPtr<::ULIS::FBlock> Read = CopyToBlock(*Image, ReadRect, Case.BlockFormat);
        CheckBlock(*this, Case.Name, Read.Get(), FIntPoint(4, 4), Source.Get(), SourceRect.Min, Size, Case.Tolerance);
        CheckBlock(*this, Case.Name, Read.Get(), FIntPoint::ZeroValue, nullptr, FIntPoint::ZeroValue, FIntPoint(ReadRect.Width(), 4), 0.0);
    }

    TSharedPtr<::ULIS::FBlock> Source = MakeBlock(16, 16, ::ULIS::Format_RGBA8, 0);
    TStrongObjectPtr<UOdysseyTiledImage> Image = MakeImage(EOdysseyTiledImageFormat::RGBA8);
    TestFalse(TEXT("A rectangle outside the block copies nothing"), CopyFromBlock(*Image, *Source, FIntRect(20, 20, 30, 30), FIntPoint::ZeroValue));
    TestFalse(TEXT("An empty rectangle copies nothing"), CopyFromBlock(*Image, *Source, FIntRect(4, 4, 4, 8), FIntPoint::ZeroValue));
    TestNull(TEXT("An empty rectangle reads nothing"), CopyToBlock(*Image, FIntRect(0, 0, 0, 8), ::ULIS::Format_RGBA8).Get());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTiledImageBlockPartialTest, "Odyssey.Imaging.TiledImage.Block.PartialCopyKeepsSurroundingPixels", TestFlags)
bool FOdysseyTiledImageBlockPartialTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TiledImage;
    TSharedPtr<::ULIS::FBlock> Background = MakeBlock(96, 96, ::ULIS::Format_RGBA8, 3);
    TSharedPtr<::ULIS::FBlock> Patch = MakeBlock(20, 10, ::ULIS::Format_RGBA8, 101);
    TStrongObjectPtr<UOdysseyTiledImage> Image = MakeImage(EOdysseyTiledImageFormat::RGBA8);
    CopyFromBlock(*Image, *Background, FIntRect(0, 0, 96, 96), FIntPoint::ZeroValue);
    // Crosses the boundary of four 32-pixel tiles.
    const FIntPoint PatchPosition(25, 27);
    CopyFromBlock(*Image, *Patch, FIntRect(0, 0, 20, 10), PatchPosition);

    TSharedPtr<::ULIS::FBlock> Read = CopyToBlock(*Image, FIntRect(0, 0, 96, 96), ::ULIS::Format_RGBA8);
    if (!TestNotNull(TEXT("Read image"), Read.Get()))
    {
        return false;
    }
    for (int32 Y = 0; Y < 96; ++Y)
    {
        for (int32 X = 0; X < 96; ++X)
        {
            const FIntPoint InPatch = FIntPoint(X, Y) - PatchPosition;
            const bool bInPatch = InPatch.X >= 0 && InPatch.Y >= 0 && InPatch.X < 20 && InPatch.Y < 10;
            const ::ULIS::FBlock& Expected = bInPatch ? *Patch : *Background;
            const FIntPoint ExpectedOrigin = bInPatch ? InPatch : FIntPoint(X, Y);
            if (!CheckBlock(*this, bInPatch ? TEXT("Patch") : TEXT("Background"), Read.Get(), FIntPoint(X, Y), &Expected, ExpectedOrigin, FIntPoint(1, 1), 0.0))
            {
                return false;
            }
        }
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTiledImageCompatibleTargetTest, "Odyssey.Imaging.TiledImage.Block.CompatibleRenderTarget", TestFlags)
bool FOdysseyTiledImageCompatibleTargetTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TiledImage;
    for (const FFormatCase& Case : FormatCases)
    {
        TSharedPtr<::ULIS::FBlock> Source = MakeBlock(70, 50, Case.BlockFormat, 57);
        TStrongObjectPtr<UOdysseyTiledImage> Image = MakeImage(Case.TileFormat);
        CopyFromBlock(*Image, *Source, FIntRect(0, 0, 70, 50), FIntPoint(9, -14));

        // Render to a compatible target, then copy it into another image: the path of render-target based editing.
        TStrongObjectPtr<UTextureRenderTarget2D> Target(Image->CreateRenderTarget(FIntPoint(70, 50)));
        if (!TestNotNull(Case.Name, Target.Get()))
        {
            continue;
        }
        TestEqual(FString::Printf(TEXT("%s: target uses the tile pixel format"), Case.Name), Target->GetFormat(), Image->GetPixelFormat());
        Image->Render(Target.Get(), FIntRect(9, -14, 79, 36), FIntPoint::ZeroValue);
        TStrongObjectPtr<UOdysseyTiledImage> Copy = MakeImage(Case.TileFormat);
        Copy->CopyFromTexture(Target.Get(), FIntRect(0, 0, 70, 50), FIntPoint(9, -14));
        FlushRenderingCommands();

        TSharedPtr<::ULIS::FBlock> Read = CopyToBlock(*Copy, FIntRect(9, -14, 79, 36), Case.BlockFormat);
        CheckBlock(*this, Case.Name, Read.Get(), FIntPoint::ZeroValue, Source.Get(), FIntPoint::ZeroValue, FIntPoint(70, 50), Case.Tolerance);
    }
    TStrongObjectPtr<UOdysseyTiledImage> Image = MakeImage(EOdysseyTiledImageFormat::RGBA8);
    TestNull(TEXT("An empty size creates no target"), Image->CreateRenderTarget(FIntPoint(0, 16)));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTiledImageBlockUndoTest, "Odyssey.Imaging.TiledImage.Block.NativeUndo", TestFlags)
bool FOdysseyTiledImageBlockUndoTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TiledImage;
    if (!TestNotNull(TEXT("Editor"), GEditor))
    {
        return false;
    }
    TSharedPtr<::ULIS::FBlock> Before = MakeBlock(80, 48, ::ULIS::Format_RGBA8, 5);
    TSharedPtr<::ULIS::FBlock> After = MakeBlock(80, 48, ::ULIS::Format_RGBA8, 77);
    TStrongObjectPtr<UOdysseyTiledImage> Image = MakeImage(EOdysseyTiledImageFormat::RGBA8);
    TestTrue(TEXT("Initialized image is transactional"), Image->HasAnyFlags(RF_Transactional));
    CopyFromBlock(*Image, *Before, FIntRect(0, 0, 80, 48), FIntPoint::ZeroValue);
    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyTiledImageBlockTests", "Edit", "Tiled image block edit"));
        // Only a part of the tiles change.
        CopyFromBlock(*Image, *After, FIntRect(0, 0, 40, 30), FIntPoint::ZeroValue);
    }
    const FIntRect Bounds(0, 0, 80, 48);

    TestTrue(TEXT("Undo"), GEditor->UndoTransaction());
    TSharedPtr<::ULIS::FBlock> Undone = CopyToBlock(*Image, Bounds, ::ULIS::Format_RGBA8);
    CheckBlock(*this, TEXT("After undo"), Undone.Get(), FIntPoint::ZeroValue, Before.Get(), FIntPoint::ZeroValue, Bounds.Size(), 0.0);

    TestTrue(TEXT("Redo"), GEditor->RedoTransaction());
    TSharedPtr<::ULIS::FBlock> Redone = CopyToBlock(*Image, Bounds, ::ULIS::Format_RGBA8);
    CheckBlock(*this, TEXT("After redo, edited part"), Redone.Get(), FIntPoint::ZeroValue, After.Get(), FIntPoint::ZeroValue, FIntPoint(40, 30), 0.0);
    CheckBlock(*this, TEXT("After redo, untouched part"), Redone.Get(), FIntPoint(40, 0), Before.Get(), FIntPoint(40, 0), FIntPoint(40, 48), 0.0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTiledImageBlockSRGBTest, "Odyssey.Imaging.TiledImage.Block.SRGBRoundTrip", TestFlags)
/** 8-bit sRGB encoded blocks are stored as linear colors, and every encoded value is read back exactly. */
bool FOdysseyTiledImageBlockSRGBTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TiledImage;
    TestEqual(TEXT("sRGB 8-bit blocks are stored in half floats"), TiledImageFormatForULISFormat(::ULIS::Format_BGRA8, true), EOdysseyTiledImageFormat::RGBA16F);
    TestEqual(TEXT("Linear 8-bit blocks are stored as they are"), TiledImageFormatForULISFormat(::ULIS::Format_BGRA8, false), EOdysseyTiledImageFormat::RGBA8);

    // Every 8-bit value appears in every color channel; BGRA8 is stored as B, G, R, A.
    constexpr int32 Size = 16;
    TSharedPtr<::ULIS::FBlock> Block = MakeShared<::ULIS::FBlock>(Size, Size, ::ULIS::Format_BGRA8);
    TArray<FColor> Colors;
    for (int32 Index = 0; Index < Size * Size; ++Index)
    {
        const FColor Color(uint8((Index * 7) % 256), uint8(255 - Index), uint8(Index), uint8(1 + Index % 255));
        uint8* Pixel = Block->PixelBits(static_cast<uint16>(Index % Size), static_cast<uint16>(Index / Size));
        Pixel[0] = Color.B;
        Pixel[1] = Color.G;
        Pixel[2] = Color.R;
        Pixel[3] = Color.A;
        Colors.Add(Color);
    }

    TStrongObjectPtr<UOdysseyTiledImage> Linear8 = MakeImage(EOdysseyTiledImageFormat::RGBA8);
    TestFalse(TEXT("8-bit tiles cannot hold decoded colors"), CopyFromBlock(*Linear8, *Block, FIntRect(0, 0, Size, Size), FIntPoint::ZeroValue, true));

    TStrongObjectPtr<UOdysseyTiledImage> Image = MakeImage(EOdysseyTiledImageFormat::RGBA16F);
    if (!TestTrue(TEXT("Copy sRGB block"), CopyFromBlock(*Image, *Block, FIntRect(0, 0, Size, Size), FIntPoint::ZeroValue, true)))
    {
        return false;
    }

    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(Size, Size, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    Image->Render(Target.Get(), FIntRect(0, 0, Size, Size), FIntPoint::ZeroValue);
    FlushRenderingCommands();
    TArray<FLinearColor> Pixels;
    FReadSurfaceDataFlags Flags(RCM_MinMax);
    Flags.SetLinearToGamma(false);
    if (!TestTrue(TEXT("Read image"), Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags))
        || !TestEqual(TEXT("Image pixel count"), Pixels.Num(), Size * Size))
    {
        return false;
    }
    for (int32 Index = 0; Index < Pixels.Num(); ++Index)
    {
        const FLinearColor Expected = FLinearColor::FromSRGBColor(Colors[Index]);
        // Half floats keep about three decimal digits.
        if (!Pixels[Index].Equals(Expected, 0.001f))
        {
            AddError(FString::Printf(TEXT("Pixel %d is not the decoded color: expected %s, got %s"), Index, *Expected.ToString(), *Pixels[Index].ToString()));
            break;
        }
    }

    TSharedPtr<::ULIS::FBlock> Read = CopyToBlock(*Image, FIntRect(0, 0, Size, Size), ::ULIS::Format_BGRA8, true);
    CheckBlock(*this, TEXT("Encoded colors are read back exactly"), Read.Get(), FIntPoint::ZeroValue, Block.Get(), FIntPoint::ZeroValue, FIntPoint(Size, Size), 0.0);
    return true;
}

#endif
