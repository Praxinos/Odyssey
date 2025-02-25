// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#pragma once

#include "Components/ActorComponent.h"

#include "ScalingComponent.generated.h"

class ACineCameraActor;

/**
 * A component containing management of actor scaling from camera
 */
UCLASS()
class EPOSACTORS_API UScalingComponent
    : public USceneComponent
{
    GENERATED_UCLASS_BODY()

public:
    virtual FVector ComputeSizeOfCameraView( const ACineCameraActor* iCamera, float iDistance ) const; // From FDrawFrustumSceneProxy::GetDynamicMeshElements()
    virtual FVector ComputeScaleWithScaleAndMargin( const FVector& iCameraViewSize ) const;

    virtual FIntPoint ComputeTextureSize( const FVector& iCameraViewSize, int32 iTextureHeight ) const;

public:
    /** Get the margin as percent (0.1 for 10%) */
    virtual float GetSafeMargin() const;
    /** Get the relative scaling as percent (1. for 100%) */
    virtual FVector2D GetRelativeScaling() const;

public:
    /** This will make the actor bigger than the original size viewed by the camera by adding a margin to the actor */
    UPROPERTY( EditAnywhere, Category="Scaling", meta=(UIMin="0", ClampMin="0", UIMax="200", ClampMax="200", Units=Percent) )
    float SafeMargin = 0.f;

    /** This will rescale the original size of the actor
      * The original size is the one viewed by the camera
      * The scale is applied before safe margin
      * The default value (which means no rescale) is (100%, 100%)
      */
    UPROPERTY( EditAnywhere, Category="Scaling", meta=(AllowPreserveRatio) )
    FVector2D RelativeScaling = { 100.f, 100.f };
};
