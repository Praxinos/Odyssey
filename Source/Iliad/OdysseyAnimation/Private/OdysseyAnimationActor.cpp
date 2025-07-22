// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationActor.h"

#include "Materials/MaterialInstanceConstant.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationPlayer.h"

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

FName AOdysseyAnimationActor::AnimationComponentName( TEXT( "AnimationComponent" ) );

AOdysseyAnimationActor::AOdysseyAnimationActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    AnimationComponent = CreateDefaultSubobject<UOdysseyAnimationComponent>( AnimationComponentName );
    AnimationComponent->SetRelativeRotation( FRotator( 0, 90, 90 ) );

    SetRootComponent( AnimationComponent );
}

#if WITH_EDITOR
bool AOdysseyAnimationActor::GetReferencedContentObjects(TArray<UObject*>& Objects) const
{
    Super::GetReferencedContentObjects( Objects );

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
