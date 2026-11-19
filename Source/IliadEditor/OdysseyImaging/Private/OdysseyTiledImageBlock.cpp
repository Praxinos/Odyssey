// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyTiledImageBlock.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Math/Float16.h"
#include "RenderingThread.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"

#include "ULISLoaderModule.h"

namespace
{
/** Returns the linear variant of an RGBA format when bLinear is true, so that a conversion only changes channel order and type. */
::ULIS::eFormat WithLinearity(::ULIS::eFormat Format, bool bLinear)
{
    if (!bLinear)
    {
        return Format;
    }
    switch (Format)
    {
        case ::ULIS::Format_RGBA8: return ::ULIS::Format_Linear_RGBA8;
        case ::ULIS::Format_BGRA8: return ::ULIS::Format_Linear_BGRA8;
        case ::ULIS::Format_RGBAF: return ::ULIS::Format_Linear_RGBAF;
        default: return Format;
    }
}

/** Converts SourceRect of the source block to the origin of the destination block, and waits for completion. */
void ConvertBlock(const ::ULIS::FBlock& Source, ::ULIS::FBlock& Destination, const ::ULIS::FRectI& SourceRect)
{
    ::ULIS::FContext& Context = IULISLoaderModule::StaticFindOrAddContext(Destination.Format());
    Context.ConvertFormat(Source, Destination, SourceRect, ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::MultiScanlines);
    Context.Finish();
}

/** Returns true for formats with 8-bit integer channels. */
bool Is8Bit(::ULIS::eFormat Format)
{
    return !ULIS_R_FLOATING(Format) && ULIS_R_DEPTH(Format) == 1;
}

/** Decodes an RGBA8 block holding sRGB encoded colors into linear float RGBA values, alpha being linear already. */
void DecodeSRGB(const ::ULIS::FBlock& Source, ::ULIS::FBlock& Destination)
{
    for (int32 Y = 0; Y < Source.Height(); ++Y)
    {
        const uint8* SourceValues = Source.ScanlineBits(static_cast<uint16>(Y));
        float* DestinationValues = reinterpret_cast<float*>(Destination.ScanlineBits(static_cast<uint16>(Y)));
        for (int32 X = 0; X < Source.Width(); ++X)
        {
            const FLinearColor Color = FLinearColor::FromSRGBColor(FColor(SourceValues[0], SourceValues[1], SourceValues[2], SourceValues[3]));
            DestinationValues[0] = Color.R;
            DestinationValues[1] = Color.G;
            DestinationValues[2] = Color.B;
            DestinationValues[3] = Color.A;
            SourceValues += 4;
            DestinationValues += 4;
        }
    }
}

/**
 * Creates a transient texture holding SourceRect of the block in the tile pixel format, ready to be read by CopyFromTexture().
 * Without bSRGB, values are kept, so only channel order and type change. SourceRect must lie within the block.
 */
UTexture2D* CreateTileTexture(const UOdysseyTiledImage& Image, const ::ULIS::FBlock& Block, const FIntRect& SourceRect, bool bSRGB)
{
    // FIntRect::IsEmpty() is false when only one dimension is zero.
    if (SourceRect.Width() <= 0 || SourceRect.Height() <= 0)
    {
        return nullptr;
    }

    // Convert to the memory layout of the tiles.
    const EOdysseyTiledImageFormat TileFormat = Image.GetFormat();
    const bool bDecode = bSRGB && Is8Bit(Block.Format());
    // Linear colors need more than 8 bits to keep every sRGB encoded color.
    if (bDecode && TileFormat == EOdysseyTiledImageFormat::RGBA8)
    {
        return nullptr;
    }
    const FIntPoint Size = SourceRect.Size();
    const bool bLinear = ULIS_R_LINEAR(Block.Format()) != 0;
    const ::ULIS::FRectI Rect(SourceRect.Min.X, SourceRect.Min.Y, Size.X, Size.Y);
    ::ULIS::FBlock Staging(Size.X, Size.Y, WithLinearity(Odyssey::TiledImage::ULISFormatForTiledImageFormat(TileFormat), bLinear));
    if (bDecode)
    {
        ::ULIS::FBlock Encoded(Size.X, Size.Y, WithLinearity(::ULIS::Format_RGBA8, bLinear));
        ConvertBlock(Block, Encoded, Rect);
        DecodeSRGB(Encoded, Staging);
    }
    else
    {
        ConvertBlock(Block, Staging, Rect);
    }

    const EPixelFormat PixelFormat = Image.GetPixelFormat();
    UTexture2D* Texture = UTexture2D::CreateTransient(Size.X, Size.Y, PixelFormat);
    if (!Texture)
    {
        return nullptr;
    }
    Texture->SRGB = false;
    FByteBulkData& BulkData = Texture->GetPlatformData()->Mips[0].BulkData;
    uint8* Destination = static_cast<uint8*>(BulkData.Lock(LOCK_READ_WRITE));
    const int64 DestinationPitch = int64(Size.X) * GPixelFormats[PixelFormat].BlockBytes;
    const int32 NumValuesPerRow = Size.X * 4;
    for (int32 Y = 0; Y < Size.Y; ++Y)
    {
        const uint8* SourceRow = Staging.ScanlineBits(static_cast<uint16>(Y));
        uint8* DestinationRow = Destination + Y * DestinationPitch;
        if (TileFormat == EOdysseyTiledImageFormat::RGBA16F)
        {
            const float* SourceValues = reinterpret_cast<const float*>(SourceRow);
            FFloat16* DestinationValues = reinterpret_cast<FFloat16*>(DestinationRow);
            for (int32 Index = 0; Index < NumValuesPerRow; ++Index)
            {
                DestinationValues[Index] = FFloat16(SourceValues[Index]);
            }
        }
        else
        {
            FMemory::Memcpy(DestinationRow, SourceRow, DestinationPitch);
        }
    }
    BulkData.Unlock();
    Texture->UpdateResource();
    return Texture;
}
}

