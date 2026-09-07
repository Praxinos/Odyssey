// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTileManager.h"

#include "Async/Async.h"
#include "Engine/Texture.h"
#include "HAL/FileManager.h"
#include "RenderingThread.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "Stats/Stats.h"
#include "TextureResource.h"

#include "OdysseyIsTextureEmptyShader.h"
#include "OdysseyTileManagerSettings.h"
#include "OdysseyTileUtils.h"

DECLARE_STATS_GROUP(TEXT("FOdysseyTileManager"), STATGROUP_OdysseyTileManager, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("FOdysseyTileManager::CreateTiles"), STAT_CreateTiles, STATGROUP_OdysseyTileManager);

/*static*/ FCriticalSection FOdysseyTileManager::FTileData::Mutex;
/*static*/ FCriticalSection FOdysseyTileManager::FTileData::CacheOnDiskMutex;

/*static*/ TDoubleLinkedList<TSharedPtr<FOdysseyTileManager::FTileData>> FOdysseyTileManager::FTileData::UncompressedLRU;
/*static*/ TDoubleLinkedList<TSharedPtr<FOdysseyTileManager::FTileData>> FOdysseyTileManager::FTileData::CompressedLRU;

FOdysseyTileManagerStats::FOdysseyTileManagerStats()
    : NumTilesUncompressed(0)
    , SizeUncompressed(0)
    , NumTilesCompressed(0)
    , SizeCompressed(0)
    , NumTilesOnDisk(0)
    , SizeOnDisk(0)
{

}

void
FOdysseyTileManagerStats::AddUncompressed(const FSharedBuffer& InBuffer)
{
    NumTilesUncompressed++;
    SizeUncompressed += InBuffer.GetSize();
}

void
FOdysseyTileManagerStats::AddCompressed(const FCompressedBuffer& InBuffer)
{
    NumTilesCompressed++;
    SizeCompressed += InBuffer.GetCompressedSize();
}

void
FOdysseyTileManagerStats::AddCachedOnDisk(int64 InSizeOnDisk)
{
    NumTilesOnDisk++;
    SizeOnDisk += InSizeOnDisk;
}

void
FOdysseyTileManagerStats::RemoveUncompressed(const FSharedBuffer& InBuffer)
{
    NumTilesUncompressed--;
    SizeUncompressed -= InBuffer.GetSize();
}

void
FOdysseyTileManagerStats::RemoveCompressed(const FCompressedBuffer& InBuffer)
{
    NumTilesCompressed--;
    SizeCompressed -= InBuffer.GetCompressedSize();
}

void
FOdysseyTileManagerStats::RemoveCachedOnDisk(int64 InSizeOnDisk)
{
    NumTilesOnDisk--;
    SizeOnDisk -= InSizeOnDisk;
}

int64
FOdysseyTileManagerStats::GetNumTilesUncompressed() const
{
    return NumTilesUncompressed;
}

int64
FOdysseyTileManagerStats::GetSizeUncompressed() const
{
    return SizeUncompressed;
}

int64
FOdysseyTileManagerStats::GetNumTilesCompressed() const
{
    return NumTilesCompressed;
}

int64
FOdysseyTileManagerStats::GetSizeCompressed() const
{
    return SizeCompressed;
}

int64
FOdysseyTileManagerStats::GetNumTilesOnDisk() const
{
    return NumTilesOnDisk;
}

int64
FOdysseyTileManagerStats::GetSizeOnDisk() const
{
    return SizeOnDisk;
}

//static
FOdysseyTileManager&
FOdysseyTileManager::Get()
{
    static FOdysseyTileManager self;
    return self;
}

//static
FString
FOdysseyTileManager::GetCacheOnDiskPath()
{
    return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Temp"), TEXT("OdysseyTileManager"), TEXT("TempCache.dat"));
}

void
FOdysseyTileManager::Initialize()
{
    static bool IsInitialized = false;
    if (IsInitialized)
        return;

    IsInitialized = true;
    FString CacheDirectory = FPaths::GetPath(GetCacheOnDiskPath());
    IFileManager::Get().DeleteDirectory(*CacheDirectory, false, true);
}

