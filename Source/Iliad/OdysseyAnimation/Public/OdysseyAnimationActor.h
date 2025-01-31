// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "GameFramework/Actor.h"
#include "OdysseyAnimationActor.generated.h"

class ACineCameraActor;
class UOdysseyAnimationComponent;

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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation")
    TObjectPtr<UOdysseyAnimationComponent> AnimationComponent;

    static FName AnimationComponentName;

public:
    virtual FVector ComputeAnimationScaleOfCameraView( const ACineCameraActor* iCamera, float iDistance ) const; // From FDrawFrustumSceneProxy::GetDynamicMeshElements()
    virtual FVector ComputeAnimationScaleWithScaleAndMargin( const ACineCameraActor* iCamera, float iDistance ) const;

    virtual FIntPoint ComputeTextureSize( const ACineCameraActor* iCamera, int32 iTextureHeight ) const;

public:
    /** Get the margin as percent (0.1 for 10%) */
    virtual float GetSafeMargin() const;
    /** Get the relative scaling as percent (1. for 100%) */
    virtual FVector2D GetRelativeScaling() const;

public:
    /** This will make the animation bigger than the original size viewed by the camera by adding a margin to the animation */
    UPROPERTY( EditAnywhere, Category = "Animation Actor", meta = ( UIMin = "0", ClampMin = "0", UIMax = "200", ClampMax = "200", Units = Percent ) )
    float SafeMargin = 0.f;

    /** This will rescale the original size of the animation
      * The original size is the one viewed by the camera
      * The scale is applied before safe margin
      * The default value (which means no rescale) is (100%, 100%)
      */
    UPROPERTY( EditAnywhere, Category = "Animation Actor", meta = ( AllowPreserveRatio ) )
    FVector2D RelativeScaling = { 100.f, 100.f };
};