namespace Odyssey::TiledImage
{
EOdysseyTiledImageFormat TiledImageFormatForULISFormat(::ULIS::eFormat Format, bool bSRGB)
{
    if (Is8Bit(Format))
    {
        // Half floats hold every decoded 8-bit sRGB color closely enough to encode it back exactly.
        return bSRGB ? EOdysseyTiledImageFormat::RGBA16F : EOdysseyTiledImageFormat::RGBA8;
    }
    // Half floats cannot hold 16-bit integers exactly.
    return EOdysseyTiledImageFormat::RGBA32F;
}

::ULIS::eFormat ULISFormatForTiledImageFormat(EOdysseyTiledImageFormat Format)
{
    return Format == EOdysseyTiledImageFormat::RGBA8 ? ::ULIS::Format_RGBA8 : ::ULIS::Format_RGBAF;
}

bool CopyFromBlock(UOdysseyTiledImage& Image, const ::ULIS::FBlock& Block, FIntRect SourceRect, FIntPoint Position, bool bSRGB)
{
    check(IsInGameThread());
    TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::TiledImage::CopyFromBlock);
    SourceRect.Clip(FIntRect(0, 0, Block.Width(), Block.Height()));
    TStrongObjectPtr<UTexture2D> Texture(CreateTileTexture(Image, Block, SourceRect, bSRGB));
    if (!Texture)
    {
        return false;
    }
    Image.CopyFromTexture(Texture.Get(), FIntRect(FIntPoint::ZeroValue, SourceRect.Size()), Position);
    // The tiles are written on the render thread, which reads the texture.
    FlushRenderingCommands();
    return true;
}

bool CopyFromBlock(UOdysseyTiledImage& Image, const ::ULIS::FBlock& Block, const TArray<FIntRect>& Rects, bool bSRGB)
{
    check(IsInGameThread());
    TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::TiledImage::CopyFromBlockRects);
    const FIntRect BlockRect(0, 0, Block.Width(), Block.Height());
    TArray<FIntRect> Clipped;
    Clipped.Reserve(Rects.Num());
    FIntRect Bounds;
    for (FIntRect Rect : Rects)
    {
        Rect.Clip(BlockRect);
        if (Rect.Width() > 0 && Rect.Height() > 0)
        {
            if (Clipped.IsEmpty())
            {
                Bounds = Rect;
            }
            else
            {
                Bounds.Union(Rect);
            }
            Clipped.Add(Rect);
        }
    }
    // Upload the bounds once, then only replace the tiles touched by each rect.
    TStrongObjectPtr<UTexture2D> Texture(CreateTileTexture(Image, Block, Bounds, bSRGB));
    if (!Texture)
    {
        return false;
    }
    for (const FIntRect& Rect : Clipped)
    {
        Image.CopyFromTexture(Texture.Get(), Rect - Bounds.Min, Rect.Min);
    }
    // The tiles are written on the render thread, which reads the texture.
    FlushRenderingCommands();
    return true;
}

