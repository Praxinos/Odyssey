// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Abilities/IOdysseyAnimationImageRasterEditingAbility.h"

class ODYSSEYANIMATION_API FOdysseyAnimationCellImageRasterImageRasterEditingAbility
    : public IOdysseyAnimationImageRasterEditingAbility
{
public:
    virtual ~FOdysseyAnimationCellImageRasterImageRasterEditingAbility() {}
    FOdysseyAnimationCellImageRasterImageRasterEditingAbility(TSharedPtr<FOdysseyAnimationCellImageRaster> iCellImageRaster);

public:
    virtual TSharedPtr<FOdysseyRasterBlock> GetRasterBlock(uint32 iFrameIndex) const override;

private:
    TWeakPtr<FOdysseyAnimationCellImageRaster> mCellImageRaster;
};