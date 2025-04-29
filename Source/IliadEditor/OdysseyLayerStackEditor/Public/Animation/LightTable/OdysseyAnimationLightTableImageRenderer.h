// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderer.h"

class UOdysseyAnimationLayer;

class ODYSSEYLAYERSTACKEDITOR_API FOdysseyAnimationLightTableImageRenderer
    : public IOdysseyImageRenderer
{
public:
    FOdysseyAnimationLightTableImageRenderer(const UOdysseyAnimationLayer* iLayer, int iFrame, EOdysseyRenderingType iRenderType, const TArray<FIntRect>& iDefaultRects, FImageRendererFilter iFilter);

public:
    virtual void Init() override;
    virtual bool IsGameThreadOnly() override;
    virtual TArray<::ULIS::FEvent> Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;
    virtual TArray<::ULIS::FEvent> Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;

public:
    struct FFrameData
    {
        float mOpacity;
        TSharedPtr<IOdysseyImageRenderer> mRenderer;
        ::ULIS::FColor mColor;
        float mContrast;
        ::ULIS::FMat3F mOutOfPegsTransform;
    };
    TArray<FFrameData> mFramesData;
};
