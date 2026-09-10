// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTileManager.h"

#include "Async/Async.h"
#include "Engine/Texture.h"
#include "GlobalShader.h"
#include "HAL/FileManager.h"
#include "RenderingThread.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "Stats/Stats.h"
#include "TextureResource.h"

#include "OdysseyAreTextureTilesEmptyShader.h"
#include "OdysseyTileManagerSettings.h"
#include "OdysseyTileUtils.h"

/**
 * A 64x64 Tiles Atlas of 64x64 RGBA8 pixels tiles
 * Is ~64MB on GPU
 */
#define TILE_ATLAS_WIDTH_IN_TILES 64
#define TILE_ATLAS_HEIGHT_IN_TILES 64
#define TILE_ATLAS_NUM_TILES (TILE_ATLAS_WIDTH_IN_TILES * TILE_ATLAS_HEIGHT_IN_TILES)

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

    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::Initialize);

    IsInitialized = true;
    FString CacheDirectory = FPaths::GetPath(GetCacheOnDiskPath());
    IFileManager::Get().DeleteDirectory(*CacheDirectory, false, true);
}

void
FOdysseyTileManager::Finalize()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::Finalize);
    FString CacheDirectory = FPaths::GetPath(GetCacheOnDiskPath());

    WaitUntilAllTilesAreCached(false);

    IFileManager::Get().DeleteDirectory(*CacheDirectory, false, true);
}

BEGIN_SHADER_PARAMETER_STRUCT(FWriteTileParameters, )
    RDG_TEXTURE_ACCESS(Texture, ERHIAccess::CopyDest)
END_SHADER_PARAMETER_STRUCT()

