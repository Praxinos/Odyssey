// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "TextureResource.h"
#include "VirtualTexturing.h"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorTiledViewportClient
class FOdysseyVirtualRenderTargetResource
    : public FVirtualTexture2DResource
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

    int GetNumBlocks() const;
    virtual EPixelFormat GetFormat(uint32 LayerIndex) const override;
    virtual FIntPoint GetSizeInBlocks() const override;
    virtual uint32 GetNumTilesX() const override;
    virtual uint32 GetNumTilesY() const override;
    virtual uint32 GetNumMips() const override;
    virtual uint32 GetNumLayers() const override;
    virtual uint32 GetTileSize() const override; //no borders
    virtual uint32 GetBorderSize() const override;


protected:
    /**
     * The actual data of the render target (not GPU)
     */
    TWeakPtr<class FOdysseyVirtualRenderTargetData> WeakVTData;
};
