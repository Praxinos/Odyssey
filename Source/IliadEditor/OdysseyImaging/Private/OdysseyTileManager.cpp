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
    FString CacheDirectory = FPaths::GetPath(GetCacheOnDiskPath());
    IFileManager::Get().DeleteDirectory(*CacheDirectory, false, true);
}

void
FOdysseyTileManager::Finalize()
{
    FString CacheDirectory = FPaths::GetPath(GetCacheOnDiskPath());
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
    TArray<FSharedBuffer> TileOldBuffers;
    TArray<TSharedPtr<FRHIGPUTextureReadback>> TileReadBacks;
    TArray<TSharedPtr<FRHIGPUBufferReadback>> TileIsEmptyReadBacks;
    TileOldBuffers.Reserve(TilePositions.Num());
    TileReadBacks.Reserve(TilePositions.Num());
    TileIsEmptyReadBacks.Reserve(TilePositions.Num());

    TArray<FOdysseyTileManager::FCreatedTile> CreatedTiles;

    FScopeLock Lock(&FreeTilesMutex);
    for (const FIntPoint& TilePosition : TilePositions)
    {
        FTile Tile;

        FOdysseyTileId OldTileId;
        if (InGetExistingTileId.IsBound())
            OldTileId = InGetExistingTileId.Execute(TilePosition);

        FSharedBuffer OldTileBuffer;
        if (OldTileId.IsValid())
            GetTileBuffer(OldTileId, OldTileBuffer);

        TileOldBuffers.Add(OldTileBuffer);

        Tile.GPUReadBack = MakeShared<FRHIGPUTextureReadback>(TEXT("FOdysseyTileManager::ReadBack"));
        TileReadBacks.Add(Tile.GPUReadBack);

        Tile.GPUIsEmptyReadBack = MakeShared<FRHIGPUBufferReadback>(TEXT("FOdysseyTileManager::IsEmptyReadBack"));
        TileIsEmptyReadBacks.Add(Tile.GPUIsEmptyReadBack);

        FOdysseyTileId TileId;
        if (FreeTiles.Num() > 0)
        {
            TileId.Index = FreeTiles[0];
            TileId.Generation = Tiles[TileId.Index].Generation + 1;
            Tile.Generation = TileId.Generation;
            Tiles[TileId.Index] = MoveTemp(Tile);

            FreeTiles.RemoveAtSwap(0);
        }
        else
        {
            TileId.Index = Tiles.Num();
            TileId.Generation = 0;
            Tile.Generation = TileId.Generation;
            Tiles.Add(MoveTemp(Tile));
        }

        PendingTilesToReadBack.Add(TileId);

        FCreatedTile CreatedTile;
        CreatedTile.Id = TileId;
        CreatedTile.Pos = TilePosition;
        CreatedTiles.Add(CreatedTile);
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
            //RHICmdList.SubmitAndBlockUntilGPUIdle();
            RHICmdList.ImmediateFlush(EImmediateFlushType::FlushRHIThread);
        }
    );

    return CreatedTiles;
}

void
FOdysseyTileManager::TryLoadTileFromReadBack(FOdysseyTileId InTileId)
{
    FTile& Tile = Tiles[InTileId.Index];
    TSharedPtr<FRHIGPUTextureReadback> ReadBack = Tile.GPUReadBack;
    TSharedPtr<FRHIGPUBufferReadback> IsEmptyReadBack = Tile.GPUIsEmptyReadBack;
    bool IsReady = ReadBack.IsValid() && ReadBack->IsReady() && IsEmptyReadBack.IsValid() && IsEmptyReadBack->IsReady();
    if (IsReady)
        PendingTilesToReadBack.RemoveSwap(InTileId);

    LoadTileFromReadBack(InTileId);
}

void
FOdysseyTileManager::LoadTileFromReadBack(FOdysseyTileId InTileId)
{
    PendingTilesToReadBack.RemoveSwap(InTileId);
    FTile& Tile = Tiles[InTileId.Index];

    if (Tile.Generation != InTileId.Generation)
        return;

    TPromise<TSharedPtr<FTileData>> Promise;
    Tile.TileData = Promise.GetFuture();

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
            TileIndex = InTileId.Index
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
                    FScopeLock Lock(&FreeTilesMutex);
                    FreeTiles.Add(TileIndex);
                    Promise.SetValue(nullptr);
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

                //Compare Buffer to other buffers
                FIoHash Hash = FIoHashBuilder::HashBuffer(Buffer.GetView());
                TArray<TSharedPtr<FTileData>> TilesData;
                HashToTileData.MultiFind(Hash, TilesData, false);
                for (TSharedPtr<FTileData> TileData : TilesData)
                {
                    FSharedBuffer TileDataBuffer;
                    if(!TileData->GetUncompressedBuffer(TileDataBuffer))
                        continue;

                    if (TileDataBuffer.GetView().EqualBytes(Buffer.GetView()))
                    {
                        Promise.SetValue(TileData);
                        return;
                    }
                }

                //Create a new TileData
                TSharedPtr<FTileData> TileData = FTileData::FromUncompressedBuffer(Buffer);
                HashToTileData.Add(Hash, TileData);
                Promise.SetValue(TileData);
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

    if (Tile.Generation != InTileId.Generation)
        return false;

    //If readback is still valid, we need to start reading back
    if (Tile.GPUReadBack.IsValid())
        LoadTileFromReadBack(InTileId);

    //Here we get the tile data index
    //Tile.TileData is a future, so it will block current thread until GPU Readback is done
    TSharedPtr<FTileData> TileData = Tile.TileData.Get();
    if (!TileData)
        return false;

    return TileData->GetUncompressedBuffer(OutBuffer);
}

