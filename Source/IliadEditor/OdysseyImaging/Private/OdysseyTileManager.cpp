// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTileManager.h"

#include "Async/Async.h"
#include "Engine/Texture.h"
#include "RenderingThread.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "Stats/Stats.h"
#include "TextureResource.h"

#include "OdysseyTileUtils.h"

DECLARE_STATS_GROUP(TEXT("FOdysseyTileManager"), STATGROUP_OdysseyTileManager, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("FOdysseyTileManager::CreateTiles"), STAT_CreateTiles, STATGROUP_OdysseyTileManager);

//static
FOdysseyTileManager&
FOdysseyTileManager::Get()
{
    static FOdysseyTileManager self;
    return self;
}


BEGIN_SHADER_PARAMETER_STRUCT(FWriteTileParameters, )
    RDG_TEXTURE_ACCESS(Texture, ERHIAccess::CopyDest)
END_SHADER_PARAMETER_STRUCT()

//static
void
FOdysseyTileManager::AddWriteTilePass(FRDGBuilder& GraphBuilder, FRDGTextureRef OutTexture, FSharedBuffer InBuffer)
{
    FWriteTileParameters* Params = GraphBuilder.AllocParameters<FWriteTileParameters>();
    Params->Texture = OutTexture;

    GraphBuilder.AddPass(
        RDG_EVENT_NAME("UOdysseyTiledImage::AddWriteTilePass"),
        Params,
        ERDGPassFlags::Copy | ERDGPassFlags::NeverCull,
        [OutTexture, InBuffer](FRHICommandList& RHICmdList)
        {
            const FRDGTextureDesc& Desc = OutTexture->Desc;
            const FPixelFormatInfo& PixelFormatInfo = GPixelFormats[Desc.Format];
            uint32 BytesPerPixel = PixelFormatInfo.BlockBytes;
            uint32 BufferStride = BytesPerPixel * Desc.Extent.X;

            FUpdateTextureRegion2D region(0, 0, 0, 0, Desc.Extent.X, Desc.Extent.Y);
            RHICmdList.UpdateTexture2D(OutTexture->GetRHI(), 0, region, BufferStride, (const uint8*)InBuffer.GetData());
        }
    );
}

