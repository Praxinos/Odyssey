// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderer.h"

class FOdysseyAnimationProxy;

class ODYSSEYANIMATION_API FOdysseyAnimationProxyImageRenderer
    : public IOdysseyImageRenderer
{
public:
    FOdysseyAnimationProxyImageRenderer(const UOdysseyAnimation* iAnimation, int iFrameIndex, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI> iDefaultRects);

public:
    virtual void Init() override;
    virtual bool IsGameThreadOnly() override;

public:
    virtual TArray<::ULIS::FEvent> Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList) override;
    virtual TArray<::ULIS::FEvent> Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList) override;

public:
    TSharedPtr<FOdysseyAnimationProxy> mProxy;
    int mFrameIndex;
    TSharedPtr<IOdysseyImageRenderer> mAnimationRenderer;
    TSharedPtr<::ULIS::FBlock> mBlock;
};