void
FOdysseyTileManager::Finalize()
{
    FString CacheDirectory = FPaths::GetPath(GetCacheOnDiskPath());

    WaitUntilAllTilesAreCached(false);

    IFileManager::Get().DeleteDirectory(*CacheDirectory, false, true);
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
    TArray<TSharedPtr<FTile>> NewTiles;
    TArray<TFuture<FTextureRHIRef>> TileOldTextures;
    NewTiles.Reserve(TilePositions.Num());
    TileOldTextures.Reserve(TilePositions.Num());

    TArray<FOdysseyTileManager::FCreatedTile> CreatedTiles;

    for (const FIntPoint& TilePosition : TilePositions)
    {
        //Try to retrieve the previous content of the tile we are updating
        FOdysseyTileId OldTileId;
        if (InGetExistingTileId.IsBound())
            OldTileId = InGetExistingTileId.Execute(TilePosition);

        TFuture<FTextureRHIRef> OldTileTexture;
        if (OldTileId.IsValid())
        {
            OldTileTexture = GetTileTexture(OldTileId, InTileSize, InTileFormat);
        }
        else
        {
            TPromise<FTextureRHIRef> Promise;
            OldTileTexture = Promise.GetFuture();
            Promise.SetValue(FTextureRHIRef());
        }
        TileOldTextures.Add(MoveTemp(OldTileTexture));

        //Create a new tile representing the new version of the updated tile
        TSharedPtr<FTile> Tile = CreateNewTile();
        NewTiles.Add(Tile);
        PendingTilesToReadBack.Add(Tile);

        FCreatedTile CreatedTile;
        CreatedTile.Id = Tile->Id;
        CreatedTile.Pos = TilePosition;
        CreatedTiles.Add(CreatedTile);
    }

    ENQUEUE_RENDER_COMMAND(FOdysseyTileManager_CreateTiles)(
        [
            Source = InTexture,
            TileSize = InTileSize,
            PixelFormat = InTileFormat,
            NewTiles,
            TilePositions,
            TileOldTextures = MoveTemp(TileOldTextures),
            SourceRect,
            DestinationRect
        ](FRHICommandListImmediate& RHICmdList)
        {
            SCOPE_CYCLE_COUNTER(STAT_CreateTiles);
            DECLARE_GPU_STAT(FOdysseyTileManager_CreateTiles);

            FIntPoint TileWH(TileSize, TileSize);

            //Step1 : Create the Tile Textures
            TArray<FTextureRHIRef> TileTextureRHIs;
            for (int i = 0; i < TilePositions.Num(); i++)
            {
                const FRHITextureCreateDesc CreateDesc = FRHITextureCreateDesc::Create2D(TEXT("FOdysseyTileManager::TileTexture"))
                    .SetExtent(TileWH)
                    .SetFormat(PixelFormat)
                    .SetClearValue(FClearValueBinding::Transparent)
                    .SetFlags(ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable);

                TileTextureRHIs.Add(RHICmdList.CreateTexture(CreateDesc));
            }

            //Step2 :
            // - Draw in the Tile Textures
            // - Initiate "IsEmpty" ReadBack
            // - Initiate "Pixels" ReadBack
            FRDGBuilder GraphBuilder(RHICmdList);
            for (int i = 0; i < TilePositions.Num(); i++)
            {
                const TSharedPtr<FTile>& NewTile = NewTiles[i];
                const FIntPoint& TilePosition = TilePositions[i];
                FTextureRHIRef TileOldTextureRHI;
                FTextureRHIRef TileTextureRHI = TileTextureRHIs[i];

                if (TileOldTextures[i].IsValid())
                    TileOldTextureRHI = TileOldTextures[i].Get();

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
                FRDGTextureRef TileTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(TileTextureRHI, TEXT("FOdysseyTileManager::TileTexture")));

                if (TileOldTextureRHI.IsValid())
                {
                    FRDGTextureRef TileOldTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(TileOldTextureRHI, TEXT("FOdysseyTileManager::TileOldTexture")));
                    AddDrawTexturePass(
                        GraphBuilder,
                        FScreenPassViewInfo(),
                        TileOldTexture,
                        TileTexture
                    );
                }
                else
                {
                    AddClearRenderTargetPass(GraphBuilder, TileTexture, FLinearColor::Transparent);
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
                    NewTile->GPUIsEmptyReadBack
                );

                AddEnqueueCopyPass(GraphBuilder, NewTile->GPUReadBack.Get(), TileTexture);
            }
            GraphBuilder.Execute();

            //Step 3 : Fullfil the TileTexture Promises
            for (int i = 0; i < TileTextureRHIs.Num(); i++)
            {
                //We only validate the TileTexture once the GraphBuilder has been executed
                //It ensures the TileTexture will be filled with the right pixels data
                //even if the texture is immediately used
                const TSharedPtr<FTile>& NewTile = NewTiles[i];
                NewTile->TempTexturePromise.SetValue(TileTextureRHIs[i]);
            }

            //Needed for the readback buffers to be read properly
            //Otherwise, ReadBack->Wait() can hang indefinitely
            RHICmdList.ImmediateFlush(EImmediateFlushType::FlushRHIThread);
        }
    );

    return CreatedTiles;
}

