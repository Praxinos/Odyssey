// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderer.h"

#include <ULIS>

class UOdysseyTextureLayerImageVector;
class FOdysseyVectorBlock;

class FOdysseyTextureLayerImageVectorImageRenderer
    : public IOdysseyImageRenderer
{
public:
    FOdysseyTextureLayerImageVectorImageRenderer(const UOdysseyTextureLayerImageVector* iLayer, TSharedPtr<FOdysseyVectorBlock> iVectorBlock, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects, FImageRendererFilter iFilter);

public:
    virtual void Init() override;
    virtual bool IsGameThreadOnly() override;
    virtual TArray<::ULIS::FEvent> Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;
    virtual TArray<::ULIS::FEvent> Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;

public:
    TSharedPtr<FOdysseyVectorBlock> mVectorBlock;
    TSharedPtr<::ULIS::FBlock> mBlock;
    TSharedPtr<::ULIS::FBlock> mHUDBlock;
    uint64 mDrawingFlags;
    bool mRenderHUD;
};
