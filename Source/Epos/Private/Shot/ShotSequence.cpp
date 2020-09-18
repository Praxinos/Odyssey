// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "Shot/ShotSequence.h"
#include "CineCameraActor.h"
#include "Components/ActorComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "MovieScene.h"
#include "Tracks/MovieSceneFadeTrack.h"
#include "Tracks/MovieSceneLevelVisibilityTrack.h"
#include "Tracks/MovieSceneAudioTrack.h"
#include "Tracks/MovieSceneCameraCutTrack.h"

//---

UShotSequence::UShotSequence( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
    , MovieScene( nullptr )
{
    //bParentContextsAreSignificant = true;
}

void UShotSequence::Initialize()
{
    MovieScene = NewObject<UMovieScene>( this, NAME_None, RF_Transactional );

    MovieScene->SetEvaluationType( EMovieSceneEvaluationType::WithSubFrames );

    FFrameRate TickResolution( 24000, 1 );
    MovieScene->SetTickResolutionDirectly( TickResolution );

    FFrameRate DisplayRate( 24, 1 );
    MovieScene->SetDisplayRate( DisplayRate );
}

void UShotSequence::BindPossessableObject( const FGuid& ObjectId, UObject& PossessedObject, UObject* Context )
{
    if( !CanPossessObject( PossessedObject, Context ) )
    {
        MovieScene->RemovePossessable( ObjectId );
        //UnbindPossessableObjects( ObjectId ); // Not necessary (?) as it can't have been added previously (?)

        return;
    }

    if( PossessedObject.IsA<ACineCameraActor>() )
    {
        MovieScene->RemovePossessable( CameraBindingId );
        UnbindPossessableObjects( CameraBindingId );

        CameraBindingId = ObjectId;
        CameraBindingReference = FLevelSequenceBindingReference( &PossessedObject, Context );
    }
    else
    {
        BindingIdToReferences.FindOrAdd( ObjectId ) = FLevelSequenceBindingReference( &PossessedObject, Context );
    }
}

bool UShotSequence::CanPossessObject( UObject& Object, UObject* InPlaybackContext ) const
{
    return Object.IsA<AStaticMeshActor>() || Object.IsA<ACineCameraActor>();
}

void UShotSequence::LocateBoundObjects( const FGuid& ObjectId, UObject* Context, TArray<UObject*, TInlineAllocator<1>>& OutObjects ) const
{
    if( CameraBindingId == ObjectId )
    {
        UObject* object = CameraBindingReference.Resolve( Context, NAME_None );
        if( !object )
            return;

        OutObjects.Add( object );
        return;
    }

    const FLevelSequenceBindingReference* Reference = BindingIdToReferences.Find( ObjectId );
    if( Reference )
    {
        UObject* ResolvedObject = Reference->Resolve( Context, NAME_None );
        if( ResolvedObject && ResolvedObject->GetWorld() )
        {
            OutObjects.Add( ResolvedObject );
        }
    }
}

UMovieScene* UShotSequence::GetMovieScene() const
{
    return MovieScene;
}

UObject* UShotSequence::GetParentObject( UObject* Object ) const
{
    //if (UActorComponent* Component = Cast<UActorComponent>(Object))
    //{
    //	return Component->GetOwner();
    //}

    return nullptr;
}

void UShotSequence::UnbindPossessableObjects( const FGuid& ObjectId )
{
    if( CameraBindingId == ObjectId )
    {
        CameraBindingId = FGuid();
        CameraBindingReference = FLevelSequenceBindingReference();
        return;
    }

    BindingIdToReferences.Remove( ObjectId );
}

void UShotSequence::UnbindObjects( const FGuid& ObjectId, const TArray<UObject*>& InObjects, UObject* Context )
{
    if( CameraBindingId == ObjectId )
    {
        CameraBindingId = FGuid();
        CameraBindingReference = FLevelSequenceBindingReference();

        return;
    }

    FLevelSequenceBindingReference* Reference = BindingIdToReferences.Find( ObjectId );
    if( Reference )
    {
        UObject* ResolvedObject = Reference->Resolve( Context, NAME_None );
        if( InObjects.Contains( ResolvedObject ) )
        {
            *Reference = FLevelSequenceBindingReference();
        }

        return;
    }
}

void UShotSequence::UnbindInvalidObjects( const FGuid& ObjectId, UObject* Context )
{
    if( CameraBindingId == ObjectId )
    {
        CameraBindingId = FGuid();
        CameraBindingReference = FLevelSequenceBindingReference();

        return;
    }

    FLevelSequenceBindingReference* Reference = BindingIdToReferences.Find( ObjectId );
    if( Reference )
    {
        UObject* ResolvedObject = Reference->Resolve( Context, NAME_None );
        if( !ResolvedObject || ResolvedObject->IsPendingKill() )
        {
            *Reference = FLevelSequenceBindingReference();
        }

        return;
    }
}

#if WITH_EDITOR

ETrackSupport
UShotSequence::IsTrackSupported( TSubclassOf<class UMovieSceneTrack> InTrackClass ) const
{
    if( InTrackClass == UMovieSceneCameraCutTrack::StaticClass() ||
        InTrackClass == UMovieSceneAudioTrack::StaticClass() ||
        InTrackClass == UMovieSceneFadeTrack::StaticClass() ||
        InTrackClass == UMovieSceneLevelVisibilityTrack::StaticClass() )
    {
        return ETrackSupport::Supported;
    }

    return ETrackSupport::NotSupported;
    //return Super::IsTrackSupported( InTrackClass );
}

//FText UShotSequence::GetDisplayName() const
//{
//	return UMovieSceneSequence::GetDisplayName();
//}
//
//void UShotSequence::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
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
//void UShotSequence::GetAssetRegistryTagMetadata(TMap<FName, FAssetRegistryTagMetadata>& OutMetadata) const
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
