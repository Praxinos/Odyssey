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
#include "ScreenPass.h"
#include "Stats/Stats.h"
#include "TextureResource.h"

#include "OdysseyAreTextureTilesEmptyShader.h"
#include "OdysseyTileManagerSettings.h"
#include "OdysseyTileUtils.h"

/**
 * A 64x64 Tiles Atlas of 64x64 RGBA8 pixels tiles
 * Is ~64MB on GPU
 */
#define TILE_ATLAS_MAX_WIDTH_IN_TILES 64
#define TILE_ATLAS_MAX_HEIGHT_IN_TILES 64
#define TILE_ATLAS_MAX_TILES (TILE_ATLAS_MAX_WIDTH_IN_TILES * TILE_ATLAS_MAX_HEIGHT_IN_TILES)

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
        InPosition + (SourceRect.Min - InRect.Min) + SourceRect.Size()
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

    TilesToCreate.Reserve(TilePositions.Num());

    TArray<FOdysseyTileManager::FCreatedTile> CreatedTiles;

    TArray<FOdysseyTileId> OldTileIds;
    OldTileIds.Reserve(TilePositions.Num());
    for (const FIntPoint& TilePosition : TilePositions)
    {
        FOdysseyTileId OldTileId;
        if (InGetExistingTileId.IsBound())
            OldTileId = InGetExistingTileId.Execute(TilePosition);
        OldTileIds.Add(OldTileId);
    }

    TArray<TFuture<TSharedPtr<FTileTextureHandle>>> OldTileTextureHandles = GetTileTextures(OldTileIds, InTileSize, InTileFormat);

    for (int i = 0; i < TilePositions.Num(); i++)
    {
        const FIntPoint& TilePosition = TilePositions[i];

        //Create a new tile representing the new version of the updated tile
        TSharedPtr<FTile> Tile = CreateNewTile();

        FTileToCreate TileToCreate;
        TileToCreate.Position = TilePosition;
        TileToCreate.Tile = Tile;
        TileToCreate.OldTextureHandle = MoveTemp(OldTileTextureHandles[i]);
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

            FRDGTextureRef SourceTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(Source->GetResource()->TextureRHI, TEXT("FOdysseyTileManager::CreateTiles")));

            const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
            TMap<FRDGTextureRef, TArray<int>> ReadBackParams;

            uint32 NumAtlases = (TilesToCreate.Num() + TILE_ATLAS_MAX_TILES - 1) / TILE_ATLAS_MAX_TILES;
            for (uint32 AtlasIndex = 0; AtlasIndex < NumAtlases; AtlasIndex++)
            {
                //Create an Atlas Texture containing the new tiles
                uint32 NumTilesInAtlas = FMath::Min(uint32(TILE_ATLAS_MAX_TILES), TilesToCreate.Num() - AtlasIndex * TILE_ATLAS_MAX_TILES);
                TSharedRef<FTileAtlas> Atlas = MakeShared<FTileAtlas>(RHICmdList, TileSize, PixelFormat, NumTilesInAtlas);
                FRDGTextureRef AtlasTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(Atlas->GetTexture(), TEXT("FOdysseyTileManager::TileAtlasTexture")));

                TArray<FIntPoint> TilePositionsInAtlas;
                TilePositionsInAtlas.Reserve(NumTilesInAtlas);
                for (uint32 TileIndex = 0; TileIndex < NumTilesInAtlas; TileIndex++)
                {
                    int TileToCreateIndex = AtlasIndex * TILE_ATLAS_MAX_TILES + TileIndex;
                    FTileToCreate& TileToCreate = TilesToCreate[TileToCreateIndex];

                    //Reserve the tile in the tile atlas
                    TileToCreate.TextureHandle = MakeShared<FTileTextureHandle>(Atlas, TileIndex);
                    TilePositionsInAtlas.Add(TileToCreate.TextureHandle->GetPositionInTexture());

                    TSharedPtr<FTileTextureHandle> OldTileTextureHandle;
                    if (TileToCreate.OldTextureHandle.IsValid())
                        OldTileTextureHandle = TileToCreate.OldTextureHandle.Get();

                    //Ensure the Rects are precisely the one we need
                    //to prevent bleeding on neighbour Tiles in the Tile's Texture Atlas
                    FIntPoint TileDstPos = TileToCreate.Position * TileSize;
                    FIntRect TileDstRect(TileDstPos, TileDstPos + FIntPoint(TileSize, TileSize));
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

                    //Draw in the Tile Texture
                    if (OldTileTextureHandle.IsValid())
                    {
                        //First we draw the Old pixels in the tile
                        //This is needed, as the new pixels can occupy a partial amount of the tile
                        FRDGTextureRef TileOldTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(OldTileTextureHandle->GetTextureRHI(), TEXT("FOdysseyTileManager::TileOldTexture")));

                        AddDrawTexturePass(
                            GraphBuilder,
                            FScreenPassViewInfo(),
                            TileOldTexture,
                            AtlasTexture,
                            OldTileTextureHandle->GetPositionInTexture(),
                            FIntPoint(TileSize, TileSize),
                            TileToCreate.TextureHandle->GetPositionInTexture(),
                            FIntPoint(TileSize, TileSize)
                        );
                    }
                    else
                    {
                        //First we clear the Tile
                        FRDGTextureClearInfo ClearInfo;

                        ClearInfo.Viewport = TileToCreate.TextureHandle->GetRectInTexture();
                        ClearInfo.ClearColor = FLinearColor::Transparent;

                        AddClearRenderTargetPass(GraphBuilder, AtlasTexture, ClearInfo);
                    }

                    AddDrawTexturePass(
                        GraphBuilder,
                        FScreenPassViewInfo(),
                        SourceTexture,
                        AtlasTexture,
                        TileSrcRect.Min,
                        TileSrcRect.Size(),
                        TileDstRect.Min + TileToCreate.TextureHandle->GetPositionInTexture(),
                        TileSrcRect.Size()
                    );
                }

                //Initiate Is Altas Texture Empty Readback
                TSharedRef<FRHIGPUBufferReadback> GPUIsEmptyReadBack = MakeShared<FRHIGPUBufferReadback>(TEXT("FOdysseyTileManager::FTile::GPUIsEmptyReadBack"));
                Odyssey::Shaders::AddAreTextureTilesEmptyPass(
                    GraphBuilder,
                    GMaxRHIFeatureLevel,
                    AtlasTexture,
                    TilePositionsInAtlas,
                    TileSize,
                    GPUIsEmptyReadBack
                );

                //Initiate Altas Texture Readback
                TSharedPtr<FRHIGPUTextureReadback> GPUReadBack = MakeShared<FRHIGPUTextureReadback>(TEXT("FOdysseyTileManager::FTile::GPUReadBack"));
                AddEnqueueCopyPass(
                    GraphBuilder,
                    GPUReadBack.Get(),
                    AtlasTexture
                );

                const FPixelFormatInfo& PixelFormatInfo = GPixelFormats[PixelFormat];
                uint32 BytesPerPixel = PixelFormatInfo.BlockBytes;

                for (uint32 TileIndex = 0; TileIndex < NumTilesInAtlas; TileIndex++)
                {
                    int TileToCreateIndex = AtlasIndex * TILE_ATLAS_MAX_TILES + TileIndex;
                    FTileToCreate& TileToCreate = TilesToCreate[TileToCreateIndex];

                    //IsEmpty ReadBack
                    FTile::FPartialBufferReadBack IsEmptyReadBack;
                    IsEmptyReadBack.ReadBack = GPUIsEmptyReadBack;
                    IsEmptyReadBack.ResultSize = NumTilesInAtlas * sizeof(uint32);
                    IsEmptyReadBack.ResultIndex = TileIndex;

                    TileToCreate.Tile->GPUIsEmptyReadBackPromise->SetValue(IsEmptyReadBack);

                    //Pixels ReadBack
                    FTile::FPartialTextureReadBack PixelsReadBack;
                    PixelsReadBack.ReadBack = GPUReadBack;
                    PixelsReadBack.PositionInPixels = Atlas->GetTilePosition(TileIndex);
                    PixelsReadBack.BytesPerPixel = BytesPerPixel;
                    PixelsReadBack.BytesPerTextureRow = Atlas->GetWidthInTiles() * TileSize * BytesPerPixel;
                    PixelsReadBack.TileSize = TileSize;

                    TileToCreate.Tile->GPUReadBackPromise->SetValue(PixelsReadBack);
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

    if (!InTile->GPUIsEmptyReadBack.IsValid() || !InTile->GPUReadBack.IsValid())
        return;

    const FTile::FPartialBufferReadBack& IsEmptyReadBack = InTile->GPUIsEmptyReadBack.Get();
    if (!IsEmptyReadBack.ReadBack.IsValid() || !IsEmptyReadBack.ReadBack->IsReady())
        return;

    const FTile::FPartialTextureReadBack& PixelsReadBack = InTile->GPUReadBack.Get();
    if (!PixelsReadBack.ReadBack.IsValid() || !PixelsReadBack.ReadBack->IsReady())
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
                const FTile::FPartialBufferReadBack& IsEmptyReadBack = InTile->GPUIsEmptyReadBack.Get();
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
                    InTile->GPUReadBack.Reset();
                    InTile->GPUReadBackPromise.Reset();
                    InTile->GPUReadBackState = FTile::EGPUReadBackState::Done;

                    FOdysseyTileId TileId = InTile->Id;
                    TileId.Generation++;
                    FreeTiles.Add(TileId);
                    return;
                }
            }

            //Tile Data ReadBack
            {
                const FTile::FPartialTextureReadBack& PixelsReadBack = InTile->GPUReadBack.Get();
                // This blocks the render thread until the GPU has completed
                // the readback copy.
                PixelsReadBack.ReadBack->Wait(RHICmdList, GPUMask);

                // At this point Lock() is safe.
                int32 Width = 0;
                int32 Height = 0;

                void* Data = PixelsReadBack.ReadBack->Lock(Width, &Height);

                uint64 BufferSize = PixelsReadBack.TileSize * PixelsReadBack.TileSize * PixelsReadBack.BytesPerPixel;
                FSharedBuffer TileBuffer = FUniqueBuffer::Alloc(BufferSize).MoveToShared();

                for (uint32 Y = 0; Y < PixelsReadBack.TileSize; ++Y)
                {
                    const uint8* Src =
                        (const uint8*)Data
                        + (PixelsReadBack.PositionInPixels.Y + Y) * PixelsReadBack.BytesPerTextureRow
                        + PixelsReadBack.PositionInPixels.X * PixelsReadBack.BytesPerPixel;

                    uint8* Dst = (uint8*)TileBuffer.GetData() + Y * PixelsReadBack.TileSize * PixelsReadBack.BytesPerPixel;

                    FMemory::Memcpy(Dst, Src, PixelsReadBack.TileSize * PixelsReadBack.BytesPerPixel);
                }

                PixelsReadBack.ReadBack->Unlock();

                //Compare Buffer to other buffers
                FIoHash Hash = FIoHashBuilder::HashBuffer(TileBuffer.GetView());
                TArray<TSharedPtr<FTileData>> TilesData;

                {
                    FScopeLock HashToTileDataLock(&HashToTileDataMutex);
                    HashToTileData.MultiFind(Hash, TilesData, false);

                    for (TSharedPtr<FTileData> TileData : TilesData)
                    {
                        FSharedBuffer TileDataBuffer;
                        if(!TileData->GetUncompressedBuffer(TileDataBuffer))
                            continue;

                        if (TileDataBuffer.GetView().EqualBytes(TileBuffer.GetView()))
                        {
                            FScopeLock TileLock(&InTile->Mutex);
                            InTile->TempTexture.Reset();
                            InTile->TempTexturePromise.Reset();
                            InTile->TileDataPromise.SetValue(TileData);
                            InTile->GPUIsEmptyReadBack.Reset();
                            InTile->GPUIsEmptyReadBackPromise.Reset();
                            InTile->GPUReadBack.Reset();
                            InTile->GPUReadBackPromise.Reset();
                            InTile->GPUReadBackState = FTile::EGPUReadBackState::Done;
                            return;
                        }
                    }

                    //Create a new TileData
                    TSharedPtr<FTileData> TileData = FTileData::FromUncompressedBuffer(Hash, TileBuffer);
                    HashToTileData.Add(Hash, TileData);

                    {
                        FScopeLock TileLock(&InTile->Mutex);
                        InTile->TempTexture.Reset();
                        InTile->TempTexturePromise.Reset();
                        InTile->TileDataPromise.SetValue(TileData);
                        InTile->GPUIsEmptyReadBack.Reset();
                        InTile->GPUIsEmptyReadBackPromise.Reset();
                        InTile->GPUReadBack.Reset();
                        InTile->GPUReadBackPromise.Reset();
                        InTile->GPUReadBackState = FTile::EGPUReadBackState::Done;
                    }
                }
            }
        }
    );
}

