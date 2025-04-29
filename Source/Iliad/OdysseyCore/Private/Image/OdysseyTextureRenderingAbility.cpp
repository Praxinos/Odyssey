// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyTextureRenderingAbility.h"

#define LOCTEXT_NAMESPACE "Imaging"

void
FOdysseyTextureRenderingAbility::RenderToTexture(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame) const
{
    RenderToTextureFromRects(iRenderTarget, iFrame, GetRenderingRects());
}

#undef LOCTEXT_NAMESPACE