//static
void
FOdysseyTileManager::AddWriteTilePass(FRDGBuilder& GraphBuilder, FRDGTextureRef OutAtlasTexture, FIntPoint TilePositionInAtlas, uint32 InTileSize, FSharedBuffer InBuffer)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::AddWriteTilePass);

    FWriteTileParameters* Params = GraphBuilder.AllocParameters<FWriteTileParameters>();
    Params->Texture = OutAtlasTexture;

    GraphBuilder.AddPass(
        RDG_EVENT_NAME("UOdysseyTiledImage::AddWriteTilePass"),
        Params,
        ERDGPassFlags::Copy | ERDGPassFlags::NeverCull,
        [OutAtlasTexture, InBuffer, TilePositionInAtlas, InTileSize](FRHICommandList& RHICmdList)
        {
            const FRDGTextureDesc& Desc = OutAtlasTexture->Desc;
            const FPixelFormatInfo& PixelFormatInfo = GPixelFormats[Desc.Format];
            uint32 BytesPerPixel = PixelFormatInfo.BlockBytes;
            uint32 BufferStride = BytesPerPixel * InTileSize;

            FUpdateTextureRegion2D Region(TilePositionInAtlas.X, TilePositionInAtlas.Y, 0, 0, InTileSize, InTileSize);
            RHICmdList.UpdateTexture2D(OutAtlasTexture->GetRHI(), 0, Region, BufferStride, (const uint8*)InBuffer.GetData());
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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::CreateOrUpdateTiles);

    FIntRect SourceRect(0, 0, InTexture->GetSurfaceWidth(), InTexture->GetSurfaceHeight());
    SourceRect.Clip(InRect);
    FIntRect DestinationRect(
        InPosition + (SourceRect.Min - InRect.Min),
        InPosition + SourceRect.Size()
    );

    struct FTileToCreate
    {
        FIntPoint Position;
        TSharedPtr<FTile> Tile;
        FRDGTextureRef RDGTexture;
        TSharedPtr<FTileTextureHandle> TextureHandle;
        TFuture<TSharedPtr<FTileTextureHandle>> OldTextureHandle;
    };

    TArray<FIntPoint> TilePositions = Odyssey::TileUtils::GetTilePositionsFromRect(InTileSize, DestinationRect);
    TArray<FTileToCreate> TilesToCreate;

    TilesToCreate.Reserve(TilesToCreate.Num());

    TArray<FOdysseyTileManager::FCreatedTile> CreatedTiles;

    for (const FIntPoint& TilePosition : TilePositions)
    {
        //Try to retrieve the previous content of the tile we are updating
        FOdysseyTileId OldTileId;
        if (InGetExistingTileId.IsBound())
            OldTileId = InGetExistingTileId.Execute(TilePosition);

        TFuture<TSharedPtr<FTileTextureHandle>> OldTileTextureHandle;
        if (OldTileId.IsValid())
        {
            OldTileTextureHandle = GetTileTexture(OldTileId, InTileSize, InTileFormat);
        }
        else
        {
            TPromise<TSharedPtr<FTileTextureHandle>> Promise;
            OldTileTextureHandle = Promise.GetFuture();
            Promise.SetValue(TSharedPtr<FTileTextureHandle>());
        }

        //Create a new tile representing the new version of the updated tile
        TSharedPtr<FTile> Tile = CreateNewTile();

        FTileToCreate TileToCreate;
        TileToCreate.Position = TilePosition;
        TileToCreate.Tile = Tile;
        TileToCreate.OldTextureHandle = MoveTemp(OldTileTextureHandle);
        TilesToCreate.Add(MoveTemp(TileToCreate));

        PendingTilesToReadBack.Add(Tile);

        FCreatedTile CreatedTile;
        CreatedTile.Id = Tile->Id;
        CreatedTile.Pos = TilePosition;
        CreatedTiles.Add(CreatedTile);
    }

    ENQUEUE_RENDER_COMMAND(FOdysseyTileManager_CreateTiles)(
        [
            this,
            Source = InTexture,
            TileSize = InTileSize,
            PixelFormat = InTileFormat,
            SourceRect,
            DestinationRect,
            TilesToCreate = MoveTemp(TilesToCreate)
        ](FRHICommandListImmediate& RHICmdList) mutable
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::CreateOrUpdateTiles_0);

            //Step1 :
            // - Create the Tile Textures
            // - Draw in the Tile Textures
            FRDGBuilder GraphBuilder(RHICmdList);

            /**
             * We need an intermediate Texture to draw our tile in
             * before drawing it into the Tile's Atlas.
             * Because an OldTileTexture can share the same Atlas Texture as the new TileTexture
             * And calling AddDrawTexturePass() with InputTexture == OutputTexture
             * returns silently.
             */
            FIntPoint TileWH(TileSize, TileSize);
            FRDGTextureDesc TileIntermediateTextureDesc = FRDGTextureDesc::Create2D(
                TileWH,
                PixelFormat,
                FClearValueBinding::Transparent,
                ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
            );

            FRDGTextureRef TileIntermediateTexture = GraphBuilder.CreateTexture(TileIntermediateTextureDesc, TEXT("FOdysseyTileManager::TileIntermediateTextureDesc"));

            const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
            TMap<FRDGTextureRef, TArray<int>> ReadBackParams;
            for (int i = 0; i < TilesToCreate.Num(); i++)
            {
                FTileToCreate& TileToCreate = TilesToCreate[i];
                //Make sure we get the OldTileTexture now
                TSharedPtr<FTileTextureHandle> OldTileTextureHandle;
                if (TileToCreate.OldTextureHandle.IsValid())
                    OldTileTextureHandle = TileToCreate.OldTextureHandle.Get();

                //Create the TileTexture
                TileToCreate.TextureHandle = ReserveTileTexture(RHICmdList, TileSize, PixelFormat);

                //Ensure the Rects are precisely the one we need
                //to prevent bleeding on neighbour Tiles in the Tile's Texture Atlas
                FIntPoint TileDstPos = TileToCreate.Position * TileSize;
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
                TileToCreate.RDGTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(TileToCreate.TextureHandle->GetTextureRHI(), TEXT("FOdysseyTileManager::TileTexture")));

                //Prepare ReadBack params
                ReadBackParams.FindOrAdd(TileToCreate.RDGTexture).Add(i);

                //Draw in the Tile Texture
                if (OldTileTextureHandle.IsValid())
                {
                    //First we draw the Old pixels in the tile
                    //This is needed, as the new pixels can occupy a partial amount of the tile
                    FRDGTextureRef TileOldTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(OldTileTextureHandle->GetTextureRHI(), TEXT("FOdysseyTileManager::TileOldTexture")));

                    FRDGDrawTextureInfo DrawInfo;
                    DrawInfo.Size = TileWH;
                    DrawInfo.SourcePosition = OldTileTextureHandle->GetPositionInTexture();
                    AddDrawTexturePass(GraphBuilder, ShaderMap, TileOldTexture, TileIntermediateTexture, DrawInfo);
                }
                else
                {
                    //First we clear the Tile
                    AddClearRenderTargetPass(GraphBuilder, TileIntermediateTexture);
                }

                //Then we draw the new pixels in the Tile
                FRDGDrawTextureInfo DrawSourceInTileInfo;
                DrawSourceInTileInfo.Size = TileSrcRect.Size();
                DrawSourceInTileInfo.SourcePosition = TileSrcRect.Min;
                DrawSourceInTileInfo.DestPosition = TileDstRect.Min;// + TileToCreate.TextureHandle->GetPositionInTexture();
                AddDrawTexturePass(GraphBuilder, ShaderMap, SourceTexture, TileIntermediateTexture, DrawSourceInTileInfo);

                //Finally, we draw the tile into the Tile Atlas
                FRDGDrawTextureInfo DrawTileInTextureAtlasInfo;
                DrawTileInTextureAtlasInfo.Size = TileWH;
                DrawTileInTextureAtlasInfo.DestPosition = TileToCreate.TextureHandle->GetPositionInTexture();
                AddDrawTexturePass(GraphBuilder, ShaderMap, TileIntermediateTexture, TileToCreate.RDGTexture, DrawTileInTextureAtlasInfo);
            }

            //Step2 : Initiate ReadBacks
            for (auto& Element : ReadBackParams)
            {
                FRDGTextureRef Texture = Element.Key;
                TArray<int> Indexes = Element.Value;

                TArray<FIntPoint> TilePositions;
                for(int Index : Indexes)
                {
                    TilePositions.Add(TilesToCreate[Index].TextureHandle->GetPositionInTexture());
                }

                TSharedRef<FRHIGPUBufferReadback> GPUIsEmptyReadBack = MakeShared<FRHIGPUBufferReadback>(TEXT("FOdysseyTileManager::FTile::GPUIsEmptyReadBack"));
                Odyssey::Shaders::AddAreTextureTilesEmptyPass(
                    GraphBuilder,
                    GMaxRHIFeatureLevel,
                    Texture,
                    TilePositions,
                    TileSize,
                    GPUIsEmptyReadBack
                );

                for(int i = 0; i < Indexes.Num(); i++)
                {
                    int Index = Indexes[i];

                    FTile::FIsEmptyReadBack IsEmptyReadBack;
                    IsEmptyReadBack.ReadBack = GPUIsEmptyReadBack;
                    IsEmptyReadBack.ResultSize = Indexes.Num() * sizeof(uint32);
                    IsEmptyReadBack.ResultIndex = i;

                    TilesToCreate[Index].Tile->GPUIsEmptyReadBackPromise->SetValue(IsEmptyReadBack);

                    AddEnqueueCopyPass(
                        GraphBuilder,
                        TilesToCreate[Index].Tile->GPUReadBack.Get(),
                        Texture,
                        FResolveRect(TilesToCreate[Index].TextureHandle->GetRectInTexture())
                    );
                }
            }

            GraphBuilder.Execute();

            //Step 3 : Fullfil the TileTexture Promises
            for (FTileToCreate& TileToCreate : TilesToCreate)
            {
                //We only validate the TileTexture once the GraphBuilder has been executed
                //It ensures the TileTexture will be filled with the right pixels data
                //even if the texture is immediately used
                TileToCreate.Tile->TempTexturePromise->SetValue(TileToCreate.TextureHandle);
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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::CreateTile);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::CreateNewTile);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::TryLoadTileFromReadBack);

    if (InTile->GPUReadBackState != FTile::EGPUReadBackState::Idle)
        return;

    if (!InTile->GPUReadBack.IsValid() || !InTile->GPUReadBack->IsReady())
        return;

    if (!InTile->GPUIsEmptyReadBack.IsValid())
        return;

    const FTile::FIsEmptyReadBack& IsEmptyReadBack = InTile->GPUIsEmptyReadBack.Get();
    if (!IsEmptyReadBack.ReadBack.IsValid() || !IsEmptyReadBack.ReadBack->IsReady())
        return;

    LoadTileFromReadBack(InTile);
}

