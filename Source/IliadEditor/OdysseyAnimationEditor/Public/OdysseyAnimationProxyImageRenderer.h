// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderer.h"

#include <ULIS>

class FOdysseyAnimationProxy;
class UOdysseyAnimationLayerRoot;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationProxyImageRenderer
    : public IOdysseyImageRenderer
{
public:
    FOdysseyAnimationProxyImageRenderer(const UOdysseyAnimationLayerRoot* iLayer, int iFrameIndex, EOdysseyRenderingType iRenderType, const TArray<FIntRect> iDefaultRects, FImageRendererFilter iFilter);

public:
    virtual void Init() override;
    virtual bool IsGameThreadOnly() override;

public:
    virtual TArray<::ULIS::FEvent> Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;
    virtual TArray<::ULIS::FEvent> Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;

public:
    TSharedPtr<FOdysseyAnimationProxy> mProxy;
    int mFrameIndex;
    TSharedPtr<IOdysseyImageRenderer> mLayerRenderer;
    TSharedPtr<::ULIS::FBlock> mBlock;
    bool mForceRender;
};
