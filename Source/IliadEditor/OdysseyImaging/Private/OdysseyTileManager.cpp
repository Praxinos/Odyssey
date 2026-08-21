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
        FTileId OldTileId;
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

    FScopeLock Lock(&FreeTilesMutex);
    TArray<FCreatedTile> CreatedTiles;
    for (int i = 0; i < TilePositions.Num(); i++)
    {
        FTile Tile;
        Tile.GPUReadBack = TileReadBacks[i];
        Tile.GPUIsEmptyReadBack = TileIsEmptyReadBacks[i];

        FTileId TileId;
        if (FreeTiles.Num() > 0)
        {
            TileId = FreeTiles[0];
            FreeTiles.RemoveAtSwap(0);
            Tiles[TileId.Index] = MoveTemp(Tile);
        }
        else
        {
            TileId.Index = Tiles.Num();
            TileId.Generation = 0;
            Tiles.Add(MoveTemp(Tile));
        }

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
    PendingTilesToReadBack.RemoveSwap(InTileId);

    FTile& Tile = Tiles[InTileId.Index];

    TPromise<FSharedBuffer> Promise;
    Tile.UncompressedBuffer = Promise.GetFuture();

    TSharedPtr<FRHIGPUTextureReadback> GPUReadBack = Tile.GPUReadBack;
    TSharedPtr<FRHIGPUBufferReadback> GPUIsEmptyReadBack = Tile.GPUIsEmptyReadBack;

    Tile.GPUReadBack.Reset();
    Tile.GPUIsEmptyReadBack.Reset();

    ENQUEUE_RENDER_COMMAND(FOdysseyTileManager_CreateTiles)(
        [
            this,
            Promise = MoveTemp(Promise),
            GPUReadBack,
            GPUIsEmptyReadBack,
            InTileId
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
                    FTileId FreeTileId = InTileId;
                    FreeTileId.Generation++;
                    //Add TileId to FreeTiles
                    {
                        FScopeLock Lock(&FreeTilesMutex);
                        FreeTiles.Add(FreeTileId);
                    }

                    Promise.SetValue(FSharedBuffer());
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

                Promise.SetValue(MoveTemp(Buffer));

                //Go to next pipeline step
                Async(
                    EAsyncExecution::ThreadPool,
                    [this, InTileId]()
                    {
                        CompressTile(InTileId);
                    }
                );
            }
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
        StatNumTilesUncompressed++;
        StatSizeUncompressed += Buffer.GetSize();

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

    if (!Tile.CompressedBuffer.IsNull())
        return;

    Tile.CompressedBuffer = FCompressedBuffer::Compress(Tile.UncompressedBuffer.Get());
    StatNumTilesCompressed++;
    StatSizeCompressed += Tile.CompressedBuffer.GetCompressedSize();

    Async(
        EAsyncExecution::ThreadPool,
        [this, InTileId]()
        {
            CacheTileOnDisk(InTileId);
        }
    );
}

void
FOdysseyTileManager::CacheTileOnDisk(FTileId InTileId)
{
    //TODO: Cache On Disk
    //TODO: Update Stat
}

void
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
}

void
FOdysseyTileManager::Tick(float DeltaTime)
{

    //Execute only Ready Reabacks
    TArray<FTileId> TilesReadyToReadBack = PendingTilesToReadBack.FilterByPredicate(
        [this](const FTileId& InTileId)
        {
            FTile& Tile = Tiles[InTileId.Index];
            TSharedPtr<FRHIGPUTextureReadback> ReadBack = Tile.GPUReadBack;
            TSharedPtr<FRHIGPUBufferReadback> IsEmptyReadBack = Tile.GPUIsEmptyReadBack;
            return ReadBack.IsValid() && ReadBack->IsReady() && IsEmptyReadBack.IsValid() && IsEmptyReadBack->IsReady();
        }
    );

    /**
     * Caching pipeline
     */
    for (const FTileId& TileId : TilesReadyToReadBack)
    {
        LoadTileFromReadBack(TileId);
    }

    /**
     * Tile Eviction Pipeline
     */

    for (const FTileId& TileId : TilesToEvictUncompressed)
    {
        EvictUncompressedBufferFromTile(TileId);
    }

    for (const FTileId& TileId : TilesToEvictCompressed)
    {
        EvictCompressedBufferFromTile(TileId);
    }

    TilesToEvictUncompressed.Empty();
    TilesToEvictCompressed.Empty();
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
