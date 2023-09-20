// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderer.h"

class ODYSSEYANIMATION_API FOdysseyAnimationLayerImageVectorImageRenderer
    : public IOdysseyImageRenderer
{
public:
    FOdysseyAnimationLayerImageVectorImageRenderer(const UOdysseyAnimationLayerImageVector* iLayer, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects);

public:
    virtual void Init() override;
    virtual bool IsGameThreadOnly() override;
    virtual TArray<::ULIS::FEvent> Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList) override;
    virtual TArray<::ULIS::FEvent> Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList) override;

public:
    TSharedPtr<IOdysseyImageRenderer> mCellRenderer;
    ::ULIS::eBlendMode mBlendMode;
    float mOpacity;

    EOdysseyLightTableDisplayPosition mLightTableDisplayPosition;
    TSharedPtr<IOdysseyImageRenderer> mLightTableRenderer;
};
