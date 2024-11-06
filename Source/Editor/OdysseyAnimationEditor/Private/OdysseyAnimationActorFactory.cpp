// Copyright Epic Games, Inc. All Rights Reserved.

#include "OdysseyAnimationActorFactory.h"
#include "AssetRegistry/AssetData.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimation.h"
#include "Subsystems/UnrealEditorSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationActorFactory)

#define LOCTEXT_NAMESPACE "AnimationEditor"

//////////////////////////////////////////////////////////////////////////
// UOdysseyAnimationActorFactory

UOdysseyAnimationActorFactory::UOdysseyAnimationActorFactory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    DisplayName = LOCTEXT("animation-actor-factory.display-name", "Animation Factory");
    NewActorClass = AOdysseyAnimationActor::StaticClass();
}

void UOdysseyAnimationActorFactory::PostSpawnActor(UObject* iAsset, AActor* iActor)
{
    Super::PostSpawnActor(iAsset, iActor);

    UOdysseyAnimation* animation = Cast<UOdysseyAnimation>(iAsset);
    if (!animation)
        return;

    AOdysseyAnimationActor* animationActor = CastChecked<AOdysseyAnimationActor>(iActor);
    FOdysseyObjectEditorUtils::SetPropertyValue(animationActor->AnimationComponent, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationComponent, Animation), animation);

    //Set plane in front of the camera

    UUnrealEditorSubsystem* UnrealEditorSubsystem = GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>();
    FVector cameraLocation;
    FRotator cameraRotation;
    if (!UnrealEditorSubsystem->GetLevelViewportCameraInfo(cameraLocation, cameraRotation))
        return;

    //-

    float FocusDistance = 50;
    FVector plane_location = cameraLocation + cameraRotation.Vector() * FocusDistance;
    FRotator plane_rotator = cameraRotation;

    //---

    animationActor->SetActorRotation( FRotator(0, 90, 90) );
    animationActor->AddActorWorldRotation( plane_rotator );

}

bool UOdysseyAnimationActorFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
    if (AssetData.IsValid() && AssetData.IsInstanceOf(UOdysseyAnimation::StaticClass()))
    {
        return true;
    }
    else
    {
        OutErrorMsg = LOCTEXT("animation-actor-factory.error.no-animation-specified", "No animation was specified.");
        return false;
    }
}

#undef LOCTEXT_NAMESPACE