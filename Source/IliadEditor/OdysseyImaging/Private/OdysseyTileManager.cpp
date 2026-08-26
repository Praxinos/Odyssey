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

#include "OdysseyIsTextureEmptyShader.h"
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
    TArray<TSharedPtr<FRHIGPUBufferReadback>> TileIsEmptyReadBacks;
    TileOldBuffers.Reserve(TilePositions.Num());
    TileReadBacks.Reserve(TilePositions.Num());
    TileIsEmptyReadBacks.Reserve(TilePositions.Num());
    for (const FIntPoint& TilePosition : TilePositions)
    {
        FOdysseyTileId OldTileId;
        if (InGetExistingTileId.IsBound())
            OldTileId = InGetExistingTileId.Execute(TilePosition);

        FSharedBuffer OldTileBuffer;
        if (OldTileId.IsValid())
            GetTileBuffer(OldTileId, OldTileBuffer);

        TileOldBuffers.Add(OldTileBuffer);
        TileReadBacks.Add(MakeShared<FRHIGPUTextureReadback>(TEXT("FOdysseyTileManager::ReadBack")));
        TileIsEmptyReadBacks.Add(MakeShared<FRHIGPUBufferReadback>(TEXT("FOdysseyTileManager::IsEmptyReadBack")));
    }

    ENQUEUE_RENDER_COMMAND(FOdysseyTileManager_CreateTiles)(
        [
            Source = InTexture,
            TileSize = InTileSize,
            PixelFormat = InTileFormat,
            TilePositions,
            TileOldBuffers,
            TileReadBacks,
            TileIsEmptyReadBacks,
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
                const TSharedPtr<FRHIGPUBufferReadback>& TileIsEmptyReadBack = TileIsEmptyReadBacks[i];

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

                Odyssey::Shaders::AddIsTextureEmptyPass(
                    GraphBuilder,
                    GMaxRHIFeatureLevel,
                    TileTexture,
                    TileIsEmptyReadBack
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
        FTile Tile;
        Tile.GPUReadBack = TileReadBacks[i];
        Tile.GPUIsEmptyReadBack = TileIsEmptyReadBacks[i];

        FOdysseyTileId TileId;
        if (FreeTiles.Num() > 0)
        {
            TileId.Index = FreeTiles[0];
            FreeTiles.RemoveAtSwap(0);
            Tiles[TileId.Index] = MoveTemp(Tile);
        }
        else
        {
            TileId.Index = Tiles.Num();
            Tiles.Add(MoveTemp(Tile));
        }

        PendingTilesToReadBack.Add(TileId.Index);

        FCreatedTile CreatedTile;
        CreatedTile.Id = TileId;
        CreatedTile.Pos = TilePositions[i];
        CreatedTiles.Add(CreatedTile);
    }

    return CreatedTiles;
}

void
FOdysseyTileManager::LoadTileFromReadBack(uint64 InTileIndex)
{
    PendingTilesToReadBack.RemoveSwap(InTileIndex);

    FTile& Tile = Tiles[InTileIndex];

    TPromise<uint64> Promise;
    Tile.TileDataIndex = Promise.GetFuture();

    TSharedPtr<FRHIGPUTextureReadback> GPUReadBack = Tile.GPUReadBack;
    TSharedPtr<FRHIGPUBufferReadback> GPUIsEmptyReadBack = Tile.GPUIsEmptyReadBack;

    Tile.GPUReadBack.Reset();
    Tile.GPUIsEmptyReadBack.Reset();

    ENQUEUE_RENDER_COMMAND(FOdysseyTileManager_CreateTiles)(
        [
            this,
            Promise = MoveTemp(Promise),
            GPUReadBack,
            GPUIsEmptyReadBack
        ](FRHICommandListImmediate& RHICmdList) mutable
        {
            const FRHIGPUMask GPUMask = RHICmdList.GetGPUMask();

            //Is Empty ReadBack
            {
                // This blocks the render thread until the GPU has completed
                // the readback copy.
                GPUIsEmptyReadBack->Wait(RHICmdList, GPUMask);
                uint32* Data = (uint32*)GPUIsEmptyReadBack->Lock(4);
                bool IsEmpty = Data[0] != 0;
                GPUIsEmptyReadBack->Unlock();

                if (IsEmpty) //Is Empty
                {
                    Promise.SetValue(INDEX_NONE);
                    return;
                }
            }

            //Tile Data ReadBack
            {

                // This blocks the render thread until the GPU has completed
                // the readback copy.
                GPUReadBack->Wait(RHICmdList, GPUMask);

                // At this point Lock() is safe.
                int32 Width = 0;
                int32 Height = 0;

                void* Data = GPUReadBack->Lock(Width, &Height);

                FSharedBuffer Buffer = FSharedBuffer::Clone(
                    Data,
                    GPUReadBack->GetGPUSizeBytes()
                );

                GPUReadBack->Unlock();

                StatNumTilesUncompressed++;
                StatSizeUncompressed += Buffer.GetSize();

                uint64 TileDataIndex = INDEX_NONE;
                {
                    FScopeLock Lock(&TilesDataMutex);
                    TileDataIndex = TilesData.Num();
                    FTileData TileData;
                    TileData.UncompressedBuffer = Buffer;
                    TilesData.Add(TileData);
                }
                Promise.SetValue(TileDataIndex);

                //Go to next pipeline step
                Async(
                    EAsyncExecution::ThreadPool,
                    [this, TileDataIndex]()
                    {
                        CompressTile(TileDataIndex);
                    }
                );
            }
        }
    );
}

bool
FOdysseyTileManager::GetTileBuffer(FOdysseyTileId InTileId, FSharedBuffer& OutBuffer) const
{
    return const_cast<FOdysseyTileManager*>(this)->GetTileBuffer(InTileId, OutBuffer);
}

bool
FOdysseyTileManager::GetTileBuffer(FOdysseyTileId InTileId, FSharedBuffer& OutBuffer)
{
    if (InTileId.Index >= (uint64)Tiles.Num())
        return false;

    const FTile& Tile = Tiles[InTileId.Index];

    //If readback is still valid, we need to start reading back
    if (Tile.GPUReadBack.IsValid())
        LoadTileFromReadBack(InTileId.Index);

    //Here we get the tile data index
    //Tile.TileDataIndex is a future, so it will block current thread until GPU Readback is done
    uint64 TileDataIndex = Tile.TileDataIndex.Get();
    if (TileDataIndex == INDEX_NONE)
        return false;

    //TODO: Get a copy of the TileData (use mutex)
    FTileData TileData;
    {
        FScopeLock Lock(&TilesDataMutex);
        TileData = TilesData[TileDataIndex];
    }

    //If needed, load all the tile buffers
    bool UncompressedBufferChanged = false;
    bool CompressedBufferChanged = false;
    if (TileData.UncompressedBuffer.IsNull())
    {
        if (TileData.CompressedBuffer.IsNull())
        {
            //TODO: Load from Disk Cache
            CompressedBufferChanged = true;
        }

        TileData.UncompressedBuffer = TileData.CompressedBuffer.Decompress();
        StatNumTilesUncompressed++;
        StatSizeUncompressed += TileData.UncompressedBuffer.GetSize();
        UncompressedBufferChanged = true;
    }

    //If any buffer has been loaded, set the tile data
    if (UncompressedBufferChanged || CompressedBufferChanged)
    {
        FScopeLock Lock(&TilesDataMutex);
        if (UncompressedBufferChanged)
            TilesData[TileDataIndex].UncompressedBuffer = TileData.UncompressedBuffer;

        if (CompressedBufferChanged)
            TilesData[TileDataIndex].CompressedBuffer = TileData.CompressedBuffer;
    }

    OutBuffer = TileData.UncompressedBuffer;

    return true;
}

void
FOdysseyTileManager::CompressTile(uint64 InTileDataIndex)
{
    FSharedBuffer Buffer;
    {
        FScopeLock Lock(&TilesDataMutex);
        Buffer = TilesData[InTileDataIndex].UncompressedBuffer;
    }

    FCompressedBuffer CompressedBuffer = FCompressedBuffer::Compress(Buffer);
    StatNumTilesCompressed++;
    StatSizeCompressed += CompressedBuffer.GetCompressedSize();

    {
        FScopeLock Lock(&TilesDataMutex);
        TilesData[InTileDataIndex].CompressedBuffer = CompressedBuffer;
    }

    Async(
        EAsyncExecution::ThreadPool,
        [this, InTileDataIndex]()
        {
            CacheTileOnDisk(InTileDataIndex);
        }
    );
}

void
FOdysseyTileManager::CacheTileOnDisk(uint64 InTileDataIndex)
{
    FCompressedBuffer CompressedBuffer;
    {
        FScopeLock Lock(&TilesDataMutex);
        CompressedBuffer = TilesData[InTileDataIndex].CompressedBuffer;
    }

    //TODO: Cache On Disk
    //TODO: Update Stat

    {
        FScopeLock Lock(&TilesDataMutex);

        //TODO: Update Disk Cache Data in TilesData[InTileDataIndex]
    }
}

/* void
FOdysseyTileManager::EvictUncompressedBufferFromTile(FTileId InTileId)
{
    FTile& Tile = Tiles[InTileId.Index];

    StatNumTilesUncompressed--;
    StatSizeUncompressed -= Tile.UncompressedBuffer.Get().GetSize();

    Tile.UncompressedBuffer = TFuture<FSharedBuffer>();
}

void
FOdysseyTileManager::EvictCompressedBufferFromTile(FTileId InTileId)
{
    FTile& Tile = Tiles[InTileId.Index];

    StatNumTilesCompressed--;
    StatSizeCompressed -= Tile.CompressedBuffer.GetCompressedSize();

    Tile.CompressedBuffer = FCompressedBuffer();
} */

void
FOdysseyTileManager::Tick(float DeltaTime)
{

    //Execute only Ready Reabacks
    TArray<uint64> TilesReadyToReadBack = PendingTilesToReadBack.FilterByPredicate(
        [this](uint64 InTileIndex)
        {
            FTile& Tile = Tiles[InTileIndex];
            TSharedPtr<FRHIGPUTextureReadback> ReadBack = Tile.GPUReadBack;
            TSharedPtr<FRHIGPUBufferReadback> IsEmptyReadBack = Tile.GPUIsEmptyReadBack;
            return ReadBack.IsValid() && ReadBack->IsReady() && IsEmptyReadBack.IsValid() && IsEmptyReadBack->IsReady();
        }
    );

    /**
     * Caching pipeline
     */
    for (uint64 TileIndex : TilesReadyToReadBack)
    {
        LoadTileFromReadBack(TileIndex);
    }

    /**
     * Tile Eviction Pipeline
     */

    /* for (const FTileId& TileId : TilesToEvictUncompressed)
    {
        EvictUncompressedBufferFromTile(TileId);
    }

    for (const FTileId& TileId : TilesToEvictCompressed)
    {
        EvictCompressedBufferFromTile(TileId);
    }

    TilesToEvictUncompressed.Empty();
    TilesToEvictCompressed.Empty(); */
}

int64
FOdysseyTileManager::GetNumTilesUncompressed() const
{
    return StatNumTilesUncompressed;
}

int64
FOdysseyTileManager::GetSizeUncompressed() const
{
    return StatSizeUncompressed;
}

int64
FOdysseyTileManager::GetNumTilesCompressed() const
{
    return StatNumTilesCompressed;
}

int64
FOdysseyTileManager::GetSizeCompressed() const
{
    return StatSizeCompressed;
}
int64
FOdysseyTileManager::GetNumTilesOnDisk() const
{
    return StatNumTilesOnDisk;
}

int64
FOdysseyTileManager::GetSizeOnDisk() const
{
    return StatSizeOnDisk;
}