TArray<FOdysseyTileManager::FCreatedTile>
FOdysseyTileManager::CreateOrUpdateTiles(
    UTexture* InTexture,
    FIntRect InRect,
    FIntPoint InPosition,
    uint32 InTileSize,
    EPixelFormat InTileFormat,
    const FGetExistingTileId& InGetExistingTileId
)
{
    FIntRect SourceRect(0, 0, InTexture->GetSurfaceWidth(), InTexture->GetSurfaceHeight());
    SourceRect.Clip(InRect);
    FIntRect DestinationRect(
        InPosition + (SourceRect.Min - InRect.Min),
        InPosition + SourceRect.Size()
    );

    TArray<FIntPoint> TilePositions = Odyssey::TileUtils::GetTilePositionsFromRect(InTileSize, DestinationRect);
    TArray<FSharedBuffer> TileOldBuffers;
    TArray<TSharedPtr<FRHIGPUTextureReadback>> TileReadBacks;
    TileOldBuffers.Reserve(TilePositions.Num());
    TileReadBacks.Reserve(TilePositions.Num());
    for (const FIntPoint& TilePosition : TilePositions)
    {
        FTileId OldTileId;
        if (InGetExistingTileId.IsBound())
            OldTileId = InGetExistingTileId.Execute(TilePosition);

        FSharedBuffer OldTileBuffer;
        if (OldTileId.IsValid())
            GetTileBuffer(OldTileId, OldTileBuffer);

        TileOldBuffers.Add(OldTileBuffer);
        TileReadBacks.Add(MakeShared<FRHIGPUTextureReadback>(TEXT("FOdysseyTileManager::ReadBack")));
    }

    ENQUEUE_RENDER_COMMAND(FOdysseyTileManager_CreateTiles)(
        [
            Source = InTexture,
            TileSize = InTileSize,
            PixelFormat = InTileFormat,
            TilePositions,
            TileOldBuffers,
            TileReadBacks,
            SourceRect,
            DestinationRect
        ](FRHICommandListImmediate& RHICmdList)
        {
            SCOPE_CYCLE_COUNTER(STAT_CreateTiles);
            DECLARE_GPU_STAT(FOdysseyTileManager_CreateTiles);

            FRDGBuilder GraphBuilder(RHICmdList);

            FIntPoint TileWH(TileSize, TileSize);

            for (int i = 0; i < TilePositions.Num(); i++)
            {
                const FIntPoint& TilePosition = TilePositions[i];
                const FSharedBuffer& TileOldBuffer = TileOldBuffers[i];
                const TSharedPtr<FRHIGPUTextureReadback>& TileReadBack = TileReadBacks[i];

                FIntPoint TileDstPos(TilePosition.X * TileSize, TilePosition.Y * TileSize);
                FIntRect TileDstRect(TileDstPos, TileDstPos + TileWH);
                TileDstRect.Clip(DestinationRect);

                FIntPoint TileSrcPos(
                    SourceRect.Min.X + (TileDstRect.Min.X - DestinationRect.Min.X),
                    SourceRect.Min.Y + (TileDstRect.Min.Y - DestinationRect.Min.Y)
                );
                FIntRect TileSrcRect(
                    TileSrcPos,
                    TileSrcPos + TileDstRect.Size()
                );
                TileDstRect -= TileDstPos;

                FRDGTextureRef SourceTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(Source->GetResource()->TextureRHI, TEXT("FOdysseyTileManager::CreateTiles")));
                const FRDGTextureDesc& SourceTextureDesc = SourceTexture->Desc;

                FRDGTextureDesc TileTextureDesc = FRDGTextureDesc::Create2D(
                    FIntPoint(TileSize, TileSize),
                    PixelFormat,
                    FClearValueBinding::Transparent,
                    ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
                );

                FRDGTextureRef TileTexture = GraphBuilder.CreateTexture(TileTextureDesc, TEXT("FOdysseyTileManager::TileTexture"));

                if (TileOldBuffer.IsNull())
                {
                    AddClearRenderTargetPass(GraphBuilder, TileTexture, FLinearColor::Transparent);
                }
                else
                {
                    AddWriteTilePass(
                        GraphBuilder,
                        TileTexture,
                        TileOldBuffer
                    );
                }

                AddDrawTexturePass(
                    GraphBuilder,
                    FScreenPassViewInfo(),
                    SourceTexture,
                    TileTexture,
                    TileSrcRect.Min,
                    TileSrcRect.Size(),
                    TileDstRect.Min,
                    TileDstRect.Size()
                );

                AddEnqueueCopyPass(GraphBuilder, TileReadBack.Get(), TileTexture);
            }

            GraphBuilder.Execute();

            //Needed for the readback buffers to be read properly
            //Otherwise, ReadBack->Wait() will hang indefinitely
            RHICmdList.SubmitAndBlockUntilGPUIdle();
        }
    );

    TArray<FCreatedTile> CreatedTiles;
    for (int i = 0; i < TilePositions.Num(); i++)
    {
        FTileId TileId;
        TileId.Index = Tiles.Num();

        FTile Tile;
        Tile.GPUReadBack = TileReadBacks[i];
        Tiles.Add(MoveTemp(Tile));

        PendingTilesToReadBack.Add(TileId);

        FCreatedTile CreatedTile;
        CreatedTile.Id = TileId;
        CreatedTile.Pos = TilePositions[i];
        CreatedTiles.Add(CreatedTile);
    }

    return CreatedTiles;
}

void
FOdysseyTileManager::LoadTileFromReadBack(FTileId InTileId)
{
    FTile& Tile = Tiles[InTileId.Index];
    if (!Tile.GPUReadBack.IsValid())
        return;

    TPromise<FSharedBuffer> Promise;
    Tile.UncompressedBuffer = Promise.GetFuture();

    ENQUEUE_RENDER_COMMAND(FOdysseyTileManager_CreateTiles)(
        [
            this,
            Promise = MoveTemp(Promise),
            &Tile,
            InTileId
        ](FRHICommandListImmediate& RHICmdList) mutable
        {
            const FRHIGPUMask GPUMask = RHICmdList.GetGPUMask();

            // This blocks the render thread until the GPU has completed
            // the readback copy.
            Tile.GPUReadBack->Wait(RHICmdList, GPUMask);

            // At this point Lock() is safe.
            int32 Width = 0;
            int32 Height = 0;

            void* Data = Tile.GPUReadBack->Lock(Width, &Height);

            FSharedBuffer Buffer = FSharedBuffer::Clone(
                Data,
                Tile.GPUReadBack->GetGPUSizeBytes()
            );

            Tile.GPUReadBack->Unlock();
            Tile.GPUReadBack.Reset();

            Promise.SetValue(MoveTemp(Buffer));

            //Go to next pipeline step
            FScopeLock Lock(&PipelineMutex);
            PendingTilesToCompress.Add(InTileId);
        }
    );
}

void
FOdysseyTileManager::LoadTile(FTileId InTileId)
{
    FTile& Tile = Tiles[InTileId.Index];

    if (Tile.UncompressedBuffer.IsValid())
        return;

    if (Tile.GPUReadBack.IsValid())
    {
        LoadTileFromReadBack(InTileId);
        return;
    }

    if (!Tile.CompressedBuffer.IsNull())
    {
        TPromise<FSharedBuffer> Promise;
        Tile.UncompressedBuffer = Promise.GetFuture();
        FSharedBuffer Buffer = Tile.CompressedBuffer.Decompress();
        Promise.SetValue(Buffer);
        return;
    }
}

