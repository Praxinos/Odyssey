// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "GameFramework/Actor.h"
#include "OdysseyAnimationActor.generated.h"

class ACineCameraActor;
class UOdysseyAnimationComponent;
class UScalingComponent;

/**
 * An instance of a UOdysseyAnimation in a level.
 *
 * This actor is created when you drag a animation asset from the content browser into the level, and
 * it is just a thin wrapper around a UOdysseyAnimationComponent that actually references the asset.
 */
UCLASS(HideCategories=(Materials), meta=(PrioritizeCategories="Actions"))
class ODYSSEYANIMATION_API AOdysseyAnimationActor : public AActor
{
    GENERATED_UCLASS_BODY()

public:
    // AActor interface
#if WITH_EDITOR
    virtual bool GetReferencedContentObjects(TArray<UObject*>& Objects) const override;
#endif
    // End of AActor interface

public:
    UOdysseyAnimationComponent*         GetAnimationComponent();
    const UOdysseyAnimationComponent*   GetAnimationComponent() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation")
    TObjectPtr<UOdysseyAnimationComponent> AnimationComponent;

    static FName AnimationComponentName;
};
