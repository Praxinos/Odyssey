// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyRenderingAbility.h"

class ODYSSEYCORE_API FOdysseyTextureRenderingAbility
    : public FOdysseyRenderingAbility
{
public:
    void RenderToTexture(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame) const;
    virtual void RenderToTextureFromRects(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const TArray<FIntRect>& iRects) const = 0;
};