FOdysseyTileId
FOdysseyTileManager::CreateTile(const FIoHash& InHash, const FCompressedBuffer& InCompressedBuffer)
{
    check(IsInGameThread());
    //Compare Buffer to other buffers
    TArray<TSharedPtr<FTileData>> TilesData;
    TSharedPtr<FTileData> TileData;

    {
        FScopeLock Lock(&HashToTileDataMutex);
        HashToTileData.MultiFind(InHash, TilesData, false);
        if (TilesData.IsEmpty())
        {
            TileData = FTileData::FromCompressedBuffer(InHash, InCompressedBuffer);
            HashToTileData.Add(InHash, TileData);
        }
        else
        {
            bool bFound = false;
            FSharedBuffer Buffer = InCompressedBuffer.Decompress();
            for (TSharedPtr<FTileData> ExistingTileData : TilesData)
            {
                FSharedBuffer TileDataBuffer;
                if(!ExistingTileData->GetUncompressedBuffer(TileDataBuffer))
                    continue;

                if (TileDataBuffer.GetView().EqualBytes(Buffer.GetView()))
                {
                    bFound = true;
                    TileData = ExistingTileData;
                    break;
                }
            }

            //Create a new TileData
            if (!bFound)
            {
                TileData = FTileData::FromBuffers(InHash, Buffer, InCompressedBuffer);
                HashToTileData.Add(InHash, TileData);
            }
        }
    }

    TSharedPtr<FTile> Tile = CreateNewTile(TileData);
    return Tile->Id;
}

template<typename... ArgsType>
TSharedPtr<FOdysseyTileManager::FTile>
FOdysseyTileManager::CreateNewTile(ArgsType&&... Args)
{
    TSharedPtr<FTile> Tile;
    {
        FScopeLock Lock(&FreeTilesMutex);
        FOdysseyTileId TileId;
        if (FreeTiles.Num() > 0)
        {
            //Retrieve the first FreeTile
            TileId = FreeTiles[0];
            FreeTiles.RemoveAtSwap(0);
        }
        else
        {
            //Create a brand new Tile
            TileId.Index = Tiles.Num();
            TileId.Generation = 0;
            Tiles.AddDefaulted();
        }
        Tile = MakeShared<FTile>(TileId);
        Tiles[TileId.Index] = Tile;
        Tile->Initialize(Forward<ArgsType>(Args)...);
    }

    return Tile;
}

void
FOdysseyTileManager::TryLoadTileFromReadBack(TSharedPtr<FTile> InTile)
{
    if (!InTile->GPUReadBack.IsValid() ||
        !InTile->GPUIsEmptyReadBack.IsValid())
        return;

    if (!InTile->GPUReadBack->IsReady() ||
        !InTile->GPUIsEmptyReadBack->IsReady())
        return;

    LoadTileFromReadBack(InTile);

}

