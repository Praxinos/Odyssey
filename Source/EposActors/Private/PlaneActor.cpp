// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "PlaneActor.h"

#include "Components/StaticMeshComponent.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
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

//static
FVector
APlaneActor::ComputePlaneScaleOfCameraView( const ACineCameraActor* iCamera, float iDistance ) const
{
    float FrustumAngle = iCamera->GetCineCameraComponent()->GetHorizontalFieldOfView();
    float FrustumAspectRatio = iCamera->GetCineCameraComponent()->AspectRatio;
    float FrustumEndDist = iDistance;

    //---

    FVector Direction( 1, 0, 0 );
    FVector LeftVector( 0, 1, 0 );
    FVector UpVector( 0, 0, 1 );

    FVector Verts[8];

    // FOVAngle controls the horizontal angle.
    const float HozHalfAngleInRadians = FMath::DegreesToRadians( FrustumAngle * 0.5f );

    float HozLength = 0.0f;
    float VertLength = 0.0f;

    //if( FrustumAngle > 0.0f )
    //{
    //    HozLength = FrustumStartDist * FMath::Tan( HozHalfAngleInRadians );
    //    VertLength = HozLength / FrustumAspectRatio;
    //}
    //else
    //{
    //    const float OrthoWidth = ( FrustumAngle == 0.0f ) ? 1000.0f : -FrustumAngle;
    //    HozLength = OrthoWidth * 0.5f;
    //    VertLength = HozLength / FrustumAspectRatio;
    //}

    //// near plane verts
    //Verts[0] = ( Direction * FrustumStartDist ) + ( UpVector * VertLength ) + ( LeftVector * HozLength );
    //Verts[1] = ( Direction * FrustumStartDist ) + ( UpVector * VertLength ) - ( LeftVector * HozLength );
    //Verts[2] = ( Direction * FrustumStartDist ) - ( UpVector * VertLength ) - ( LeftVector * HozLength );
    //Verts[3] = ( Direction * FrustumStartDist ) - ( UpVector * VertLength ) + ( LeftVector * HozLength );

    if( FrustumAngle > 0.0f )
    {
        HozLength = FrustumEndDist * FMath::Tan( HozHalfAngleInRadians );
        VertLength = HozLength / FrustumAspectRatio;
    }

    // far plane verts
    Verts[4] = ( Direction * FrustumEndDist ) + ( UpVector * VertLength ) + ( LeftVector * HozLength );
    Verts[5] = ( Direction * FrustumEndDist ) + ( UpVector * VertLength ) - ( LeftVector * HozLength );
    Verts[6] = ( Direction * FrustumEndDist ) - ( UpVector * VertLength ) - ( LeftVector * HozLength );
    Verts[7] = ( Direction * FrustumEndDist ) - ( UpVector * VertLength ) + ( LeftVector * HozLength );

    //for( int32 X = 0; X < 8; ++X )
    //{
    //    Verts[X] = GetLocalToWorld().TransformPosition( Verts[X] );
    //}

    float norm_x = FVector::Distance( Verts[4], Verts[5] );
    float norm_y = FVector::Distance( Verts[4], Verts[7] );

    //---

    return FVector( norm_x, norm_y, 1.f );
}

//static
FVector
APlaneActor::ComputePlaneScaleWithScaleAndMargin( const ACineCameraActor* iCamera, float iDistance ) const
{
    FVector size_100 = ComputePlaneScaleOfCameraView( iCamera, iDistance );

    FVector size_scaled = size_100 * FVector( GetRelativeScaling(), 1.f );

    check( FMath::IsWithinInclusive( GetSafeMargin(), 0.f, 1.f ) );
    FVector size_with_marging = size_scaled * ( 1.f + GetSafeMargin() );

    return size_with_marging;
}

FIntPoint
APlaneActor::ComputeTextureSize( const ACineCameraActor* iCamera, int32 iTextureHeight ) const
{
    FVector plane_scale = ComputePlaneScaleWithScaleAndMargin( iCamera, 200 /* arbitrary as we only need its ratio */ );

    float plane_ratio = plane_scale.X / plane_scale.Y;

    //-

    int32 width = int32( iTextureHeight * plane_ratio );
    if( width % 4 )
        width += ( 4 - width % 4 ); // To always have a multiple of 4 (like the height)

    width = FMath::Clamp( width, 16, 8192 );

    return FIntPoint( width, iTextureHeight );
}

//---

float
APlaneActor::GetSafeMargin() const
{
    return SafeMargin / 100.f;
}

FVector2D
APlaneActor::GetRelativeScaling() const
{
    return RelativeScaling / 100.f;
}

//---

#undef LOCTEXT_NAMESPACE
