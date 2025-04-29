// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderer.h"
#include <ULIS>

class FOdysseyRasterBlock;
class UOdysseyTextureLayerImageRaster;

class FOdysseyTextureLayerImageRasterImageRenderer
    : public IOdysseyImageRenderer
{
public:
    FOdysseyTextureLayerImageRasterImageRenderer(const UOdysseyTextureLayerImageRaster* iLayer, EOdysseyRenderingType iRenderType, const TArray<FIntRect>& iDefaultRects, FImageRendererFilter iFilter);

public:
    virtual void Init() override;
    virtual bool IsGameThreadOnly() override;
    virtual TArray<::ULIS::FEvent> Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;
    virtual TArray<::ULIS::FEvent> Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;

public:
    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
    TSharedPtr<::ULIS::FBlock> mBlock;
};
