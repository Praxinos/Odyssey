// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "Board/BoardSequence.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "MovieScene.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "Tracks/MovieSceneFadeTrack.h"
#include "Tracks/MovieSceneLevelVisibilityTrack.h"
#include "Tracks/MovieSceneAudioTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h" //TMP

//---

UBoardSequence::UBoardSequence(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MovieScene(nullptr)
{
	//bParentContextsAreSignificant = true;
}

void UBoardSequence::Initialize()
{
	MovieScene = NewObject<UMovieScene>(this, NAME_None, RF_Transactional);

	MovieScene->SetEvaluationType(EMovieSceneEvaluationType::WithSubFrames);

	FFrameRate TickResolution(24000, 1);
	MovieScene->SetTickResolutionDirectly(TickResolution);

	FFrameRate DisplayRate(24, 1);
	MovieScene->SetDisplayRate(DisplayRate);
}

void UBoardSequence::BindPossessableObject(const FGuid& ObjectId, UObject& PossessedObject, UObject* Context)
{
    MovieScene->RemovePossessable( ObjectId );
}

bool UBoardSequence::CanPossessObject(UObject& Object, UObject* InPlaybackContext) const
{
    return false;
}

void UBoardSequence::LocateBoundObjects(const FGuid& ObjectId, UObject* Context, TArray<UObject*, TInlineAllocator<1>>& OutObjects) const
{
}

UMovieScene* UBoardSequence::GetMovieScene() const
{
	return MovieScene;
}

UObject* UBoardSequence::GetParentObject(UObject* Object) const
{
	return nullptr;
}

void UBoardSequence::UnbindPossessableObjects(const FGuid& ObjectId)
{
}

void UBoardSequence::UnbindObjects(const FGuid& ObjectId, const TArray<UObject*>& InObjects, UObject* Context)
{
}

void UBoardSequence::UnbindInvalidObjects(const FGuid& ObjectId, UObject* Context)
{
}

#if WITH_EDITOR

ETrackSupport
UBoardSequence::IsTrackSupported( TSubclassOf<class UMovieSceneTrack> InTrackClass ) const
{
    if( InTrackClass == UMovieSceneCinematicBoardTrack::StaticClass() || 
        InTrackClass == UMovieSceneSingleCameraCutTrack::StaticClass() || //TMP
        InTrackClass == UMovieSceneAudioTrack::StaticClass() ||
        InTrackClass == UMovieSceneFadeTrack::StaticClass() ||
        InTrackClass == UMovieSceneLevelVisibilityTrack::StaticClass() )
    {
        return ETrackSupport::Supported;
    }

    return ETrackSupport::NotSupported;
}

//FText UBoardSequence::GetDisplayName() const
//{
//	return UMovieSceneSequence::GetDisplayName();
//}
//
//void UBoardSequence::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
//{
//	Super::GetAssetRegistryTags(OutTags);
//
//	if (BoundActorClass != nullptr)
//	{
//		FAssetRegistryTag Tag("BoundActorClass", BoundActorClass->GetName(), FAssetRegistryTag::TT_Alphabetical);
//		OutTags.Add(Tag);
//	}
//	else
//	{
//		OutTags.Emplace("BoundActorClass", "(None)", FAssetRegistryTag::TT_Alphabetical);
//	}
//}
//
//void UBoardSequence::GetAssetRegistryTagMetadata(TMap<FName, FAssetRegistryTagMetadata>& OutMetadata) const
//{
//	Super::GetAssetRegistryTagMetadata(OutMetadata);
//
//	OutMetadata.Add(
//		"BoundActorClass",
//		FAssetRegistryTagMetadata()
//			.SetDisplayName(NSLOCTEXT("TemplateSequence", "BoundActorClass_Label", "Bound Actor Class"))
//			.SetTooltip(NSLOCTEXT("TemplateSequence", "BoundActorClass_Tooltip", "The type of actor bound to this template sequence"))
//		);
//}
//
#endif
