// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyLayerCell.h"

#include "OdysseyAnimationCell.generated.h"

class UOdysseyAnimation;

UCLASS(Abstract, BlueprintType, HideDropdown)
class ODYSSEYANIMATION_API UOdysseyAnimationCell
    : public UOdysseyLayerCell
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyAnimation* GetAnimation() const;
};
