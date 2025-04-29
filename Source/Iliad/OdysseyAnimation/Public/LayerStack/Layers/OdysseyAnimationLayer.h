// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"
#include "OdysseyAnimationLayerStack.h"

#include "OdysseyAnimationLayer.generated.h"

UCLASS(Abstract, BlueprintType, HideDropdown)
class ODYSSEYANIMATION_API UOdysseyAnimationLayer
    : public UOdysseyLayer
{
    GENERATED_BODY()

public:

    virtual UOdysseyAnimationLayerStack* GetLayerStack() const;

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    UOdysseyAnimation* GetAnimation() const;

#if WITH_EDITOR
protected:
    friend class FOdysseyAnimationCellsContainerImport;
#endif
};
