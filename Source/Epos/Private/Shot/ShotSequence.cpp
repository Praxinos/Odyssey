// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "Shot/ShotSequence.h"
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
    //if (UActorComponent* Component = Cast<UActorComponent>(&PossessedObject))
    //{
    //	const FName ComponentName = Component->GetFName();
    //	BoundActorComponents.Add(ObjectId, ComponentName);
    //}

    UPackage* ObjectPackage = PossessedObject.GetOutermost();
    if( !ensure( ObjectPackage ) )
    {
        return;
    }

    BindingId = ObjectId;
    //ObjectPath = PossessedObject.GetPathName( Context );

    FString PackageName = ObjectPackage->GetName();
#if WITH_EDITORONLY_DATA
    // If this is being set from PIE we need to remove the pie prefix and point to the editor object
    if( ObjectPackage->PIEInstanceID != INDEX_NONE )
    {
        FString PIEPrefix = FString::Printf( PLAYWORLD_PACKAGE_PREFIX TEXT( "_%d_" ), ObjectPackage->PIEInstanceID );
        PackageName.ReplaceInline( *PIEPrefix, TEXT( "" ) );
    }
#endif

    FString FullPath = PackageName + TEXT( "." ) + PossessedObject.GetPathName( ObjectPackage );
    ExternalObjectPath = FSoftObjectPath( FullPath );
}

bool UShotSequence::CanPossessObject( UObject& Object, UObject* InPlaybackContext ) const
{
    return Object.IsA<AStaticMeshActor>();
}

void UShotSequence::LocateBoundObjects( const FGuid& ObjectId, UObject* Context, TArray<UObject*, TInlineAllocator<1>>& OutObjects ) const
{
    //AActor* Actor = Cast<AActor>(Context);
    //if (Actor == nullptr)
    //{
    //	return;
    //}

    //const FName* ComponentName = BoundActorComponents.Find(ObjectId);
    //if (ComponentName == nullptr)
    //{
    //	return;
    //}

    //if (UActorComponent* FoundComponent = FindObject<UActorComponent>(Actor, *ComponentName->ToString(), false))
    //{
    //	OutObjects.Add(FoundComponent);
    //}

    if( ObjectId != BindingId )
        return;

    //UObject* object = FindObject<UObject>( Context, *ObjectPath, false );
    //if( !object )
    //    return;

    FSoftObjectPath TempPath = ExternalObjectPath;

    // Soft Object Paths don't follow asset redirectors when attempting to call ResolveObject or TryLoad.
    // We want to follow the asset redirector so that maps that have been renamed (from Untitled to their first asset name)
    // properly resolve. This fixes Possessable bindings losing their references the first time you save a map.
    TempPath.PreSavePath();

#if WITH_EDITORONLY_DATA
    int32 ContextPlayInEditorID = Context ? Context->GetOutermost()->PIEInstanceID : INDEX_NONE;

    if( ContextPlayInEditorID != INDEX_NONE )
    {
        // We have an override PIE id, so set the global before entering
        TGuardValue<int32> PIEGuard( GPlayInEditorID, ContextPlayInEditorID );
        TempPath.FixupForPIE();
    }
    else
    {
        TempPath.FixupForPIE();
    }
#endif

    UObject* object = TempPath.ResolveObject();
    if( !object )
        return;

    OutObjects.Add( object );
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
    //BoundActorComponents.Remove(ObjectId);
}

void UShotSequence::UnbindObjects( const FGuid& ObjectId, const TArray<UObject*>& InObjects, UObject* Context )
{
    //BoundActorComponents.Remove(ObjectId);
}

void UShotSequence::UnbindInvalidObjects( const FGuid& ObjectId, UObject* Context )
{
    //BoundActorComponents.Remove(ObjectId);
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
