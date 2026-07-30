// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVirtualRenderTargetProducer.h"

#include "OdysseyVirtualRenderTargetData.h"

FOdysseyVirtualRenderTargetProducer::~FOdysseyVirtualRenderTargetProducer()
{

}

FOdysseyVirtualRenderTargetProducer::FOdysseyVirtualRenderTargetProducer(const FName& InName, TSharedPtr<class FOdysseyVirtualRenderTargetData> InData, int32 InFirstMipToUse)
    : Name(InName)
    , WeakVTData(InData)
    , FirstMipOffset(InFirstMipToUse)
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

    return FVTRequestPageResult();
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

    return nullptr;
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