TSharedPtr<::ULIS::FBlock> CopyToBlock(const UOdysseyTiledImage& Image, FIntRect Rect, ::ULIS::eFormat Format, bool bSRGB)
{
    check(IsInGameThread());
    TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::TiledImage::CopyToBlock);
    const FIntPoint Size = Rect.Size();
    if (Size.X <= 0 || Size.Y <= 0 || Size.X > MAX_uint16 || Size.Y > MAX_uint16)
    {
        return nullptr;
    }

    TStrongObjectPtr<UTextureRenderTarget2D> Target(Image.CreateRenderTarget(Size));
    Image.Render(Target.Get(), Rect, FIntPoint::ZeroValue);
    FTextureRenderTargetResource* Resource = Target->GameThread_GetRenderTargetResource();
    const bool bLinear = ULIS_R_LINEAR(Format) != 0;
    const int32 NumPixels = Size.X * Size.Y;
    TSharedPtr<::ULIS::FBlock> Result = MakeShared<::ULIS::FBlock>(Size.X, Size.Y, Format);
    if (bSRGB && Is8Bit(Format) && Image.GetFormat() != EOdysseyTiledImageFormat::RGBA8)
    {
        TArray<FLinearColor> Pixels;
        if (!Resource->ReadLinearColorPixels(Pixels, FReadSurfaceDataFlags(RCM_MinMax)) || Pixels.Num() != NumPixels)
        {
            return nullptr;
        }
        // Encode the linear colors, alpha excepted; FColor is stored as BGRA.
        TArray<FColor> Encoded;
        Encoded.SetNumUninitialized(NumPixels);
        for (int32 Index = 0; Index < NumPixels; ++Index)
        {
            const FLinearColor& Color = Pixels[Index];
            Encoded[Index] = FLinearColor(Color.R, Color.G, Color.B, 1.0f).ToFColorSRGB();
            Encoded[Index].A = static_cast<uint8>(FMath::RoundToInt(FMath::Clamp(Color.A, 0.0f, 1.0f) * 255.0f));
        }
        ::ULIS::FBlock Staging(reinterpret_cast<uint8*>(Encoded.GetData()), Size.X, Size.Y, WithLinearity(::ULIS::Format_BGRA8, bLinear));
        ConvertBlock(Staging, *Result, ::ULIS::FRectI::Auto);
    }
    else if (Image.GetFormat() == EOdysseyTiledImageFormat::RGBA8)
    {
        TArray<FColor> Pixels;
        FReadSurfaceDataFlags Flags(RCM_UNorm);
        Flags.SetLinearToGamma(false);
        if (!Resource->ReadPixels(Pixels, Flags) || Pixels.Num() != NumPixels)
        {
            return nullptr;
        }
        // FColor is stored as BGRA.
        ::ULIS::FBlock Staging(reinterpret_cast<uint8*>(Pixels.GetData()), Size.X, Size.Y, WithLinearity(::ULIS::Format_BGRA8, bLinear));
        ConvertBlock(Staging, *Result, ::ULIS::FRectI::Auto);
    }
    else
    {
        TArray<FLinearColor> Pixels;
        if (!Resource->ReadLinearColorPixels(Pixels, FReadSurfaceDataFlags(RCM_MinMax)) || Pixels.Num() != NumPixels)
        {
            return nullptr;
        }
        ::ULIS::FBlock Staging(reinterpret_cast<uint8*>(Pixels.GetData()), Size.X, Size.Y, WithLinearity(::ULIS::Format_RGBAF, bLinear));
        ConvertBlock(Staging, *Result, ::ULIS::FRectI::Auto);
    }
    return Result;
}
}