FOdysseyTileManagerStats&
FOdysseyTileManager::GetStats()
{
    return Stats;
}


void
FOdysseyTileManager::Tick(float DeltaTime)
{
    TArray<FOdysseyTileId> LocalPendingTilesToReadBack = PendingTilesToReadBack;
    for (FOdysseyTileId TileId : LocalPendingTilesToReadBack)
    {
        TryLoadTileFromReadBack(TileId);
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

TSharedRef<FOdysseyTileManager::FTileData>
FOdysseyTileManager::FTileData::FromUncompressedBuffer(const FSharedBuffer& InUncompressedBuffer)
{
    TSharedRef<FTileData> TileData = MakeShared<FTileData>();
    TileData->UncompressedBuffer = InUncompressedBuffer;

    FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
    TileStats.AddUncompressed(TileData->UncompressedBuffer);

    Touch(TileData);

    //Go to next pipeline step
    Async(
        EAsyncExecution::ThreadPool,
        [TileData]()
        {
            TileData->Compress();
        }
    );

    return TileData;
}

TSharedRef<FOdysseyTileManager::FTileData>
FOdysseyTileManager::FTileData::FromCompressedBuffer(const FCompressedBuffer& InCompressedBuffer)
{
    TSharedRef<FTileData> TileData = MakeShared<FTileData>();
    TileData->CompressedBuffer = InCompressedBuffer;

    FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
    TileStats.AddCompressed(TileData->CompressedBuffer);

    Touch(TileData);

    Async(
        EAsyncExecution::ThreadPool,
        [TileData]()
        {
            TileData->CacheOnDisk();
        }
    );

    return TileData;
}

bool
FOdysseyTileManager::FTileData::GetUncompressedBuffer(FSharedBuffer& OutBuffer) const
{
    FScopeLock Lock(&Mutex);
    if (!LoadUncompressedBuffer())
        return false;

    Touch(ConstCastSharedRef<FOdysseyTileManager::FTileData>(AsShared()));
    OutBuffer = UncompressedBuffer;

    return true;
}

bool
FOdysseyTileManager::FTileData::GetCompressedBuffer(FCompressedBuffer& OutBuffer) const
{
    FScopeLock Lock(&Mutex);
    if (!LoadCompressedBuffer())
        return false;

    Touch(ConstCastSharedRef<FOdysseyTileManager::FTileData>(AsShared()));
    OutBuffer = CompressedBuffer;
    return true;
}

bool
FOdysseyTileManager::FTileData::LoadUncompressedBuffer() const
{
    if (UncompressedBuffer.IsNull())
    {
        if (!LoadCompressedBuffer())
            return false;

        UncompressedBuffer = CompressedBuffer.Decompress();

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
    if (CompressedBuffer.IsNull())
    {
        FScopeLock Lock(&CacheOnDiskMutex);

        FString FilePath = GetCacheOnDiskPath();
        const TUniquePtr<FArchive> Ar(IFileManager::Get().CreateFileReader(*FilePath, FILEWRITE_Append));
        if (!Ar)
            return false;

        Ar->Seek(DiskCacheOffset);
        *Ar << CompressedBuffer;

        if (!CompressedBuffer.IsNull())
        {
            FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
            TileStats.AddCompressed(CompressedBuffer);
        }
        Ar->Close();
    }
    return true;
}

void
FOdysseyTileManager::FTileData::Compress()
{
    CompressedBuffer = FCompressedBuffer::Compress(UncompressedBuffer);

    FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
    TileStats.AddCompressed(CompressedBuffer);

    Async(
        EAsyncExecution::ThreadPool,
        [self = AsShared()]()
        {
            self->CacheOnDisk();
        }
    );
}

void
FOdysseyTileManager::FTileData::CacheOnDisk()
{
    FScopeLock Lock(&CacheOnDiskMutex);

    FString FilePath = FOdysseyTileManager::GetCacheOnDiskPath();

    const TUniquePtr<FArchive> Ar(IFileManager::Get().CreateFileWriter(*FilePath, FILEWRITE_Append));
    if (!Ar)
        return;

    DiskCacheOffset = Ar->Tell();
    *Ar << CompressedBuffer;

    FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
    TileStats.AddCachedOnDisk(Ar->TotalSize() - DiskCacheOffset);

    Ar->Close();
}

void
FOdysseyTileManager::FTileData::EvictUncompressed()
{
    //Eviction is not permitted if Caching is not yet finished
    if (DiskCacheOffset == INDEX_NONE)
        return;

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
    if (DiskCacheOffset == INDEX_NONE)
        return;

    TDoubleLinkedList<TSharedPtr<FTileData>>::TDoubleLinkedListNode* Node = CompressedLRUNode;
    if (!Node)
        return;

    FOdysseyTileManagerStats& TileStats = FOdysseyTileManager::Get().GetStats();
    TileStats.RemoveCompressed(CompressedBuffer);

    CompressedBuffer = FCompressedBuffer();
    CompressedLRUNode = nullptr;
    CompressedLRU.RemoveNode(Node);
}

void
FOdysseyTileManager::FTileData::Touch(TSharedRef<FOdysseyTileManager::FTileData> InTileData)
{
    FScopeLock Lock(&Mutex);

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