void
FOdysseyTileManager::LoadTileFromReadBack(TSharedPtr<FTile> InTile)
{
    PendingTilesToReadBack.RemoveSwap(InTile);

    ENQUEUE_RENDER_COMMAND(FOdysseyTileManager_CreateTiles)(
        [
            this, //To access HashToTileData and Mutexes
            InTile
        ](FRHICommandListImmediate& RHICmdList) mutable
        {
            const FRHIGPUMask GPUMask = RHICmdList.GetGPUMask();

            //Is Empty ReadBack
            {
                // This blocks the render thread until the GPU has completed
                // the readback copy.
                InTile->GPUIsEmptyReadBack->Wait(RHICmdList, GPUMask);
                uint32* Data = (uint32*)InTile->GPUIsEmptyReadBack->Lock(4);
                bool IsEmpty = Data[0] != 0;
                InTile->GPUIsEmptyReadBack->Unlock();

                if (IsEmpty) //Is Empty
                {
                    //We have 2 locks here
                    //1 to Modify FreeTiles
                    //1 to the Tile itself
                    FScopeLock Lock(&FreeTilesMutex);
                    FScopeLock TileLock(&InTile->Mutex);

                    InTile->TempTexture.Reset();
                    InTile->TileDataPromise.SetValue(nullptr);

                    FOdysseyTileId TileId = InTile->Id;
                    TileId.Generation++;
                    FreeTiles.Add(TileId);
                    return;
                }
            }

            //Tile Data ReadBack
            {

                // This blocks the render thread until the GPU has completed
                // the readback copy.
                InTile->GPUReadBack->Wait(RHICmdList, GPUMask);

                // At this point Lock() is safe.
                int32 Width = 0;
                int32 Height = 0;

                void* Data = InTile->GPUReadBack->Lock(Width, &Height);

                FSharedBuffer Buffer = FSharedBuffer::Clone(
                    Data,
                    InTile->GPUReadBack->GetGPUSizeBytes()
                );

                InTile->GPUReadBack->Unlock();

                //Compare Buffer to other buffers
                FIoHash Hash = FIoHashBuilder::HashBuffer(Buffer.GetView());
                TArray<TSharedPtr<FTileData>> TilesData;

                {
                    FScopeLock HashToTileDataLock(&HashToTileDataMutex);
                    HashToTileData.MultiFind(Hash, TilesData, false);

                    for (TSharedPtr<FTileData> TileData : TilesData)
                    {
                        FSharedBuffer TileDataBuffer;
                        if(!TileData->GetUncompressedBuffer(TileDataBuffer))
                            continue;

                        if (TileDataBuffer.GetView().EqualBytes(Buffer.GetView()))
                        {
                            FScopeLock TileLock(&InTile->Mutex);
                            InTile->TempTexture.Reset();
                            InTile->TileDataPromise.SetValue(TileData);
                            return;
                        }
                    }

                    //Create a new TileData
                    TSharedPtr<FTileData> TileData = FTileData::FromUncompressedBuffer(Hash, Buffer);
                    HashToTileData.Add(Hash, TileData);

                    {
                        FScopeLock TileLock(&InTile->Mutex);
                        InTile->TempTexture.Reset();
                        InTile->TileDataPromise.SetValue(TileData);
                    }
                }
            }
        }
    );
}

TFuture<FTextureRHIRef>
FOdysseyTileManager::GetTileTexture(FOdysseyTileId InTileId, uint32 InTileSize, EPixelFormat InTileFormat) const
{
    check(IsInGameThread());

    TPromise<FTextureRHIRef> Promise;
    TFuture<FTextureRHIRef> Future = Promise.GetFuture();

    //Check if TileIndex is out of bounds
    if (InTileId.Index >= (uint64)Tiles.Num())
    {
        Promise.SetValue(FTextureRHIRef());
        return Future;
    }

    //Retrieve Tile
    TSharedPtr<FTile> Tile = Tiles[InTileId.Index];

    //We read the tile here
    //We lock its mutex to ensure it is not modified by another thread
    FScopeLock TileLock(&Tile->Mutex);

    //If Tile.Id does not correspond to TileId
    //it indicates the tile corresponding to the TileId was empty
    //and freed.
    if (Tile->Id != InTileId)
    {
        Promise.SetValue(FTextureRHIRef());
        return Future;
    }

    //Check if the Tile has a TempTexture
    //Indicating the Tile's ReadBack has not yet finished
    //Rely on TempTexture if it exists
    if (Tile->TempTexture.IsValid())
    {
        FTextureRHIRef TextureRHI = Tile->TempTexture.Get();
        if (TextureRHI.IsValid())
        {
            Promise.SetValue(TextureRHI);
            return Future;
        }
    }

    //We rely directly on FTileData which is guaranteed to be valid here
    //Because TempTexture is Null or Invalid, indicating ReadBack has finished
    //
    //We don't rely on FTile anymore
    //So we can release the lock here
    TileLock.Unlock();

    //TempTexture is Null indicating ReadBack has finished
    //The Tile's Buffer should be available
    //Rely on the Tile's Buffer to create a TextureRHIRef
    TSharedPtr<FTileData> TileData = Tile->TileData.Get();
    if (!TileData)
    {
        Promise.SetValue(FTextureRHIRef());
        return Future;
    }

    FSharedBuffer Buffer;
    if (!TileData->GetUncompressedBuffer(Buffer))
    {
        //Tile's Buffer could not be retrieved
        //indicating an empty Tile
        Promise.SetValue(FTextureRHIRef());
        return Future;
    }

    ENQUEUE_RENDER_COMMAND(FOdysseyTileManager_GetTileTexture)(
        [
            Buffer,
            PixelFormat = InTileFormat,
            TileSize = InTileSize,
            Promise = MoveTemp(Promise)
        ](FRHICommandListImmediate& RHICmdList) mutable
        {
            FIntPoint TileWH(TileSize, TileSize);

            const FRHITextureCreateDesc CreateDesc = FRHITextureCreateDesc::Create2D(TEXT("FOdysseyTileManager::TileTexture"))
                .SetExtent(TileWH)
                .SetFormat(PixelFormat)
                .SetClearValue(FClearValueBinding::Transparent)
                .SetFlags(ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable);

            FTextureRHIRef TextureRHI = RHICmdList.CreateTexture(CreateDesc);

            FRDGBuilder GraphBuilder(RHICmdList);

            FRDGTextureRef TileTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(TextureRHI, TEXT("FOdysseyTileManager::TileTexture")));

            AddWriteTilePass(
                GraphBuilder,
                TileTexture,
                Buffer
            );

            GraphBuilder.Execute();

            Promise.SetValue(TextureRHI);
        }
    );

    return Future;
}

