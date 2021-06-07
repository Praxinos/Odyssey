// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"

#include "PlaneActor.generated.h"

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
};
