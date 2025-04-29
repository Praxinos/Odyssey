// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyTextureRenderingAbility.h"

#define LOCTEXT_NAMESPACE "Imaging"

void
IOdysseyTextureRenderingAbility::Render_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame) const
{
    RenderToTextureFromRects(iRenderTarget, iFrame, GetRenderingRects(), FIntPoint(0, 0));
}

void
IOdysseyTextureRenderingAbility::RenderRects_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const TArray<FIntRect>& iRects) const
{
    RenderToTextureFromRects(iRenderTarget, iFrame, iRects, FIntPoint(0, 0));
}

void
IOdysseyTextureRenderingAbility::RenderRectsAtPosition_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const TArray<FIntRect>& iRects, const FIntPoint& iPos) const
{
    RenderToTextureFromRects(iRenderTarget, iFrame, iRects, iPos);
}

#undef LOCTEXT_NAMESPACE