bool
FOdysseyTileManager::GetTileCompressedBuffer(FOdysseyTileId InTileId, FCompressedBuffer& OutBuffer) const
{
    return const_cast<FOdysseyTileManager*>(this)->GetTileCompressedBuffer(InTileId, OutBuffer);
}

bool
FOdysseyTileManager::GetTileCompressedBuffer(FOdysseyTileId InTileId, FCompressedBuffer& OutBuffer)
{
    if (InTileId.Index >= (uint64)Tiles.Num())
        return false;

    TSharedPtr<FTile> Tile = Tiles[InTileId.Index];

    //We read the tile here
    //We lock its mutex to ensure it is not modified by another thread
    FScopeLock TileLock(&Tile->Mutex);

    //If Tile.Id does not correspond to TileId
    //it indicates the tile corresponding to the TileId was empty
    //and freed.
    if (Tile->Id != InTileId)
        return false;

    //If readback is still valid, we need to start reading back
    if (Tile->GPUReadBack.IsValid())
        LoadTileFromReadBack(Tile);

    //We don't rely on FTile anymore
    //So we can release the lock here
    TileLock.Unlock();

    //Tile->TileData is a future, so it will block current thread until GPU Readback is done
    TSharedPtr<FTileData> TileData = Tile->TileData.Get();
    if (!TileData)
        return false;

    return TileData->GetCompressedBuffer(OutBuffer);
}

bool
FOdysseyTileManager::GetTileHash(FOdysseyTileId InTileId, FIoHash& OutHash)
{
    if (InTileId.Index >= (uint64)Tiles.Num())
        return false;

    TSharedPtr<FTile> Tile = Tiles[InTileId.Index];

    //We read the tile here
    //We lock its mutex to ensure it is not modified by another thread
    FScopeLock TileLock(&Tile->Mutex);

    //If Tile.Id does not correspond to TileId
    //it indicates the tile corresponding to the TileId was empty
    //and freed.
    if (Tile->Id != InTileId)
        return false;

    //If readback is still valid, we need to start reading back
    if (Tile->GPUReadBack.IsValid())
        LoadTileFromReadBack(Tile);

    //We don't rely on FTile anymore
    //So we can release the lock here
    TileLock.Unlock();

    //Tile->TileData is a future, so it will block current thread until GPU Readback is done
    TSharedPtr<FTileData> TileData = Tile->TileData.Get();
    if (!TileData)
        return false;

    OutHash = TileData->GetHash();
    return true;
}

FOdysseyTileManagerStats&
FOdysseyTileManager::GetStats()
{
    return Stats;
}

void
FOdysseyTileManager::WaitUntilAllTilesAreCached(bool InEvictTiles)
{
    for (TSharedPtr<FTile>& Tile : Tiles)
    {
        //Calling Tile.TileData.Get() waits until GPU Readback is finished
        TSharedPtr<FTileData> TileData = Tile->TileData.Get();
        if (!TileData)
            continue;

        //Then wait until the caching process is finished
        TileData->WaitUntilCachedOnDisk();
    }

    if (InEvictTiles)
        EvictTiles();
}