void
FOdysseyTileManager::LoadTileFromReadBack(TSharedPtr<FTile> InTile)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::LoadTileFromReadBack);

    PendingTilesToReadBack.RemoveSwap(InTile);

    if (InTile->GPUReadBackState != FTile::EGPUReadBackState::Idle)
        return;

    InTile->GPUReadBackState = FTile::EGPUReadBackState::InProgress;

    ENQUEUE_RENDER_COMMAND(FOdysseyTileManager_LoadTileFromReadBack)(
        [
            this, //To access HashToTileData and Mutexes
            InTile
        ](FRHICommandListImmediate& RHICmdList) mutable
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::LoadTileFromReadBack_0);

            const FRHIGPUMask GPUMask = RHICmdList.GetGPUMask();

            //Is Empty ReadBack
            {
                // This blocks the render thread until the GPU has completed
                // the readback copy.
                const FTile::FIsEmptyReadBack& IsEmptyReadBack = InTile->GPUIsEmptyReadBack.Get();
                IsEmptyReadBack.ReadBack->Wait(RHICmdList, GPUMask);
                uint32* Data = static_cast<uint32*>(IsEmptyReadBack.ReadBack->Lock(IsEmptyReadBack.ResultSize));
                bool IsEmpty = Data[IsEmptyReadBack.ResultIndex] != 0;

                IsEmptyReadBack.ReadBack->Unlock();

                if (IsEmpty) //Is Empty
                {
                    //We have 2 locks here
                    //1 to Modify FreeTiles
                    //1 to the Tile itself
                    FScopeLock Lock(&FreeTilesMutex);
                    FScopeLock TileLock(&InTile->Mutex);

                    InTile->TempTexture.Reset();
                    InTile->TempTexturePromise.Reset();
                    InTile->TileDataPromise.SetValue(nullptr);
                    InTile->GPUIsEmptyReadBack.Reset();
                    InTile->GPUIsEmptyReadBackPromise.Reset();
                    InTile->GPUReadBack = nullptr;
                    InTile->GPUReadBackState = FTile::EGPUReadBackState::Done;

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
                            InTile->TempTexturePromise.Reset();
                            InTile->TileDataPromise.SetValue(TileData);
                            InTile->GPUIsEmptyReadBack.Reset();
                            InTile->GPUIsEmptyReadBackPromise.Reset();
                            InTile->GPUReadBack = nullptr;
                            InTile->GPUReadBackState = FTile::EGPUReadBackState::Done;
                            return;
                        }
                    }

                    //Create a new TileData
                    TSharedPtr<FTileData> TileData = FTileData::FromUncompressedBuffer(Hash, Buffer);
                    HashToTileData.Add(Hash, TileData);

                    {
                        FScopeLock TileLock(&InTile->Mutex);
                        InTile->TempTexture.Reset();
                        InTile->TempTexturePromise.Reset();
                        InTile->TileDataPromise.SetValue(TileData);
                        InTile->GPUIsEmptyReadBack.Reset();
                        InTile->GPUIsEmptyReadBackPromise.Reset();
                        InTile->GPUReadBack = nullptr;
                        InTile->GPUReadBackState = FTile::EGPUReadBackState::Done;
                    }
                }
            }
        }
    );
}