TArray<TFuture<TSharedPtr<FOdysseyTileManager::FTileTextureHandle>>>
FOdysseyTileManager::GetTileTextures(const TArray<FOdysseyTileId>& InTileIds, uint32 InTileSize, EPixelFormat InTileFormat) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyTileManager::GetTileTextures);

    check(IsInGameThread());
    TArray<TPromise<TSharedPtr<FTileTextureHandle>>> RenderThreadPromises;
    RenderThreadPromises.Reserve(InTileIds.Num());

    TArray<TFuture<TSharedPtr<FTileTextureHandle>>> Futures;
    Futures.Reserve(InTileIds.Num());

    TArray<FSharedBuffer> Buffers;
    Buffers.Reserve(InTileIds.Num());
    for (int i = 0; i < InTileIds.Num(); i++)
    {
        const FOdysseyTileId& TileId = InTileIds[i];

        TPromise<TSharedPtr<FTileTextureHandle>> Promise;
        TFuture<TSharedPtr<FTileTextureHandle>> Future = Promise.GetFuture();
        Futures.Add(MoveTemp(Future));

        //Check if TileIndex is out of bounds
        if (!TileId.IsValid() || TileId.Index >= (uint64)Tiles.Num())
        {
            Promise.SetValue(TSharedPtr<FTileTextureHandle>());
            continue;
        }

        //Retrieve Tile
        TSharedPtr<FTile> Tile = Tiles[TileId.Index];

        //We read the tile here
        //We lock its mutex to ensure it is not modified by another thread
        FScopeLock TileLock(&Tile->Mutex);

        //If Tile.Id does not correspond to TileId
        //it indicates the tile corresponding to the TileId was empty
        //and freed.
        if (Tile->Id != TileId)
        {
            Promise.SetValue(TSharedPtr<FTileTextureHandle>());
            continue;
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
                continue;
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
            continue;
        }

        FSharedBuffer Buffer;
        if (!TileData->GetUncompressedBuffer(Buffer))
        {
            //Tile's Buffer could not be retrieved
            //indicating an empty Tile
            Promise.SetValue(TSharedPtr<FTileTextureHandle>());
            continue;
        }

        RenderThreadPromises.Add(MoveTemp(Promise));
        Buffers.Add(Buffer);
    }

    if (Buffers.Num() > 0)
    {
        ENQUEUE_RENDER_COMMAND(FOdysseyTileManager_GetTileTextures)(
            [
                self = const_cast<FOdysseyTileManager*>(this),
                Buffers,
                PixelFormat = InTileFormat,
                TileSize = InTileSize,
                Promises = MoveTemp(RenderThreadPromises)
            ](FRHICommandListImmediate& RHICmdList) mutable
            {
                TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::GetTileTextures_0);

                uint32 NumAtlases = (Buffers.Num() + TILE_ATLAS_MAX_TILES - 1) / TILE_ATLAS_MAX_TILES;
                FRDGBuilder GraphBuilder(RHICmdList);
                for (uint32 AtlasIndex = 0; AtlasIndex < NumAtlases; AtlasIndex++)
                {
                    //Create an Atlas Texture containing the new tiles
                    uint32 NumTilesInAtlas = FMath::Min(uint32(TILE_ATLAS_MAX_TILES), Buffers.Num() - AtlasIndex * TILE_ATLAS_MAX_TILES);
                    TSharedRef<FTileAtlas> Atlas = MakeShared<FTileAtlas>(RHICmdList, TileSize, PixelFormat, NumTilesInAtlas);
                    FRDGTextureRef AtlasTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(Atlas->GetTexture(), TEXT("FOdysseyTileManager::TileAtlasTexture")));

                    for (uint32 TileIndex = 0; TileIndex < NumTilesInAtlas; TileIndex++)
                    {
                        int BufferIndex = AtlasIndex * TILE_ATLAS_MAX_TILES + TileIndex;
                        FSharedBuffer Buffer = Buffers[BufferIndex];
                        TPromise<TSharedPtr<FTileTextureHandle>>& Promise = Promises[BufferIndex];

                        TSharedPtr<FTileTextureHandle> TileTextureHandle = MakeShared<FTileTextureHandle>(Atlas, TileIndex);
                        Promise.SetValue(TileTextureHandle);

                        AddWriteTilePass(
                            GraphBuilder,
                            AtlasTexture,
                            TileTextureHandle->GetPositionInTexture(),
                            TileSize,
                            Buffer
                        );
                    }
                }

                GraphBuilder.Execute();
            }
        );
    }

    return Futures;
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

    GPUReadBackPromise = MakeUnique<TPromise<FPartialTextureReadBack>>();
    GPUReadBack = GPUReadBackPromise->GetFuture();
    GPUIsEmptyReadBackPromise = MakeUnique<TPromise<FPartialBufferReadBack>>();
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