void
FOdysseyTileManager::Tick(float DeltaTime)
{
    //PendingTilesToReadBack is modified by LoadTileFromReadBack()
    //So we copy into a local variable to avoid iterating over a changing Array
    TArray<TSharedPtr<FTile>> LocalPendingTilesToReadBack = PendingTilesToReadBack;
    for (TSharedPtr<FTile> Tile : LocalPendingTilesToReadBack)
    {
        TryLoadTileFromReadBack(Tile);
    }

    EvictTiles();
}

void
FOdysseyTileManager::EvictTiles()
{
    uint64 MaxUncompressedSize = UOdysseyTileManagerSettings::Get()->MaxUncompressedSize * 1000 * 1000;
    uint64 MaxCompressedSize = UOdysseyTileManagerSettings::Get()->MaxCompressedSize * 1000 * 1000;
    FTileData::EvictTiles(MaxUncompressedSize, MaxCompressedSize);
}

void
FOdysseyTileManager::EvictAllTiles()
{
    FTileData::EvictTiles(0, 0);
}

/*************************************
 * FTile
 *************************************/

FOdysseyTileManager::FTile::FTile(const FOdysseyTileId& InId)
    : Id(InId)
{
}

void
FOdysseyTileManager::FTile::Initialize()
{
    TempTexture = TempTexturePromise.GetFuture();
    TileData = TileDataPromise.GetFuture();

    GPUReadBack = MakeShared<FRHIGPUTextureReadback>(TEXT("FOdysseyTileManager::FTile::GPUReadBack"));
    GPUIsEmptyReadBack = MakeShared<FRHIGPUBufferReadback>(TEXT("FOdysseyTileManager::FTile::GPUIsEmptyReadBack"));
}

void
FOdysseyTileManager::FTile::Initialize(TSharedPtr<FTileData> InTileData)
{
    TileData = TileDataPromise.GetFuture();
    TileDataPromise.SetValue(InTileData);
}

TSharedRef<FOdysseyTileManager::FTileData>
FOdysseyTileManager::FTileData::FromUncompressedBuffer(const FIoHash& InHash, const FSharedBuffer& InUncompressedBuffer)
{
    TSharedRef<FTileData> TileData = MakeShared<FTileData>();
    TileData->Hash = InHash;
    TileData->UncompressedBuffer = InUncompressedBuffer;

    FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
    TileStats.AddUncompressed(TileData->UncompressedBuffer);
    TPromise<FCompressedBuffer> Promise;
    TileData->CompressedBuffer = Promise.GetFuture();

    //Go to next pipeline step
    TileData->CacheCompressedCompletionEvent = Async(
        EAsyncExecution::ThreadPool,
        [TileData, Promise = MoveTemp(Promise)]() mutable
        {
            FCompressedBuffer Buffer = TileData->Compress();

            FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
            TileStats.AddCompressed(Buffer);

            Promise.SetValue(Buffer);
        }
    );

    {
        FScopeLock Lock(&Mutex);
        Touch(TileData);
    }

    return TileData;
}

TSharedRef<FOdysseyTileManager::FTileData>
FOdysseyTileManager::FTileData::FromCompressedBuffer(const FIoHash& InHash, const FCompressedBuffer& InCompressedBuffer)
{
    TSharedRef<FTileData> TileData = MakeShared<FTileData>();
    TileData->Hash = InHash;

    TPromise<FCompressedBuffer> Promise;
    TileData->CompressedBuffer = Promise.GetFuture();
    Promise.SetValue(InCompressedBuffer);

    FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
    TileStats.AddCompressed(TileData->CompressedBuffer.Get());

    TPromise<void> CacheCompressedPromise;
    TileData->CacheCompressedCompletionEvent = CacheCompressedPromise.GetFuture();
    CacheCompressedPromise.SetValue();

    TileData->CacheOnDiskCompletionEvent = Async(
        EAsyncExecution::ThreadPool,
        [TileData]()
        {
            TileData->CacheOnDisk();
        }
    );

    {
        FScopeLock Lock(&Mutex);
        Touch(TileData);
    }

    return TileData;
}