TFuture<TSharedPtr<FOdysseyTileManager::FTileTextureHandle>>
FOdysseyTileManager::GetTileTexture(FOdysseyTileId InTileId, uint32 InTileSize, EPixelFormat InTileFormat) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::GetTileTexture);

    check(IsInGameThread());

    TPromise<TSharedPtr<FTileTextureHandle>> Promise;
    TFuture<TSharedPtr<FTileTextureHandle>> Future = Promise.GetFuture();

    //Check if TileIndex is out of bounds
    if (InTileId.Index >= (uint64)Tiles.Num())
    {
        Promise.SetValue(TSharedPtr<FTileTextureHandle>());
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
        Promise.SetValue(TSharedPtr<FTileTextureHandle>());
        return Future;
    }

    //Check if the Tile has a TempTexture
    //Indicating the Tile's ReadBack has not yet finished
    //Rely on TempTexture if it exists
    if (Tile->TempTexture.IsValid())
    {
        TSharedPtr<FTileTextureHandle> TileTextureHandle = Tile->TempTexture.Get();
        if (TileTextureHandle.IsValid())
        {
            Promise.SetValue(TileTextureHandle);
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
        Promise.SetValue(TSharedPtr<FTileTextureHandle>());
        return Future;
    }

    FSharedBuffer Buffer;
    if (!TileData->GetUncompressedBuffer(Buffer))
    {
        //Tile's Buffer could not be retrieved
        //indicating an empty Tile
        Promise.SetValue(TSharedPtr<FTileTextureHandle>());
        return Future;
    }

    ENQUEUE_RENDER_COMMAND(FOdysseyTileManager_GetTileTexture)(
        [
            self = const_cast<FOdysseyTileManager*>(this),
            Buffer,
            PixelFormat = InTileFormat,
            TileSize = InTileSize,
            Promise = MoveTemp(Promise)
        ](FRHICommandListImmediate& RHICmdList) mutable
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::GetTileTexture_0);

            /* FIntPoint TileWH(TileSize, TileSize);

            const FRHITextureCreateDesc CreateDesc = FRHITextureCreateDesc::Create2D(TEXT("FOdysseyTileManager::TileTexture"))
                .SetExtent(TileWH)
                .SetFormat(PixelFormat)
                .SetClearValue(FClearValueBinding::Transparent)
                .SetFlags(ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable);

            FTextureRHIRef TextureRHI = RHICmdList.CreateTexture(CreateDesc); */

            TSharedPtr<FTileTextureHandle> TileTextureHandle = self->ReserveTileTexture(RHICmdList, TileSize, PixelFormat);

            FRDGBuilder GraphBuilder(RHICmdList);

            FRDGTextureRef TileTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(TileTextureHandle->GetTextureRHI(), TEXT("FOdysseyTileManager::TileTexture")));

            AddWriteTilePass(
                GraphBuilder,
                TileTexture,
                TileTextureHandle->GetPositionInTexture(),
                TileSize,
                Buffer
            );

            GraphBuilder.Execute();

            Promise.SetValue(TileTextureHandle);
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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::GetTileCompressedBuffer);

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
    if (Tile->GPUReadBackState == FTile::EGPUReadBackState::Idle)
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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::GetTileHash);

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
    if (Tile->GPUReadBackState == FTile::EGPUReadBackState::Idle)
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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::WaitUntilAllTilesAreCached);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::Tick);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::EvictTiles);

    uint64 MaxUncompressedSize = UOdysseyTileManagerSettings::Get()->MaxUncompressedSize * 1000 * 1000;
    uint64 MaxCompressedSize = UOdysseyTileManagerSettings::Get()->MaxCompressedSize * 1000 * 1000;
    FTileData::EvictTiles(MaxUncompressedSize, MaxCompressedSize);
}

