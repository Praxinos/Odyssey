// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTiledImage.h"

#include "Engine/Texture.h"

#include "OdysseyTileUtils.h"

UOdysseyTiledImage::~UOdysseyTiledImage()
{
}

UOdysseyTiledImage::UOdysseyTiledImage()
{
}

void
UOdysseyTiledImage::Initialize(int InTileSize, EOdysseyTiledImageFormat InFormat)
{
    TileSize = InTileSize;
    Format = InFormat;

    Tiles.Empty();
}

EOdysseyTiledImageFormat
UOdysseyTiledImage::GetFormat() const
{
    return Format;
}

EPixelFormat
UOdysseyTiledImage::GetPixelFormat() const
{
    switch(Format)
    {
        case EOdysseyTiledImageFormat::RGBA8: return PF_R8G8B8A8;
        case EOdysseyTiledImageFormat::RGBA16F: return PF_FloatRGBA;
        case EOdysseyTiledImageFormat::RGBA32F: return PF_A32B32G32R32F; //Is actually RGBA32F not ABGR32F (not sure why)
    }
    return PF_B8G8R8A8;
}

/* void
UOdysseyTiledImage::AddWriteTilePass(FRDGBuilder& GraphBuilder, FRDGTextureRef OutTexture, TSharedFuture<FSharedBuffer> InBufferFuture)
{
    GraphBuilder.AddPass(
        RDG_EVENT_NAME("UOdysseyTiledImage::AddWriteTilePass"),
        ERDGPassFlags::Copy,
        [OutTexture, InBufferFuture](FRHICommandList& RHICmdList)
        {
            const FRDGTextureDesc& Desc = OutTexture->Desc;
            const FPixelFormatInfo& PixelFormatInfo = GPixelFormats[Desc.Format];
            uint32 BytesPerPixel = PixelFormatInfo.BlockBytes;
            uint32 BufferStride = BytesPerPixel * Desc.Extent.X;

            FSharedBuffer Buffer = InBufferFuture.Get();

            FUpdateTextureRegion2D region(0, 0, 0, 0, Desc.Extent.X, Desc.Extent.Y);
            RHICmdList.UpdateTexture2D(OutTexture->GetRHI(), 0, region, BufferStride, (const uint8*)Buffer.GetData());
        }
    );
}

void
UOdysseyTiledImage::AddReadTilePass(FRDGBuilder& GraphBuilder, FRDGTextureRef InTexture, TPromise<FSharedBuffer>&& OutBufferPromise)
{
    GraphBuilder.AddPass(
        RDG_EVENT_NAME("UOdysseyTiledImage::AddReadTilePass"),
        ERDGPassFlags::Readback,
        [InTexture, OutBufferPromise](FRHICommandList& RHICmdList)
        {
            const FRDGTextureDesc& Desc = InTexture->Desc;
            const FPixelFormatInfo& PixelFormatInfo = GPixelFormats[Desc.Format];

            uint32 BytesPerPixel = PixelFormatInfo.BlockBytes;
            uint32 BufferStride = BytesPerPixel * Desc.Extent.X;
            uint32 BufferTotalBytes = BufferStride * Desc.Extent.Y;

            //Read Texture into a buffer
            FRHILockTextureArgs LockArgs = FRHILockTextureArgs::Lock2D(InTexture->GetRHI(), 0, RLM_ReadOnly, false);
            FRHILockTextureResult LockResult = RHICmdList.LockTexture(LockArgs);

            FUniqueBuffer Buffer = FUniqueBuffer::Alloc(BufferTotalBytes);
            for (uint32 y = 0; y < textureHeight; y++)
            {
                void* Src = (uint8*)(LockResult.Data) + LockResult.Stride * y;
                void* Dst = (uint8*)(Buffer.GetData()) + BufferStride * y;
                FMemory::Memcpy(Dst, Src, BufferStride);
            }
            RHICmdList.UnlockTexture(LockArgs);
            OutBufferPromise.SetValue(Buffer.MoveToShared());
        }
    );
}*/

void
UOdysseyTiledImage::CopyFromTexture(UTexture* InTexture, FIntRect InRect, FIntPoint InPosition)
{
    FIntRect SourceRect(0, 0, InTexture->GetSurfaceWidth(), InTexture->GetSurfaceHeight());
    SourceRect.Clip(InRect);
    FIntRect DestinationRect(
        InPosition.X + (SourceRect.Min.X - InRect.Min.X),
        InPosition.Y + (SourceRect.Min.Y - InRect.Min.Y),
        InPosition.X + SourceRect.Size().X,
        InPosition.X + SourceRect.Size().Y
    );

    TArray<FIntPoint> TilePositions = Odyssey::TileUtils::GetTilePositionsFromRect(TileSize, DestinationRect);

    TArray<FOdysseyTileManager::FCreateOrUpdateTile> TilesInfos;
    for (const FIntPoint& TilePos : TilePositions)
    {
        FOdysseyTileManager::FCreateOrUpdateTile TileInfos;

        FTile* Tile = Tiles.Find(TilePos);
        TileInfos.Pos = TilePos;
        if (Tile)
        {
            TileInfos.OldTileId = Tile->Id;
            TileInfos.IsNewTile = false;
        }
        else
        {
            TileInfos.IsNewTile = true;
        }

        TilesInfos.Add(TileInfos);
    }

    TArray<FOdysseyTileManager::FTileId> CreatedTiles = FOdysseyTileManager::Get().CreateOrUpdateTiles(
        InTexture,
        InRect,
        InPosition,
        TileSize,
        GetPixelFormat(),
        TilesInfos
    );

    check(CreatedTiles.Num() == TilePositions.Num());

    for (int i = 0; i < TilePositions.Num(); i++)
    {
        FTile Tile;
        Tile.Id = CreatedTiles[i];
        if (TilesInfos[i].IsNewTile)
        {
            Tiles.Add(TilePositions[i], Tile);
        }
        else
        {
            Tiles[i] = Tile;
        }
        //TODO: Someday, remove empty tiles
    }
}

void
UOdysseyTiledImage::CopyFromTiledImage(UOdysseyTiledImage* TiledImage, FIntRect Rect, FIntPoint Position)
{

}

void
UOdysseyTiledImage::Render(UTextureRenderTarget2D* Destination, FIntRect Rect, FIntPoint Position) const
{

}

void
UOdysseyTiledImage::Serialize(FArchive& Ar)
{

}
