// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "VirtualTexturing.h"

#include "OdysseyVirtualRenderTargetFinalizer.h"

/**
 * A Finalizer is an object that does the final work of filling the physical textures.
 * The work for all finalizers is scheduled at a specific point in the frame where we can write to the physical texture without hazards, and
 * where the virtual texture page tables are also updated.
 * The finalizer work may be split into two parts.
 * RenderFinalize() does not write to the physical texture, but can read from virtual textures. This is a phase that allows any
 * page rendering which may need to sample virtual textures. Runtime virtual textures and material systems require this.
 * Finalise() must write to the physical textures, but cannot sample from them.
 * All finalizers need to implement Finalize() but only ones that need to sample virtual textures need to implement RenderFinalize().
 */
class FOdysseyVirtualRenderTargetProducer
    : public IVirtualTexture //This is actually the base class for a Producer
{
public:
    virtual ~FOdysseyVirtualRenderTargetProducer();
    FOdysseyVirtualRenderTargetProducer(const FName& InName, TSharedPtr<class FOdysseyVirtualRenderTargetData> InData, int32 FirstMipToUse, const FVTProducerDescription& InProducerDesc);

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
    FVTProducerDescription ProducerDesc;

    FOdysseyVirtualRenderTargetFinalizer Finalizer;
};
