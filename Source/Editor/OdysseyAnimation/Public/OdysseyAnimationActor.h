// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "OdysseyAnimationActor.generated.h"

class UOdysseyAnimationComponent;

/**
 * An instance of a UOdysseyAnimation in a level.
 *
 * This actor is created when you drag a animation asset from the content browser into the level, and
 * it is just a thin wrapper around a UOdysseyAnimationComponent that actually references the asset.
 */
UCLASS(ComponentWrapperClass, meta=(PrioritizeCategories="Actions"))
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
    UFUNCTION(Category="Actions", CallInEditor)
    void Play();

    UFUNCTION(Category="Actions", CallInEditor)
    void Stop();

    UFUNCTION(Category="Actions", CallInEditor)
    void FaceCamera();

    UFUNCTION(Category="Actions", CallInEditor)
    void MoveInFrontOfCamera();

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation")
    TObjectPtr<UOdysseyAnimationComponent> AnimationComponent;
};
