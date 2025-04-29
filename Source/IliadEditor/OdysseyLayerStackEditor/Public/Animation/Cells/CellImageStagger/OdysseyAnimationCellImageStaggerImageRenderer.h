// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderer.h"
#include "OdysseyRenderingType.h"
#include <ULIS>

class UOdysseyAnimationCellImageStagger;

class ODYSSEYLAYERSTACKEDITOR_API FOdysseyAnimationCellImageStaggerImageRenderer
    : public IOdysseyImageRenderer
{
public:
    FOdysseyAnimationCellImageStaggerImageRenderer(const UOdysseyAnimationCellImageStagger* iCell, int iFrame, EOdysseyRenderingType iRenderType, const TArray<FIntRect>& iDefaultRects, FImageRendererFilter iFilter);

public:
    virtual void Init() override;
    virtual bool IsGameThreadOnly() override;
    virtual TArray<::ULIS::FEvent> Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;
    virtual TArray<::ULIS::FEvent> Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;

private:
    TSharedPtr<IOdysseyImageRenderer> mCellRenderer;
};
