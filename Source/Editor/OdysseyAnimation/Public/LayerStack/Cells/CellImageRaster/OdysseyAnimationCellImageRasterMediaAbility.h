// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Abilities/IOdysseyAnimationMediaAbility.h"

class UOdysseyAnimationCellImageRaster;

class ODYSSEYANIMATION_API FOdysseyAnimationCellImageRasterMediaAbility
    : public IOdysseyAnimationMediaAbility
{
public:
    FOdysseyAnimationCellImageRasterMediaAbility(TSharedPtr<FOdysseyAnimationCellImageRaster> iCellImageRaster);
    
public:
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

private:
    TWeakPtr<FOdysseyAnimationCellImageRaster> mCellImageRaster;
};