TSharedRef<FOdysseyTileManager::FTileData>
FOdysseyTileManager::FTileData::FromBuffers(const FIoHash& InHash, const FSharedBuffer& InUncompressedBuffer, const FCompressedBuffer& InCompressedBuffer)
{
    TSharedRef<FTileData> TileData = MakeShared<FTileData>();
    TileData->Hash = InHash;
    TileData->UncompressedBuffer = InUncompressedBuffer;

    TPromise<FCompressedBuffer> Promise;
    TileData->CompressedBuffer = Promise.GetFuture();
    Promise.SetValue(InCompressedBuffer);

    FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
    TileStats.AddUncompressed(TileData->UncompressedBuffer);
    TileStats.AddCompressed(TileData->CompressedBuffer.Get());

    TPromise<void> CacheCompressedPromise;
    TileData->CacheCompressedCompletionEvent = CacheCompressedPromise.GetFuture();
    CacheCompressedPromise.SetValue();

    TileData->CacheOnDiskCompletionEvent = Async(
        EAsyncExecution::ThreadPool,
        [TileData]()
        {
            TileData->CacheOnDisk();
        }
    );

    {
        FScopeLock Lock(&Mutex);
        Touch(TileData);
    }

    return TileData;
}

bool
FOdysseyTileManager::FTileData::GetUncompressedBuffer(FSharedBuffer& OutBuffer) const
{
    FScopeLock Lock(&Mutex);
    if (!LoadUncompressedBuffer())
        return false;
    OutBuffer = UncompressedBuffer;

    Touch(ConstCastSharedRef<FOdysseyTileManager::FTileData>(AsShared()));

    return true;
}

bool
FOdysseyTileManager::FTileData::GetCompressedBuffer(FCompressedBuffer& OutBuffer) const
{
    FScopeLock Lock(&Mutex);
    if (!LoadCompressedBuffer())
        return false;
    OutBuffer = CompressedBuffer.Get();

    Touch(ConstCastSharedRef<FOdysseyTileManager::FTileData>(AsShared()));
    return true;
}

const FIoHash&
FOdysseyTileManager::FTileData::GetHash() const
{
    return Hash;
}

bool
FOdysseyTileManager::FTileData::LoadUncompressedBuffer() const
{
    //If UncompressedBuffer Is Null, it means UncompressedBuffer has been evicted and needs to be loaded from CompressedBuffer
    if (UncompressedBuffer.IsNull())
    {
        if (!LoadCompressedBuffer())
            return false;

        UncompressedBuffer = CompressedBuffer.Get().Decompress();

        if (!UncompressedBuffer.IsNull())
        {
            FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
            TileStats.AddUncompressed(UncompressedBuffer);
        }
    }

    return true;
}

bool
FOdysseyTileManager::FTileData::LoadCompressedBuffer() const
{
    FCompressedBuffer Buffer = CompressedBuffer.Get();

    //If Buffer Is Null, it means CompressedBuffer has been evicted and needs to be loaded from disk cache
    if (Buffer.IsNull())
    {
        FScopeLock Lock(&CacheOnDiskMutex);

        FString FilePath = GetCacheOnDiskPath();
        const TUniquePtr<FArchive> Ar(IFileManager::Get().CreateFileReader(*FilePath, FILEWRITE_Append));
        if (!Ar)
            return false;

        Ar->Seek(DiskCacheOffset);
        *Ar << Buffer;

        if (!Buffer.IsNull())
        {
            FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
            TileStats.AddCompressed(Buffer);
        }
        Ar->Close();

        TPromise<FCompressedBuffer> Promise;
        CompressedBuffer = Promise.GetFuture();
        Promise.SetValue(Buffer);
    }
    return true;
}

FCompressedBuffer
FOdysseyTileManager::FTileData::Compress()
{
    FCompressedBuffer Buffer = FCompressedBuffer::Compress(UncompressedBuffer);

    CacheOnDiskCompletionEvent = Async(
        EAsyncExecution::ThreadPool,
        [self = AsShared()]()
        {
            self->CacheOnDisk();
        }
    );

    return Buffer;
}

void
FOdysseyTileManager::FTileData::CacheOnDisk()
{
    FString FilePath = FOdysseyTileManager::GetCacheOnDiskPath();

    //We lock to prevent several threads to write in the file at the same time
    {
        FScopeLock Lock(&CacheOnDiskMutex);

        const TUniquePtr<FArchive> Ar(IFileManager::Get().CreateFileWriter(*FilePath, FILEWRITE_Append));
        if (!Ar)
            return;

        DiskCacheOffset = Ar->Tell();

        FCompressedBuffer Buffer = CompressedBuffer.Get();
        *Ar << Buffer;
        Ar->Flush();

        FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
        TileStats.AddCachedOnDisk(Ar->TotalSize() - DiskCacheOffset);

        Ar->Close();
    }

    {
        FScopeLock Lock(&CacheStateMutex);
        CacheState = ECacheState::Cached;
    }
}

