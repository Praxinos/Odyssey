// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyTiledImageBlock.h"

#include "Async/ParallelFor.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Math/Float16.h"
#include "Memory/SharedBuffer.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyTileManager.h"
#include "OdysseyTileUtils.h"

#include "ULISLoaderModule.h"

DEFINE_LOG_CATEGORY_STATIC(LogOdysseyTiledImageBlock, Log, All);

namespace
{
/** The half float of each 8-bit value, decoded as an sRGB color channel or as a linear alpha, as FLinearColor::FromSRGBColor() does. */
struct FSRGBToHalfTables
{
    FSRGBToHalfTables()
    {
        for (int32 Value = 0; Value < 256; ++Value)
        {
            const FColor Encoded(uint8(Value), 0, 0, uint8(Value));
            const FLinearColor Decoded = FLinearColor::FromSRGBColor(Encoded);
            Color[Value] = FFloat16(Decoded.R);
            Alpha[Value] = FFloat16(Decoded.A);
        }
    }

    /** The half float of each sRGB encoded 8-bit color value. */
    FFloat16 Color[256];
    /** The half float of each linear 8-bit alpha value. */
    FFloat16 Alpha[256];
};

const FSRGBToHalfTables& GetSRGBToHalfTables()
{
    static const FSRGBToHalfTables Tables;
    return Tables;
}

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
 * Returns SourceRect of the block in the memory layout of the tiles: rows of SourceRect.Width() pixels in the tile pixel format.
 * Without bSRGB, values are kept, so only channel order and type change. SourceRect must lie within the block.
 * @return A null buffer when SourceRect is empty, or an sRGB encoded 8-bit block cannot be held by RGBA8 tiles.
 */
FSharedBuffer ToTileMemory(EOdysseyTiledImageFormat TileFormat, EPixelFormat PixelFormat, const ::ULIS::FBlock& Block, const FIntRect& SourceRect, bool bSRGB)
{
    // FIntRect::IsEmpty() is false when only one dimension is zero.
    if (SourceRect.Width() <= 0 || SourceRect.Height() <= 0)
    {
        return FSharedBuffer();
    }

    // Convert to the memory layout of the tiles.
    const bool bDecode = bSRGB && Is8Bit(Block.Format());
    // Linear colors need more than 8 bits to keep every sRGB encoded color.
    if (bDecode && TileFormat == EOdysseyTiledImageFormat::RGBA8)
    {
        return FSharedBuffer();
    }
    const FIntPoint Size = SourceRect.Size();
    const bool bLinear = ULIS_R_LINEAR(Block.Format()) != 0;
    const ::ULIS::FRectI Rect(SourceRect.Min.X, SourceRect.Min.Y, Size.X, Size.Y);
    const int64 DestinationPitch = int64(Size.X) * GPixelFormats[PixelFormat].BlockBytes;
    FUniqueBuffer Memory = FUniqueBuffer::Alloc(uint64(DestinationPitch) * Size.Y);
    uint8* Destination = static_cast<uint8*>(Memory.GetData());
    const int32 NumValuesPerRow = Size.X * 4;

    if (bDecode && TileFormat == EOdysseyTiledImageFormat::RGBA16F)
    {
        // Decode the 8-bit values to half floats through lookup tables.
        ::ULIS::FBlock Encoded(Size.X, Size.Y, WithLinearity(::ULIS::Format_RGBA8, bLinear));
        ConvertBlock(Block, Encoded, Rect);
        const FSRGBToHalfTables& Tables = GetSRGBToHalfTables();
        ParallelFor(Size.Y, [&](int32 Row) -> void
        {
            const uint8* SourceValues = Encoded.ScanlineBits(static_cast<uint16>(Row));
            FFloat16* DestinationValues = reinterpret_cast<FFloat16*>(Destination + Row * DestinationPitch);
            for (int32 Index = 0; Index < NumValuesPerRow; Index += 4)
            {
                DestinationValues[Index] = Tables.Color[SourceValues[Index]];
                DestinationValues[Index + 1] = Tables.Color[SourceValues[Index + 1]];
                DestinationValues[Index + 2] = Tables.Color[SourceValues[Index + 2]];
                DestinationValues[Index + 3] = Tables.Alpha[SourceValues[Index + 3]];
            }
        });
        return Memory.MoveToShared();
    }

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

    ParallelFor(Size.Y, [&](int32 Row) -> void
    {
        const uint8* SourceRow = Staging.ScanlineBits(static_cast<uint16>(Row));
        uint8* DestinationRow = Destination + Row * DestinationPitch;
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
    });
    return Memory.MoveToShared();
}

/**
 * Creates a transient texture holding SourceRect of the block in the tile pixel format, ready to be read by CopyFromTexture().
 * Converts as ToTileMemory() does. The texture is garbage collected: keep it for single copies, not repeated ones.
 */
UTexture2D* CreateTileTexture(EOdysseyTiledImageFormat TileFormat, EPixelFormat PixelFormat, const ::ULIS::FBlock& Block, const FIntRect& SourceRect, bool bSRGB)
{
    const FSharedBuffer Memory = ToTileMemory(TileFormat, PixelFormat, Block, SourceRect, bSRGB);
    if (Memory.IsNull())
    {
        return nullptr;
    }
    UTexture2D* Texture = UTexture2D::CreateTransient(SourceRect.Width(), SourceRect.Height(), PixelFormat);
    if (!Texture)
    {
        return nullptr;
    }
    Texture->SRGB = false;
    FByteBulkData& BulkData = Texture->GetPlatformData()->Mips[0].BulkData;
    FMemory::Memcpy(BulkData.Lock(LOCK_READ_WRITE), Memory.GetData(), Memory.GetSize());
    BulkData.Unlock();
    Texture->UpdateResource();
    return Texture;
}
}