void
FOdysseyTileManager::EvictAllTiles()
{
    FTileData::EvictTiles(0, 0);
}

TSharedPtr<FOdysseyTileManager::FTileTextureHandle>
FOdysseyTileManager::ReserveTileTexture(FRHICommandList& InRHICmdList, uint32 InTileSize, EPixelFormat InTileFormat)
{
    check(IsInRenderingThread());

    FScopeLock Lock(&TileAtlasesMutex);

    TSharedPtr<FTileAtlas> Atlas;
    for(TSharedRef<FTileAtlas> TileAtlas : TileAtlases)
    {
        if (TileAtlas->TileSize != InTileSize || TileAtlas->TileFormat != InTileFormat)
            continue;

        if (TileAtlas->FreeTileIndexes.IsEmpty())
            continue;

        Atlas = TileAtlas;
        break;
    }

    if (!Atlas)
    {
        Atlas = MakeShared<FTileAtlas>(InRHICmdList, InTileSize, InTileFormat);
        TileAtlases.Add(Atlas.ToSharedRef());
    }

    TSharedPtr<FTileTextureHandle> TileTexture = MakeShared<FTileTextureHandle>(Atlas, Atlas->FreeTileIndexes[0]);
    Atlas->FreeTileIndexes.RemoveAtSwap(0);

    return TileTexture;
}

void
FOdysseyTileManager::ReleaseTileTexture(TSharedRef<FOdysseyTileManager::FTileAtlas> InAtlas, uint32 TileIndexInAtlas)
{
    FScopeLock Lock(&TileAtlasesMutex);
    InAtlas->FreeTileIndexes.Add(TileIndexInAtlas);
    if (InAtlas->FreeTileIndexes.Num() == TILE_ATLAS_NUM_TILES)
        TileAtlases.Remove(InAtlas);
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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTile::Initialize);
    TempTexturePromise = MakeUnique<TPromise<TSharedPtr<FTileTextureHandle>>>();
    TempTexture = TempTexturePromise->GetFuture();
    TileData = TileDataPromise.GetFuture();

    GPUReadBack = MakeShared<FRHIGPUTextureReadback>(TEXT("FOdysseyTileManager::FTile::GPUReadBack"));
    GPUIsEmptyReadBackPromise = MakeUnique<TPromise<FIsEmptyReadBack>>();
    GPUIsEmptyReadBack = GPUIsEmptyReadBackPromise->GetFuture();
    GPUReadBackState = EGPUReadBackState::Idle;
}