FOdysseyTileManager::FTileAtlas::FTileAtlas(FRHICommandList& InRHICmdList, uint32 InTileSize, EPixelFormat InTileFormat, uint32 InNumTiles)
    : TileSize(InTileSize)
    //, TileFormat(InTileFormat)
{
    HeightInTiles = FMath::RoundUpToPowerOfTwo((InNumTiles - 1) / TILE_ATLAS_MAX_WIDTH_IN_TILES + 1);
    WidthInTiles = (InNumTiles - 1) / HeightInTiles + 1;
    FIntPoint AtlasSize(WidthInTiles * InTileSize, HeightInTiles * InTileSize);

    const FRHITextureCreateDesc CreateDesc = FRHITextureCreateDesc::Create2D(TEXT("FOdysseyTileManager::FTileAtlas::Texture"))
        .SetExtent(AtlasSize)
        .SetFormat(InTileFormat)
        .SetClearValue(FClearValueBinding::Transparent)
        .SetFlags(ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable);

    Texture = InRHICmdList.CreateTexture(CreateDesc);
}

FIntRect
FOdysseyTileManager::FTileAtlas::GetTileRect(uint32 InTileIndexInAtlas) const
{
    FIntPoint Position = GetTilePosition(InTileIndexInAtlas);
    return FIntRect(
        Position,
        Position + FIntPoint(TileSize, TileSize)
    );
}

FIntPoint
FOdysseyTileManager::FTileAtlas::GetTilePosition(uint32 InTileIndexInAtlas) const
{
    return FIntPoint(
        TileSize * (InTileIndexInAtlas % WidthInTiles),
        TileSize * (InTileIndexInAtlas / WidthInTiles)
    );
}

FOdysseyTileManager::FTileTextureHandle::~FTileTextureHandle()
{
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

    return Atlas->GetTexture();
}

FIntRect
FOdysseyTileManager::FTileTextureHandle::GetRectInTexture() const
{
    if (!Atlas)
        return FIntRect();

    return Atlas->GetTileRect(TileIndexInAtlas);
}

FIntPoint
FOdysseyTileManager::FTileTextureHandle::GetPositionInTexture() const
{
    if (!Atlas)
        return FIntPoint();

    return Atlas->GetTilePosition(TileIndexInAtlas);
}

bool
FOdysseyTileManager::FTileTextureHandle::IsValid() const
{
    return TileIndexInAtlas != INDEX_NONE;
}