BEGIN_SHADER_PARAMETER_STRUCT(FOdysseyBlockUploadParameters, )
    RDG_TEXTURE_ACCESS(Texture, ERHIAccess::CopyDest)
END_SHADER_PARAMETER_STRUCT()

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
    TStrongObjectPtr<UTexture2D> Texture(CreateTileTexture(Image.GetFormat(), Image.GetPixelFormat(), Block, SourceRect, bSRGB));
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
    TArray<FIntRect> Clipped = Odyssey::TileUtils::ClipRects(Rects, BlockRect);
    const FIntRect Bounds = Odyssey::TileUtils::UnionRects(Clipped);
    // Upload the bounds once, then only replace the tiles touched by each rect.
    TStrongObjectPtr<UTexture2D> Texture(CreateTileTexture(Image.GetFormat(), Image.GetPixelFormat(), Block, Bounds, bSRGB));
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

bool SetFromBlock(UOdysseyTiledImage& Image, const ::ULIS::FBlock& Block, EOdysseyBlockEncoding Encoding)
{
    check(IsInGameThread());
    TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::TiledImage::SetFromBlock);
    const int32 TileSize = int32(Image.GetTileSize());
    if (!Odyssey::TileUtils::IsValidTileSize(TileSize))
    {
        UE_LOGF(LogOdysseyTiledImageBlock, Error, "Cannot set the tiled image %ls from a block: invalid tile size %d.", *GetNameSafe(&Image), TileSize);
        return false;
    }

    const bool bSRGB = Encoding == EOdysseyBlockEncoding::SRGB;
    if (bSRGB && Is8Bit(Block.Format()) && Image.GetFormat() == EOdysseyTiledImageFormat::RGBA8)
    {
        UE_LOGF(LogOdysseyTiledImageBlock, Error, "Cannot set the tiled image %ls from an sRGB block: 8-bit tiles cannot hold decoded colors.", *GetNameSafe(&Image));
        return false;
    }

    const EOdysseyTiledImageFormat TileFormat = Image.GetFormat();
    const EPixelFormat PixelFormat = Image.GetPixelFormat();
    const int32 BlockWidth = Block.Width();
    const int32 BlockHeight = Block.Height();
    Image.Initialize(TileSize, TileFormat);
    // A band of tiles at a time, so that the converted pixels never take the memory of the whole block.
    for (int32 BandY = 0; BandY < BlockHeight; BandY += TileSize)
    {
        const int32 BandEndY = FMath::Min(BandY + TileSize, BlockHeight);
        const FIntRect Band(0, BandY, BlockWidth, BandEndY);
        const FSharedBuffer Memory = ToTileMemory(TileFormat, PixelFormat, Block, Band, bSRGB);
        if (Memory.IsNull())
        {
            continue;
        }

        const FIntPoint BandSize = Band.Size();
        const FMemoryView BandPixels = Memory.GetView();
        Image.ReplaceTilesFromPixels(Band.Min, BandSize, BandPixels);
    }
    // The tile manager tick, which releases them otherwise, does not run while a package loads.
    FOdysseyTileManager::Get().EvictTiles();
    return true;
}

