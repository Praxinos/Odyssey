// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVirtualRenderTargetProducer.h"

#include "OdysseyVirtualRenderTargetData.h"

FOdysseyVirtualRenderTargetProducer::~FOdysseyVirtualRenderTargetProducer()
{

}

FOdysseyVirtualRenderTargetProducer::FOdysseyVirtualRenderTargetProducer(const FName& InName, TSharedPtr<class FOdysseyVirtualRenderTargetData> InData, int32 InFirstMipToUse, const FVTProducerDescription& InProducerDesc)
    : Name(InName)
    , WeakVTData(InData)
    , FirstMipOffset(InFirstMipToUse)
    , ProducerDesc(InProducerDesc)
    , Finalizer(InProducerDesc)
{

}

uint32
FOdysseyVirtualRenderTargetProducer::GetLocalMipBias(uint8 vLevel, uint32 vAddress) const
{
    TSharedPtr<FOdysseyVirtualRenderTargetData> VTData = WeakVTData.Pin();
    if (!VTData)
        return 0;

    vLevel += FirstMipOffset;

    const uint32 NumMips = VTData->NumMips;
    uint32 Current_vLevel = vLevel;
    uint32 Current_vAddress = vAddress;
    while (Current_vLevel < NumMips)
    {
        if (!VTData->IsValidAddress(Current_vLevel, Current_vAddress))
        {
            // vAddress is out-of-bounds for the given producer
            Current_vLevel = NumMips - 1u;
            break;
        }

        const uint32 TileOffset = VTData->GetTileOffset(Current_vLevel, Current_vAddress, 0);
        if (TileOffset != ~0u)
        {
            break;
        }

        Current_vLevel++;
        Current_vAddress >>= 2;
    }

    return Current_vLevel - vLevel;
}

FVTRequestPageResult
FOdysseyVirtualRenderTargetProducer::RequestPageData(FRHICommandListBase& RHICmdList, const FVirtualTextureProducerHandle& ProducerHandle, uint8 LayerMask, uint8 vLevel, uint64 vAddress, EVTRequestPagePriority Priority)
{
    vLevel += FirstMipOffset;

    //TODO: Request the tile (what does it mean ?)
    //Here we have to load the tile's data from anywhere it's located (BulkData, DDC, or anywhere else)
    //For example : A classical VirtualTexture might load the tile's data from DDC here and keep it in memory for later
    //Other example : FTextureCollectionVirtualRedirector can render from another texture already on GPU, in this case
    // it just ensures the texture is available on GPU for later use in ProducePageData
    //
    //So requesting page data just means "loading data from somewhere to somewhere else to be used later in ProducePageData()"

    return FVTRequestPageResult(EVTRequestPageStatus::Available, 0);
}

IVirtualTextureFinalizer*
FOdysseyVirtualRenderTargetProducer::ProducePageData(
    FRHICommandListBase& RHICmdList,
    ERHIFeatureLevel::Type FeatureLevel,
    EVTProducePageFlags Flags,
    const FVirtualTextureProducerHandle& ProducerHandle, uint8 LayerMask, uint8 vLevel, uint64 vAddress,
    uint64 RequestHandle,
    const FVTProduceTargetLayer* TargetLayers
)
{
    TSharedPtr<FOdysseyVirtualRenderTargetData> VTData = WeakVTData.Pin();
    if (!VTData)
        return nullptr;

    const uint32 SkipBorderSize = (Flags & EVTProducePageFlags::SkipPageBorders) != EVTProducePageFlags::None ? VTData->TileBorderSize : 0;

    //TODO: Produce Tile (What does it mean ?)
    // Here we need to prepare a Finalizer
    // The finalizer role is to copy some data at the right place on GPU
    // Preparing the finalizer means defining what data to copy, to which target to copy it.

    UE_LOG(LogTemp, Warning, TEXT("Produce %d, %d"), vAddress, vLevel);

    FOdysseyVirtualRenderTargetFinalizer::FTileEntry Tile;
    Tile.Target = TargetLayers[0];
    Tile.vAddress = vAddress;
    Tile.vLevel = vLevel;
    Finalizer.AddTile(Tile);
    return &Finalizer;
}

void
FOdysseyVirtualRenderTargetProducer::GatherProducePageDataTasks(FVirtualTextureProducerHandle const& ProducerHandle, FGraphEventArray& InOutTasks) const
{
    //TODO: What should we do here ?
}

void
FOdysseyVirtualRenderTargetProducer::GatherProducePageDataTasks(uint64 RequestHandle, FGraphEventArray& InOutTasks) const
{
    //TODO: What should we do here ?
}

void
FOdysseyVirtualRenderTargetProducer::DumpToConsole(bool verbose)
{
    //TODO: Write whatever debug info you need to the console
    // See FUploadingVirtualTexture::DumpToConsole() for an example
}
