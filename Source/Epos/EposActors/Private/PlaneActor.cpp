// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "PlaneActor.h"

#include "Components/StaticMeshComponent.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Engine/StaticMesh.h"

#include "ScalingComponent.h"

#define LOCTEXT_NAMESPACE "PlaneActor"

//---

APlaneActor::APlaneActor( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
    SetActorHiddenInGame( true );

    UStaticMesh* plane_mesh = LoadObject<UStaticMesh>( nullptr, TEXT( "/Odyssey/Meshes/S_1_Unit_Plane.S_1_Unit_Plane" ) );
    check( plane_mesh );
    GetStaticMeshComponent()->SetStaticMesh( plane_mesh );

    GetStaticMeshComponent()->SetRelativeRotation( FRotator( 0, 90, 90 ) );
}

void
APlaneActor::PostLoad()
{
    Super::PostLoad();

    auto CreateMissingScalingComponentIfNeeded = [this]() -> UScalingComponent*
        {
            UScalingComponent* scaling_component = FindComponentByClass<UScalingComponent>();
            if( !scaling_component )
            {
                UScalingComponent* actor_component = NewObject<UScalingComponent>( this, UScalingComponent::StaticClass(), FName( "Scaling" ), RF_Transactional );
                FinishAddComponent( actor_component, false, FTransform::Identity );
                AddInstanceComponent( actor_component );
                check( actor_component );
                scaling_component = Cast<UScalingComponent>( actor_component );
                check( scaling_component && FindComponentByClass<UScalingComponent>() );
            }

            return scaling_component;
        };

    if( !FMath::IsNearlyEqual( SafeMargin_DEPRECATED, -1.f ) )
    {
        UScalingComponent* scaling_component = CreateMissingScalingComponentIfNeeded();

        scaling_component->SetSafeMargin( SafeMargin_DEPRECATED );
        SafeMargin_DEPRECATED = -1.f;
    }

    if( !RelativeScaling_DEPRECATED.Equals( -FVector2D::UnitVector ) )
    {
        UScalingComponent* scaling_component = CreateMissingScalingComponentIfNeeded();

        scaling_component->SetRelativeScaling( RelativeScaling_DEPRECATED );
        RelativeScaling_DEPRECATED = -FVector2D::UnitVector;
    }
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
