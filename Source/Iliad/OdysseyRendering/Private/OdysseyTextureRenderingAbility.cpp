// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyTextureRenderingAbility.h"
#include "CanvasTypes.h"

#define LOCTEXT_NAMESPACE "Imaging"

void
IOdysseyTextureRenderingAbility::Render_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame) const
{
    FIntRect defaultRect = GetDefaultRenderRect();
    FTextureRenderTargetResource* dstResource = iRenderTarget->GameThread_GetRenderTargetResource();
    FCanvas Canvas(dstResource, nullptr, FGameTime(), GMaxRHIFeatureLevel);
    RenderToTexture(&Canvas, iFrame, GetDefaultRenderRect(), FIntRect(0, 0, defaultRect.Width(), defaultRect.Height()));
}

void
IOdysseyTextureRenderingAbility::RenderRect_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const FIntRect& iSrcRect) const
{
    FTextureRenderTargetResource* dstResource = iRenderTarget->GameThread_GetRenderTargetResource();
    FCanvas Canvas(dstResource, nullptr, FGameTime(), GMaxRHIFeatureLevel);
    RenderToTexture(&Canvas, iFrame, iSrcRect, FIntRect(0, 0, iSrcRect.Width(), iSrcRect.Height()));
}

void
IOdysseyTextureRenderingAbility::RenderRectAtPosition_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const FIntRect& iSrcRect, const FIntPoint& iPos) const
{
    FTextureRenderTargetResource* dstResource = iRenderTarget->GameThread_GetRenderTargetResource();
    FCanvas Canvas(dstResource, nullptr, FGameTime(), GMaxRHIFeatureLevel);
    RenderToTexture(&Canvas, iFrame, iSrcRect, FIntRect(iPos.X, iPos.Y, iPos.X + iSrcRect.Width(), iPos.Y + iSrcRect.Height()));
}

void
IOdysseyTextureRenderingAbility::RenderRectAtRect_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const FIntRect& iSrcRect, const FIntRect& iDstRect) const
{
    FTextureRenderTargetResource* dstResource = iRenderTarget->GameThread_GetRenderTargetResource();
    FCanvas Canvas(dstResource, nullptr, FGameTime(), GMaxRHIFeatureLevel);
    RenderToTexture(&Canvas, iFrame, iSrcRect, iDstRect);
}

#undef LOCTEXT_NAMESPACE
