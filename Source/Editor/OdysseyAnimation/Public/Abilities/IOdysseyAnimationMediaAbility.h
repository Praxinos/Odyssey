// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAbility.h"
#include "OdysseyMediaProvider.h"

class ODYSSEYANIMATION_API IOdysseyAnimationMediaAbility
    : public IOdysseyAbility
{
public:
    static const FGuid& Id()
    {
        static FGuid Id(0x684cd251, 0x56784af1, 0xb9049a84, 0xb3d43979);
        return Id;
    }

public:
    DECLARE_MULTICAST_DELEGATE(FOnChanged);
    static FOnChanged& OnChanged();

public:
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const = 0;
};