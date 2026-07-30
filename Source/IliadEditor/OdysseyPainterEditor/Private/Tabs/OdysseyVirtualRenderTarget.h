// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "RenderResource.h"
#include "VirtualTexturing.h"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorTiledViewportClient
class FOdysseyVirtualRenderTargetResource
    : public FTexture
{
public:
    // Construction / Destruction
    virtual ~FOdysseyVirtualRenderTargetResource() = default;
    FOdysseyVirtualRenderTargetResource(TSharedPtr<class FOdysseyVirtualRenderTargetData> InVTData);

public:
    // FTexture overrides
    virtual void InitRHI(FRHICommandListBase& RHICmdList) override;
    virtual void ReleaseRHI() override;

    virtual uint32 GetSizeX() const override;
    virtual uint32 GetSizeY() const override;
    // FTexture overrides

    const FVirtualTextureProducerHandle& GetProducerHandle() const { return ProducerHandle; }

    /**
     * FVirtualTexture2DResource may have an AllocatedVT, which represents a page table allocation for the virtual texture.
     * VTs used by materials generally don't need their own allocation, since the material has its own page table allocation for each VT stack.
     * VTs used as lightmaps need their own allocation.  Also VTs open in texture editor will have a temporary allocation.
     * GetAllocatedVT() will return the current allocation if one exists.
     * AcquireAllocatedVT() will make a new allocation if needed, and return it.
     * ReleaseAllocatedVT() will free any current allocation.
     */
    class IAllocatedVirtualTexture* GetAllocatedVT() const { return AllocatedVT; }
    class IAllocatedVirtualTexture* AcquireAllocatedVT();
    void ReleaseAllocatedVT();

    EPixelFormat GetFormat(uint32 LayerIndex) const;
    int GetNumBlocks() const;
    FIntPoint GetSizeInBlocks() const;
    uint32 GetNumTilesX() const;
    uint32 GetNumTilesY() const;
    uint32 GetNumMips() const;
    uint32 GetNumLayers() const;
    uint32 GetTileSize() const; //no borders
    uint32 GetBorderSize() const;
    uint32 GetAllocatedvAddress() const;

    FIntPoint GetPhysicalTextureSize(uint32 LayerIndex) const;

protected:
    /** The FName of the texture asset */
    FName TextureName;
    /** The FName of the texture package for stats */
    FName PackageName;
    /** A hash of the texture asset name */
    uint32 FullNameHash = 0;
    /** Cached sampler config */
    TEnumAsByte<ESamplerFilter> Filter = SF_Bilinear;
    TEnumAsByte<ESamplerAddressMode> AddressU = AM_Wrap;
    TEnumAsByte<ESamplerAddressMode> AddressV = AM_Wrap;
    /** Cached flags for texture creation. */
    ETextureCreateFlags TexCreateFlags = ETextureCreateFlags::None;
    /** Cached owner settings */
    bool bSinglePhysicalSpace = false;
    bool bRequiresSinglePhysicalPool = false;
    EVTProducerPriority VirtualTextureStreamingPriority;
    /** Mip offset */
    int32 FirstMipToUse = 0;

    /**
     * The actual data of the render target (not GPU)
     */
    TWeakPtr<class FOdysseyVirtualRenderTargetData> WeakVTData;

    /** Local allocated VT objects used for editor views etc. */
    class IAllocatedVirtualTexture* AllocatedVT = nullptr;
    FVirtualTextureProducerHandle ProducerHandle;
};
