// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#include "ScalingComponent.h"

#include "CineCameraActor.h"
#include "CineCameraComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ScalingComponent)

//////////////////////////////////////////////////////////////////////////
// UScalingComponent

UScalingComponent::UScalingComponent( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
}

//---

//static
FVector
UScalingComponent::ComputeSizeOfCameraView( const ACineCameraActor* iCamera, float iDistance ) const
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
UScalingComponent::ComputeScaleWithScaleAndMargin( const FVector& iCameraViewSize ) const
{
    FVector size_100 = iCameraViewSize;

    FVector size_scaled = size_100 * FVector( GetRelativeScaling(), 1.f );

    check( FMath::IsWithinInclusive( GetSafeMargin(), 0.f, 2.f ) );
    FVector size_with_marging = size_scaled * ( 1.f + GetSafeMargin() );

    return size_with_marging;
}

FIntPoint
UScalingComponent::ComputeTextureSize( const FVector& iCameraViewSize, int32 iTextureHeight ) const
{
    FVector size = iCameraViewSize;

    float ratio = size.X / size.Y;

    //-

    int32 width = int32( iTextureHeight * ratio );
    if( width % 4 )
        width += ( 4 - width % 4 ); // To always have a multiple of 4 (like the height)

    width = FMath::Clamp( width, 16, 8192 );

    return FIntPoint( width, iTextureHeight );
}

//---

float
UScalingComponent::GetSafeMargin() const
{
    return SafeMargin / 100.f;
}

FVector2D
UScalingComponent::GetRelativeScaling() const
{
    return RelativeScaling / 100.f;
}