bool
FOdysseyTileManager::GetTileBuffer(FTileId InTileId, FSharedBuffer& OutBuffer) const
{
    if (InTileId.Index >= (uint32)Tiles.Num())
        return false;

    const FTile& Tile = Tiles[InTileId.Index];
    if (!Tile.UncompressedBuffer.IsValid())
    {
        //Loading a Tile is not const, but getting the Tile Buffer should be
        const_cast<FOdysseyTileManager*>(this)->LoadTile(InTileId);
    }

    OutBuffer = Tile.UncompressedBuffer.Get();

    return true;
}

void
FOdysseyTileManager::CompressTile(FTileId InTileId)
{
    FTile& Tile = Tiles[InTileId.Index];
    if (!Tile.UncompressedBuffer.IsValid())
        return;

    if (Tile.CompressedBuffer.IsNull())
        return;

    Tile.CompressedBuffer = FCompressedBuffer::Compress(Tile.UncompressedBuffer.Get());
}

void
FOdysseyTileManager::CacheTileOnDisk(FTileId InTileId)
{
    //TODO:
}

void
FOdysseyTileManager::EvictUncompressedBufferFromTile(FTileId InTileId)
{
    FTile& Tile = Tiles[InTileId.Index];
    Tile.UncompressedBuffer = TFuture<FSharedBuffer>();
}

void
FOdysseyTileManager::EvictCompressedBufferFromTile(FTileId InTileId)
{
    FTile& Tile = Tiles[InTileId.Index];
    Tile.CompressedBuffer = FCompressedBuffer();
}

void
FOdysseyTileManager::Tick(float DeltaTime)
{
    TArray<FTileId> LocalPendingTilesToReadBack;
    TArray<FTileId> LocalPendingTilesToCompress;
    TArray<FTileId> LocalPendingTilesToCacheOnDisk;
    TArray<FTileId> LocalTilesToEvictUncompressed;
    TArray<FTileId> LocalTilesToEvictCompressed;

    {
        FScopeLock Lock(&PipelineMutex);

        //Execute only Ready Reabacks
        LocalPendingTilesToReadBack = PendingTilesToReadBack.FilterByPredicate(
            [this](const FTileId& InTileId)
            {
                FTile& Tile = Tiles[InTileId.Index];
                TSharedPtr<FRHIGPUTextureReadback> ReadBack = Tile.GPUReadBack;
                return ReadBack.IsValid() && ReadBack->IsReady();
            }
        );

        LocalPendingTilesToCompress = PendingTilesToCompress;
        LocalPendingTilesToCacheOnDisk = PendingTilesToCacheOnDisk;
        LocalTilesToEvictUncompressed = TilesToEvictUncompressed;
        LocalTilesToEvictCompressed = TilesToEvictCompressed;


        //Keep Not Ready Readbacks for later
        PendingTilesToReadBack.RemoveAllSwap(
            [this](const FTileId& InTileId)
            {
                FTile& Tile = Tiles[InTileId.Index];
                TSharedPtr<FRHIGPUTextureReadback> ReadBack = Tile.GPUReadBack;
                return !ReadBack.IsValid() || !ReadBack->IsReady();
            }
        );
        PendingTilesToCompress.Empty();
        PendingTilesToCacheOnDisk.Empty();
        TilesToEvictUncompressed.Empty();
        TilesToEvictCompressed.Empty();
    }

    /**
     * Caching pipeline
     */
    for (const FTileId& TileId : LocalPendingTilesToReadBack)
    {
        LoadTileFromReadBack(TileId);
    }

    for (const FTileId& TileId : LocalPendingTilesToCompress)
    {
        Async(
            EAsyncExecution::ThreadPool,
            [this, TileId]()
            {
                CompressTile(TileId);

                FScopeLock Lock(&PipelineMutex);
                PendingTilesToCacheOnDisk.Add(TileId);
            }
        );
    }

    for (const FTileId& TileId : LocalPendingTilesToCacheOnDisk)
    {
        Async(
            EAsyncExecution::ThreadPool,
            [this, TileId]()
            {
                CacheTileOnDisk(TileId);
            }
        );
    }

    /**
     * Tile Eviction Pipeline
     */

    for (const FTileId& TileId : LocalTilesToEvictUncompressed)
    {
        EvictUncompressedBufferFromTile(TileId);
    }

    for (const FTileId& TileId : LocalTilesToEvictCompressed)
    {
        EvictCompressedBufferFromTile(TileId);
    }
}
