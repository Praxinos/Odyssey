// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"

#include "UObject/OdysseyObjectPropertyTracker.h"
#include "Misc/OdysseyHandle.h"
#include <ULIS>

#include "OdysseyAnimationLayer.generated.h"

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayer
    : public UOdysseyLayer
    , public FOdysseyAbilityContainer
{
    GENERATED_BODY()
public:
    //Getters
    UOdysseyAnimation* GetAnimation();
    virtual FInt32Range GetFrameRange() const;
    
protected:
    //Property changes
    virtual void ChildrenChanged() override;
};
