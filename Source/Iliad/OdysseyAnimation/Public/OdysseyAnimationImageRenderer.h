// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderer.h"
#include <ULIS>

class UOdysseyAnimation;

class ODYSSEYANIMATION_API FOdysseyAnimationImageRenderer
    : public IOdysseyImageRenderer
{
public:
    FOdysseyAnimationImageRenderer(const UOdysseyAnimation* iAnimation, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI> iDefaultRects, FImageRendererFilter iFilter = FImageRendererFilter());

public:
    virtual void Init() override;
    virtual bool IsGameThreadOnly() override;
    virtual TArray<::ULIS::FEvent> Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;
    virtual TArray<::ULIS::FEvent> Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;

public:
    TSharedPtr<IOdysseyImageRenderer> mLayerStackRenderer;
};
