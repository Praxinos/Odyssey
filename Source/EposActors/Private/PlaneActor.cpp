// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "PlaneActor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

#define LOCTEXT_NAMESPACE "PlaneActor"

//---

APlaneActor::APlaneActor( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
    SetActorHiddenInGame( true );

    UStaticMesh* plane_mesh = LoadObject<UStaticMesh>( nullptr, TEXT( "/Epos/S_1_Unit_Plane.S_1_Unit_Plane" ) );
    check( plane_mesh );
    GetStaticMeshComponent()->SetStaticMesh( plane_mesh );
}

void
APlaneActor::BeginPlay() //override
{
    Super::BeginPlay();
}

void
APlaneActor::Tick( float iDeltaSeconds ) //override
{
    Super::Tick( iDeltaSeconds );
}

//---

#undef LOCTEXT_NAMESPACE