void
FOdysseyTileManager::FTile::Initialize(TSharedPtr<FTileData> InTileData)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTile::InitializeWithTileData);
    TileData = TileDataPromise.GetFuture();
    TileDataPromise.SetValue(InTileData);

    GPUReadBackState = EGPUReadBackState::Done;
}

TSharedRef<FOdysseyTileManager::FTileData>
FOdysseyTileManager::FTileData::FromUncompressedBuffer(const FIoHash& InHash, const FSharedBuffer& InUncompressedBuffer)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::FromUncompressedBuffer);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::FromCompressedBuffer);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::FromBuffers);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::GetUncompressedBuffer);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::GetCompressedBuffer);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::LoadUncompressedBuffer);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::LoadCompressedBuffer);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::Compress);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::CacheOnDisk);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::EvictUncompressed);
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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::EvictCompressed);
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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::Touch);
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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::EvictTiles);

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
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::WaitUntilCachedCompressed);
    CacheCompressedCompletionEvent.Wait();
}

void
FOdysseyTileManager::FTileData::WaitUntilCachedOnDisk()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::FTileData::WaitUntilCachedOnDisk);
    //We wait on CacheCompressedCompletionEvent first because
    //CacheOnDiskCompletionEvent's Future is only valid once
    //CacheCompressedCompletionEvent's Value has been set
    CacheCompressedCompletionEvent.Wait();
    CacheOnDiskCompletionEvent.Wait();
}

FOdysseyTileManager::FTileAtlas::FTileAtlas(FRHICommandList& InRHICmdList, uint32 InTileSize, EPixelFormat InTileFormat)
    : TileSize(InTileSize)
    , TileFormat(InTileFormat)
{
    FIntPoint AtlasSize(
        InTileSize * TILE_ATLAS_WIDTH_IN_TILES,
        InTileSize * TILE_ATLAS_HEIGHT_IN_TILES
    );

    const FRHITextureCreateDesc CreateDesc = FRHITextureCreateDesc::Create2D(TEXT("FOdysseyTileManager::FTileAtlas::Texture"))
        .SetExtent(AtlasSize)
        .SetFormat(InTileFormat)
        .SetClearValue(FClearValueBinding::Transparent)
        .SetFlags(ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable);

    Texture = InRHICmdList.CreateTexture(CreateDesc);

    FreeTileIndexes.Reserve(TILE_ATLAS_NUM_TILES);

    for (uint32 i = 0; i < TILE_ATLAS_NUM_TILES; i++)
    {
        FreeTileIndexes.Add(i);
    }
}

FOdysseyTileManager::FTileTextureHandle::~FTileTextureHandle()
{
    if (Atlas)
        FOdysseyTileManager::Get().ReleaseTileTexture(Atlas.ToSharedRef(), TileIndexInAtlas);
}

FOdysseyTileManager::FTileTextureHandle::FTileTextureHandle()
    : Atlas()
    , TileIndexInAtlas(INDEX_NONE)
{
}

FOdysseyTileManager::FTileTextureHandle::FTileTextureHandle(TSharedPtr<FTileAtlas> InAtlas, uint32 InTileIndexInAtlas)
    : Atlas(InAtlas)
    , TileIndexInAtlas(InTileIndexInAtlas)
{
}

FTextureRHIRef
FOdysseyTileManager::FTileTextureHandle::GetTextureRHI() const
{
    if (!Atlas)
        return FTextureRHIRef();

    return Atlas->Texture;
}

FIntRect
FOdysseyTileManager::FTileTextureHandle::GetRectInTexture() const
{
    if (!Atlas)
        return FIntRect();

    FIntPoint Position = GetPositionInTexture();
    return FIntRect(
        Position,
        Position + FIntPoint(Atlas->TileSize, Atlas->TileSize)
    );
}

FIntPoint
FOdysseyTileManager::FTileTextureHandle::GetPositionInTexture() const
{
    if (!Atlas)
        return FIntPoint();

    return FIntPoint(
        Atlas->TileSize * (TileIndexInAtlas % TILE_ATLAS_WIDTH_IN_TILES),
        Atlas->TileSize * (TileIndexInAtlas / TILE_ATLAS_WIDTH_IN_TILES)
    );
}

bool
FOdysseyTileManager::FTileTextureHandle::IsValid() const
{
    return TileIndexInAtlas != INDEX_NONE;
}
