// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"

#include "PlaneActor.generated.h"

class ACineCameraActor;

UCLASS()
class EPOSACTORS_API APlaneActor
    : public AStaticMeshActor
{
    GENERATED_BODY()

public:
    APlaneActor( const FObjectInitializer& ObjectInitializer );

public:
    virtual void BeginPlay() override;

    virtual void Tick( float iDeltaSeconds ) override;

public:
    virtual FVector ComputePlaneScaleOfCameraView( const ACineCameraActor* iCamera, float iDistance ) const; // From FDrawFrustumSceneProxy::GetDynamicMeshElements()
    virtual FVector ComputePlaneScaleWithScaleAndMargin( const ACineCameraActor* iCamera, float iDistance ) const;

    virtual FIntPoint ComputeTextureSize( const ACineCameraActor* iCamera, int32 iTextureHeight ) const;

public:
    /** Get the margin as percent (0.1 for 10%) */
    virtual float GetSafeMargin() const;
    /** Get the relative scaling as percent (1. for 100%) */
    virtual FVector2D GetRelativeScaling() const;

public:
    /** This will make the plane bigger than the original size viewed by the camera by adding a margin to the plane */
    UPROPERTY( EditAnywhere, Category="Plane Actor", meta=(UIMin = "0", ClampMin = "0", UIMax = "25", ClampMax = "25", Units=Percent) )
    float SafeMargin { 0.f };

    /** This will rescale the original size of the plane
      * The original size is the one viewed by the camera
      * The scale is applied before safe margin
      * The default value (which means no rescale) is (100%, 100%)
      */
    UPROPERTY( EditAnywhere, Category="Plane Actor", meta=(AllowPreserveRatio) )
    FVector2D RelativeScaling { 100.f, 100.f };
};
