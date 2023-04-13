// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAbility.h"

class ODYSSEYANIMATION_API FOdysseyAnimationImageRasterEditingAbility
    : public IOdysseyAbility
{
public:
    static const FGuid& Id()
    {
        static FGuid Id(0xe68d1198, 0xff2f49a6, 0x9c84fa33, 0x0d1428df);
        return Id;
    }

public:
    virtual TSharedPtr<FOdysseyRasterBlock> GetRasterBlock(uint32 iFrameIndex) const = 0;
};