bool CopyBlockToRenderTarget(UTextureRenderTarget2D& Target, EOdysseyTiledImageFormat Format, const ::ULIS::FBlock& Block, const TArray<FIntRect>& Rects, bool bSRGB, FIntPoint Position)
{
    check(IsInGameThread());
    TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::TiledImage::CopyBlockToRenderTarget);
    const EPixelFormat PixelFormat = Target.GetFormat();
    // The parts of the block landing within the target.
    FIntRect BlockRect(0, 0, Block.Width(), Block.Height());
    BlockRect.Clip(FIntRect(FIntPoint::ZeroValue - Position, FIntPoint(Target.SizeX, Target.SizeY) - Position));
    TArray<FIntRect> Clipped = Odyssey::TileUtils::ClipRects(Rects, BlockRect);
    const FIntRect Bounds = Odyssey::TileUtils::UnionRects(Clipped);
    // Tools upload their edits on every tick: the pixels are written into the target directly, without creating a texture.
    const FSharedBuffer Memory = ToTileMemory(Format, PixelFormat, Block, Bounds, bSRGB);
    if (Memory.IsNull())
    {
        return false;
    }
    const uint32 BytesPerPixel = GPixelFormats[PixelFormat].BlockBytes;
    const uint32 Pitch = uint32(Bounds.Width()) * BytesPerPixel;
    ENQUEUE_RENDER_COMMAND(OdysseyCopyBlockToRenderTarget)(
        [Destination = Target.GameThread_GetRenderTargetResource(), Clipped = MoveTemp(Clipped), Bounds, Position, Memory, BytesPerPixel, Pitch](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder GraphBuilder(RHICmdList);
            FRDGTextureRef DestinationTexture = Destination->GetRenderTargetTexture(GraphBuilder);
            for (const FIntRect& Rect : Clipped)
            {
                FOdysseyBlockUploadParameters* Parameters = GraphBuilder.AllocParameters<FOdysseyBlockUploadParameters>();
                Parameters->Texture = DestinationTexture;
                GraphBuilder.AddPass(
                    RDG_EVENT_NAME("Odyssey.TiledImage.BlockUpload"),
                    Parameters,
                    ERDGPassFlags::Copy | ERDGPassFlags::NeverCull,
                    [DestinationTexture, Memory, Rect, Bounds, Position, BytesPerPixel, Pitch](FRHICommandList& PassCmdList)
                    {
                        // The data starts at the first pixel of the rect; rows keep the pitch of the bounds.
                        const FIntPoint Source = Rect.Min - Bounds.Min;
                        const uint8* Data = static_cast<const uint8*>(Memory.GetData()) + uint64(Source.Y) * Pitch + uint64(Source.X) * BytesPerPixel;
                        const FUpdateTextureRegion2D Region(Rect.Min.X + Position.X, Rect.Min.Y + Position.Y, 0, 0, Rect.Width(), Rect.Height());
                        PassCmdList.UpdateTexture2D(DestinationTexture->GetRHI(), 0, Region, Pitch, Data);
                    });
            }
            GraphBuilder.Execute();
        });
    // Callers expect the target to hold the pixels once this returns.
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
