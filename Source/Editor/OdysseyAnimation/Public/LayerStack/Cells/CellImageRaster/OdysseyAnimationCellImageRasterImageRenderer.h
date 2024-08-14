// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderer.h"
#include <ULIS>

class FOdysseyAnimationCellImageRaster;

class ODYSSEYANIMATION_API FOdysseyAnimationCellImageRasterImageRenderer
    : public IOdysseyImageRenderer
{
public:
    FOdysseyAnimationCellImageRasterImageRenderer(UOdysseyAnimationCellImageRaster* iCell, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects, FImageRendererFilter iFilter);

public:
    virtual void Init() override;
    virtual void Lock() override;
    virtual void Unlock() override;
    virtual bool IsGameThreadOnly() override;

public:
    virtual TArray<::ULIS::FEvent> Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;
    virtual TArray<::ULIS::FEvent> Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;

public:
    TStrongObjectPtr<UOdysseyAnimationCellImageRaster> mCell;
    TSharedPtr<::ULIS::FBlock> mBlock;
    ::ULIS::FMat3F mOutOfPegsTransform;
};
