// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationActor.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationPlayer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Subsystems/UnrealEditorSubsystem.h"
#include "Editor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationActor)

//////////////////////////////////////////////////////////////////////////
// AOdysseyAnimationActor

void
AOdysseyAnimationActor::Play()
{
    if (AnimationComponent)
        AnimationComponent->Play();
}

void
AOdysseyAnimationActor::Stop()
{
    if (AnimationComponent)
        AnimationComponent->Stop();
}

void
AOdysseyAnimationActor::FaceCamera()
{
    UUnrealEditorSubsystem* UnrealEditorSubsystem = GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>();
    FVector cameraLocation;
    FRotator cameraRotation;
    if (!UnrealEditorSubsystem->GetLevelViewportCameraInfo(cameraLocation, cameraRotation))
        return;

    //-

    FRotator plane_rotator = cameraRotation;

    //---

    SetActorRotation( plane_rotator );
}

void
AOdysseyAnimationActor::MoveInFrontOfCamera()
{
    UUnrealEditorSubsystem* UnrealEditorSubsystem = GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>();
    FVector cameraLocation;
    FRotator cameraRotation;
    if (!UnrealEditorSubsystem->GetLevelViewportCameraInfo(cameraLocation, cameraRotation))
        return;

    //-

    float FocusDistance = 200;
    FVector plane_location = cameraLocation + cameraRotation.Vector() * FocusDistance;
    FRotator plane_rotator = cameraRotation;

    //---

    SetActorLocation( plane_location );
    SetActorRotation( plane_rotator );
}

FName AOdysseyAnimationActor::AnimationComponentName(TEXT("AnimationComponentName0"));

AOdysseyAnimationActor::AOdysseyAnimationActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    RootComponent = AnimationComponent = CreateDefaultSubobject<UOdysseyAnimationComponent>(AnimationComponentName);
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
        case EOdysseyAnimationComponentMode::Animation : Objects.Add(AnimationComponent->GetActiveAnimation()); break;
        case EOdysseyAnimationComponentMode::Player : Objects.Add(AnimationComponent->GetActivePlayer()); break;
    }
    return true;
}
#endif
