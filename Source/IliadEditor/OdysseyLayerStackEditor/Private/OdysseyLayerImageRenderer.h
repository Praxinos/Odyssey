// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderer.h"
#include <ULIS>

class UOdysseyLayer;

class FOdysseyLayerImageRenderer
    : public IOdysseyImageRenderer
{
public:
    FOdysseyLayerImageRenderer(const UOdysseyLayer* iLayer, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects, FImageRendererFilter iFilter);

public:
    virtual void Init() override;
    virtual bool IsGameThreadOnly() override;
    virtual TArray<::ULIS::FEvent> Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;
    virtual TArray<::ULIS::FEvent> Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;

public:
    struct FChildData
    {
        TSharedPtr<IOdysseyImageRenderer> mRenderer;
        ::ULIS::eBlendMode mBlendMode;
        float mOpacity;
    };
    TArray<FChildData> mChildrenData;
};
