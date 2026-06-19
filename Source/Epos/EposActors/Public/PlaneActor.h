// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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

private:
    virtual void PostLoad() override;

public:
    /** This will make the plane bigger than the original size viewed by the camera by adding a margin to the plane */
    UPROPERTY()
    float SafeMargin_DEPRECATED { 0.f };

    /** This will rescale the original size of the plane
      * The original size is the one viewed by the camera
      * The scale is applied before safe margin
      * The default value (which means no rescale) is (100%, 100%)
      */
    UPROPERTY()
    FVector2D RelativeScaling_DEPRECATED { 100.f, 100.f };
};
