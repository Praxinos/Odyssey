// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationActor.h"

#include "Materials/MaterialInstanceConstant.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationPlayer.h"
#include "ScalingComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationActor)

UOdysseyAnimationComponent*
AOdysseyAnimationActor::GetAnimationComponent()
{
    return AnimationComponent;
}

const UOdysseyAnimationComponent*
AOdysseyAnimationActor::GetAnimationComponent() const
{
    return const_cast<AOdysseyAnimationActor*>( this )->GetAnimationComponent();
}

UScalingComponent*
AOdysseyAnimationActor::GetScalingComponent()
{
    return ScalingComponent;
}

const UScalingComponent*
AOdysseyAnimationActor::GetScalingComponent() const
{
    return const_cast<AOdysseyAnimationActor*>( this )->GetScalingComponent();
}

FName AOdysseyAnimationActor::AnimationComponentName( TEXT( "AnimationComponent" ) );

AOdysseyAnimationActor::AOdysseyAnimationActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ScalingComponent = CreateDefaultSubobject<UScalingComponent>( TEXT( "Scaling" ) );

    SetRootComponent( ScalingComponent );

    AnimationComponent = CreateDefaultSubobject<UOdysseyAnimationComponent>( AnimationComponentName );
    AnimationComponent->SetRelativeRotation( FRotator( 0, 90, 90 ) );
    AnimationComponent->SetupAttachment( ScalingComponent );
}

#if WITH_EDITOR
bool AOdysseyAnimationActor::GetReferencedContentObjects(TArray<UObject*>& Objects) const
{
    Super::GetReferencedContentObjects( Objects );

    //TODO: also check/return scalingcomponent ? what must be added to Objects ?

    if( !AnimationComponent )
        return true;

    switch(AnimationComponent->GetMode())
    {
        case EOdysseyAnimationComponentMode::Animation : Objects.Add(AnimationComponent->GetAnimation()); break;
        case EOdysseyAnimationComponentMode::Player : Objects.Add(AnimationComponent->GetPlayer()); break;
    }
    return true;
}
#endif
