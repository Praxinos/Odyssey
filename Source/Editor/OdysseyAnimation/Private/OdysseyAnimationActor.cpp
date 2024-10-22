// Copyright Epic Games, Inc. All Rights Reserved.

#include "OdysseyAnimationActor.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationPlayer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Subsystems/UnrealEditorSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationActor)

//////////////////////////////////////////////////////////////////////////
// AOdysseyAnimationActor

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

AOdysseyAnimationActor::AOdysseyAnimationActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootComponent = AnimationComponent = CreateDefaultSubobject<UOdysseyAnimationComponent>(TEXT("AnimationComponent"));
	AnimationComponent->SetRelativeRotation(FRotator(0, 90, 90));
}

#if WITH_EDITOR
bool AOdysseyAnimationActor::GetReferencedContentObjects(TArray<UObject*>& Objects) const
{
	Super::GetReferencedContentObjects(Objects);

	if (!AnimationComponent)
		return true;

	switch(AnimationComponent->Mode)
	{
		case EOdysseyAnimationComponentMode::Animation : Objects.Add(AnimationComponent->Animation); break;
		case EOdysseyAnimationComponentMode::Player : Objects.Add(AnimationComponent->Player); break;
	}
	return true;
}
#endif
