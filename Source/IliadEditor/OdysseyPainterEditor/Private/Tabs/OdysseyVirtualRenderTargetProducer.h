// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "VirtualTexturing.h"

class FOdysseyVirtualRenderTargetProducer
    : public IVirtualTexture //This is actually the base class for a Producer
{
public:
    virtual ~FOdysseyVirtualRenderTargetProducer();
    FOdysseyVirtualRenderTargetProducer(const FName& InName, TSharedPtr<class FOdysseyVirtualRenderTargetData> InData, int32 FirstMipToUse);

public:
    // IVirtualTexture interface
    virtual uint32 GetLocalMipBias(uint8 vLevel, uint32 vAddress) const override;
    virtual bool IsPageStreamed(uint8 vLevel, uint32 vAddress) const override { return true; }
    virtual FVTRequestPageResult RequestPageData(FRHICommandListBase& RHICmdList, const FVirtualTextureProducerHandle& ProducerHandle, uint8 LayerMask, uint8 vLevel, uint64 vAddress, EVTRequestPagePriority Priority) override;
    virtual IVirtualTextureFinalizer* ProducePageData(FRHICommandListBase& RHICmdList,
        ERHIFeatureLevel::Type FeatureLevel,
        EVTProducePageFlags Flags,
        const FVirtualTextureProducerHandle& ProducerHandle, uint8 LayerMask, uint8 vLevel, uint64 vAddress,
        uint64 RequestHandle,
        const FVTProduceTargetLayer* TargetLayers) override;
    virtual void GatherProducePageDataTasks(FVirtualTextureProducerHandle const& ProducerHandle, FGraphEventArray& InOutTasks) const override;
    virtual void GatherProducePageDataTasks(uint64 RequestHandle, FGraphEventArray& InOutTasks) const override;
    virtual void DumpToConsole(bool verbose) override;
    // End IVirtualTexture interface

private:
    FName Name;
    TWeakPtr<class FOdysseyVirtualRenderTargetData> WeakVTData;
    int32 FirstMipOffset;
};