void
FOdysseyTileManager::FTileData::EvictUncompressed()
{
    //Eviction is not permitted if Caching is not yet finished
    {
        FScopeLock Lock(&CacheStateMutex);
        if (CacheState != ECacheState::Cached)
            return;
    }

    TDoubleLinkedList<TSharedPtr<FTileData>>::TDoubleLinkedListNode* Node = UncompressedLRUNode;
    if (!Node)
        return;

    FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
    TileStats.RemoveUncompressed(UncompressedBuffer);

    UncompressedBuffer = FSharedBuffer();
    UncompressedLRUNode = nullptr;
    UncompressedLRU.RemoveNode(Node);
}

void
FOdysseyTileManager::FTileData::EvictCompressed()
{
    //Eviction is not permitted if Caching is not yet finished
    {
        FScopeLock Lock(&CacheStateMutex);
        if (CacheState != ECacheState::Cached)
            return;
    }

    TDoubleLinkedList<TSharedPtr<FTileData>>::TDoubleLinkedListNode* Node = CompressedLRUNode;
    if (!Node)
        return;

    FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
    TileStats.RemoveCompressed(CompressedBuffer.Get());

    TPromise<FCompressedBuffer> Promise;
    CompressedBuffer = Promise.GetFuture();
    Promise.SetValue(FCompressedBuffer());

    CompressedLRUNode = nullptr;
    CompressedLRU.RemoveNode(Node);
}

void
FOdysseyTileManager::FTileData::Touch(TSharedRef<FOdysseyTileManager::FTileData> InTileData)
{
    if (InTileData->UncompressedLRUNode)
    {
        UncompressedLRU.RemoveNode(InTileData->UncompressedLRUNode);
    }

    if (InTileData->CompressedLRUNode)
    {
        CompressedLRU.RemoveNode(InTileData->CompressedLRUNode);
    }

    UncompressedLRU.AddHead(InTileData);
    CompressedLRU.AddHead(InTileData);

    InTileData->UncompressedLRUNode = UncompressedLRU.GetHead();
    InTileData->CompressedLRUNode = CompressedLRU.GetHead();
}

void
FOdysseyTileManager::FTileData::EvictTiles(uint64 InMaxUncompressedSize, uint64 InMaxCompressedSize)
{
    FScopeLock Lock(&Mutex);

    FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();

    TDoubleLinkedList<TSharedPtr<FTileData>>::TDoubleLinkedListNode* NodeUncompressed = UncompressedLRU.GetTail();
    while (NodeUncompressed && (uint64)TileStats.GetSizeUncompressed() > InMaxUncompressedSize)
    {
        TDoubleLinkedList<TSharedPtr<FTileData>>::TDoubleLinkedListNode* Node = NodeUncompressed;
        if (!Node)
            return;

        NodeUncompressed = Node->GetPrevNode();

        TSharedPtr<FTileData> TileData = Node->GetValue();
        TileData->EvictUncompressed();
    }

    TDoubleLinkedList<TSharedPtr<FTileData>>::TDoubleLinkedListNode* NodeCompressed = CompressedLRU.GetTail();
    while (NodeCompressed && (uint64)TileStats.GetSizeCompressed() > InMaxCompressedSize)
    {
        TDoubleLinkedList<TSharedPtr<FTileData>>::TDoubleLinkedListNode* Node = NodeCompressed;
        if (!Node)
            return;

        NodeCompressed = Node->GetPrevNode();

        TSharedPtr<FTileData> TileData = Node->GetValue();
        TileData->EvictCompressed();
    }
}

void
FOdysseyTileManager::FTileData::WaitUntilCachedCompressed()
{
    CacheCompressedCompletionEvent.Wait();
}

void
FOdysseyTileManager::FTileData::WaitUntilCachedOnDisk()
{
    //We wait on CacheCompressedCompletionEvent first because
    //CacheOnDiskCompletionEvent's Future is only valid once
    //CacheCompressedCompletionEvent's Value has been set
    CacheCompressedCompletionEvent.Wait();
    CacheOnDiskCompletionEvent.Wait();
}
