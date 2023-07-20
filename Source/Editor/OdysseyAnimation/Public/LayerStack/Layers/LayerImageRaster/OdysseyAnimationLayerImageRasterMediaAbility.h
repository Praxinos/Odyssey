// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Abilities/IOdysseyAnimationMediaAbility.h"

class UOdysseyAnimationLayerImageRaster;

class ODYSSEYANIMATION_API FOdysseyAnimationLayerImageRasterMediaAbility
    : public IOdysseyAnimationMediaAbility
{
public:
    FOdysseyAnimationLayerImageRasterMediaAbility(UOdysseyAnimationLayerImageRaster* iLayer);
    
public:
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

private:
    TSharedPtr<IOdysseyMedia> CreateMediaRaster(int iFrameIndex) const;
    void AutoCreateCell(int iFrameIndex) const;
    TSharedPtr<IOdysseyMedia> GetCellMediaRaster(uint32 iFrameIndex) const;

private:
    UOdysseyAnimationLayerImageRaster* mLayer;
};