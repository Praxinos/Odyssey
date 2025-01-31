// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationActor.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationPlayer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationActor)

FName AOdysseyAnimationActor::AnimationComponentName(TEXT("AnimationComponent"));

AOdysseyAnimationActor::AOdysseyAnimationActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    AnimationComponent = CreateDefaultSubobject<UOdysseyAnimationComponent>(AnimationComponentName);
    RootComponent = AnimationComponent;

    AnimationComponent->SetRelativeRotation(FRotator(0, 90, 90));
}

#if WITH_EDITOR
bool AOdysseyAnimationActor::GetReferencedContentObjects(TArray<UObject*>& Objects) const
{
    Super::GetReferencedContentObjects(Objects);

    if (!AnimationComponent)
        return true;

    switch(AnimationComponent->GetMode())
    {
        case EOdysseyAnimationComponentMode::Animation : Objects.Add(AnimationComponent->GetAnimation()); break;
        case EOdysseyAnimationComponentMode::Player : Objects.Add(AnimationComponent->GetPlayer()); break;
    }
    return true;
}
#endif

//---

//static
FVector
AOdysseyAnimationActor::ComputeAnimationScaleOfCameraView( const ACineCameraActor* iCamera, float iDistance ) const
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
AOdysseyAnimationActor::ComputeAnimationScaleWithScaleAndMargin( const ACineCameraActor* iCamera, float iDistance ) const
{
    FVector size_100 = ComputeAnimationScaleOfCameraView( iCamera, iDistance );

    FVector size_scaled = size_100 * FVector( GetRelativeScaling(), 1.f );

    check( FMath::IsWithinInclusive( GetSafeMargin(), 0.f, 2.f ) );
    FVector size_with_marging = size_scaled * ( 1.f + GetSafeMargin() );

    return size_with_marging;
}

FIntPoint
AOdysseyAnimationActor::ComputeTextureSize( const ACineCameraActor* iCamera, int32 iTextureHeight ) const
{
    FVector animation_scale = ComputeAnimationScaleWithScaleAndMargin( iCamera, 200 /* arbitrary as we only need its ratio */ );

    float animation_ratio = animation_scale.X / animation_scale.Y;

    //-

    int32 width = int32( iTextureHeight * animation_ratio );
    if( width % 4 )
        width += ( 4 - width % 4 ); // To always have a multiple of 4 (like the height)

    width = FMath::Clamp( width, 16, 8192 );

    return FIntPoint( width, iTextureHeight );
}

//---

float
AOdysseyAnimationActor::GetSafeMargin() const
{
    return SafeMargin / 100.f;
}

FVector2D
AOdysseyAnimationActor::GetRelativeScaling() const
{
    return RelativeScaling / 100.f;
}

//---
