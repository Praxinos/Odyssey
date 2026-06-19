// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
UScalingComponent::ComputeScaleWithScaleAndMargin( const FVector& iCameraViewSize ) const
{
    FVector size_100 = iCameraViewSize;

    FVector size_scaled = size_100 * FVector( GetRelativeScalingNormalized(), 1.f );

    check( FMath::IsWithinInclusive( GetSafeMarginNormalized(), 0.f, 2.f ) );
    FVector size_with_marging = size_scaled * ( 1.f + GetSafeMarginNormalized() );

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
//
//void
//UScalingComponent::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) //override
//{
//    Super::PostEditChangeProperty( PropertyChangedEvent );
//
//    if( PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED( UScalingComponent, SafeMargin ) )
//    {
//        UpdateToCamera();
//    }
//    if( PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED( UScalingComponent, RelativeScaling ) )
//    {
//        UpdateToCamera();
//    }
//}

//---
//
//const ACineCameraActor*
//UScalingComponent::GuessAttachedCineCamera() const
//{
//    AActor* actor = GetOwner();
//    AActor* parent = actor->GetAttachParentActor();
//    while( parent )
//    {
//        if( parent->IsA<ACineCameraActor>() )
//            return Cast<ACineCameraActor>( parent );
//
//        parent = parent->GetAttachParentActor();
//    }
//
//    return nullptr;
//}
//
//#include "EngineUtils.h"
//
//static
//FVector
//FindNextFreeAnimationLocation( UWorld* iWorld, UClass* iClass, FVector iAnimationLocation, FVector iCameraLocation )
//{
//    FVector next_location = iAnimationLocation;
//
//    TArray<AActor*> existing_animations;
//    //UGameplayStatics::GetAllActorsOfClass( iWorld, AOdysseyAnimationActor::StaticClass(), existing_animations ); // To not include UGameplayStatics
//    for( TActorIterator<AActor> It( iWorld, iClass ); It; ++It )
//        existing_animations.Add( *It );
//
//    auto ExistingAnimationOnLocation = [&existing_animations]( FVector iAnimationLocation )
//        {
//            for( auto existing_animation : existing_animations )
//            {
//                if( existing_animation->GetActorLocation().Equals( iAnimationLocation ) )
//                    return true;
//            }
//
//            return false;
//        };
//
//    while( true )
//    {
//        bool used_location = ExistingAnimationOnLocation( next_location );
//        if( !used_location )
//            break;
//
//        FVector direction = ( iCameraLocation - next_location ).GetSafeNormal();
//        next_location += direction * 0.01f;
//    }
//
//    return next_location;
//}

//void
//UScalingComponent::UpdateToCamera( const ACineCameraActor* iCamera, float iFocusDistance )
//{
//    Camera = iCamera;
//    if( !Camera.IsValid() )
//        return;
//
//    FTransform camera_transform = Camera->GetRootComponent()->GetComponentTransform();
//
//    FVector const CamLocation = camera_transform.GetLocation();
//    FVector const CamDir = camera_transform.GetRotation().Vector();
//
//    FVector animation_location = CamLocation + CamDir * iFocusDistance;
//    animation_location = FindNextFreeAnimationLocation( Camera->GetWorld(), GetOwner()->GetClass(), animation_location, CamLocation );
//
//    //SetRelativeLocation( animation_location );
//    GetOwner()->SetActorLocation( animation_location );
//
//    //---
//
//    // Need to be after the location as it use the distance between camera and this component
//
//    UpdateToCamera();
//}
//
//void
//UScalingComponent::UpdateToCamera()
//{
//    const ACineCameraActor* camera = GuessAttachedCineCamera();
//    if( !camera )
//        return;
//
//    FTransform camera_transform = camera->GetRootComponent()->GetComponentTransform();
//
//    FVector const CamLocation = camera_transform.GetLocation();
//    FVector const CamDir = camera_transform.GetRotation().Vector();
//    FRotator const CamRot = camera_transform.Rotator();
//
//    //-
//
//    float distance = FVector::Distance( camera->GetActorLocation(), GetOwner()->GetActorLocation() );
//
//    FVector camera_view_size = ComputeSizeOfCameraView( camera, distance );
//    //camera_view_size.X = FMath::Min( camera_view_size.X, camera_view_size.Y );
//    //camera_view_size.Y = FMath::Min( camera_view_size.X, camera_view_size.Y );
//    FVector animation_scale = ComputeScaleWithScaleAndMargin( camera_view_size );
//
//    FRotator animation_rotator = CamRot;
//
//    //---
//
//    //SetRelativeScale3D( animation_scale );
//    ////GetOwner()->SetActorRotation( FRotator( 0.f, 90.f, 90.f ) ); // Done during actor creation
//    //SetWorldRotation( animation_rotator );
//
//    GetOwner()->SetActorScale3D( animation_scale );
//    GetOwner()->SetActorRotation( FRotator( 0.f, 90.f, 90.f ) ); //TODO: should be computed via VectorUp and VectorFace ?
//    GetOwner()->AddActorWorldRotation( animation_rotator );
//}

//---

void
UScalingComponent::SetSafeMargin( float iMargin )
{
    SafeMargin = FMath::Clamp( iMargin, 0.f, 200.f );

    //UpdateToCamera();
}

float
UScalingComponent::GetSafeMargin() const
{
    return SafeMargin;
}

float
UScalingComponent::GetSafeMarginNormalized() const
{
    return SafeMargin / 100.f;
}

void
UScalingComponent::SetRelativeScaling( FVector2D iRelativeScaling )
{
    RelativeScaling = FVector2D::Clamp( iRelativeScaling, FVector2D::ZeroVector, FVector2D::UnitVector * 200.f );

    //UpdateToCamera();
}

FVector2D
UScalingComponent::GetRelativeScaling() const
{
    return RelativeScaling;
}

FVector2D
UScalingComponent::GetRelativeScalingNormalized() const
{
    return RelativeScaling / 100.f;
}
