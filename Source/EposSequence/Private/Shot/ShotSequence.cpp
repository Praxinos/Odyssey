// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Shot/ShotSequence.h"

#include "Animation/SkeletalMeshActor.h"
#include "CineCameraActor.h"
#include "Components/ActorComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "MovieScene.h"
#include "Sections/MovieSceneSubSection.h"
#include "MovieSceneTimeHelpers.h"
#include "Tracks/MovieSceneFadeTrack.h"
#include "Tracks/MovieSceneLevelVisibilityTrack.h"
#include "Tracks/MovieSceneAudioTrack.h"

#include "Board/BoardHelpers.h"
#include "PlaneActor.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"

//---

UShotSequence::UShotSequence( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
    , MovieScene( nullptr )
{
    bParentContextsAreSignificant = true;
}

void UShotSequence::Initialize( FFrameRate iTickRate, FFrameRate iDisplayRate )
{
    MovieScene = NewObject<UMovieScene>( this, NAME_None, RF_Transactional );

    MovieScene->SetEvaluationType( EMovieSceneEvaluationType::WithSubFrames );

    MovieScene->SetTickResolutionDirectly( iTickRate );

    MovieScene->SetDisplayRate( iDisplayRate );
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
        for( auto It = CameraBindingIdToReferences.CreateConstIterator(); It; ++It )
        {
            FGuid binding = It.Key();
            //FGuid binding = pair.Key;
            //FLevelSequenceBindingReference reference = pair.Value;

            MovieScene->RemovePossessable( binding );
            UnbindPossessableObjects( binding );
        }

        check( !CameraBindingIdToReferences.Num() );

        CameraBindingIdToReferences.FindOrAdd( ObjectId ) = FLevelSequenceBindingReference( &PossessedObject, Context );
    }
    else if( PossessedObject.IsA<UActorComponent>() && PossessedObject.GetTypedOuter<ACineCameraActor>() )
    {
        CameraBindingIdToReferences.FindOrAdd( ObjectId ) = FLevelSequenceBindingReference( &PossessedObject, Context );
    }
    else if( PossessedObject.IsA<APlaneActor>()
             || PossessedObject.IsA<UActorComponent>() && PossessedObject.GetTypedOuter<APlaneActor>() )
    {
        PlanesBindingIdToReferences.FindOrAdd( ObjectId ) = FLevelSequenceBindingReference( &PossessedObject, Context );
    }
    else
    {
        ActorsBindingIdToReferences.FindOrAdd( ObjectId ) = FLevelSequenceBindingReference( &PossessedObject, Context );
    }
}

bool UShotSequence::CanPossessObject( UObject& Object, UObject* InPlaybackContext ) const
{
    return Object.IsA<APlaneActor>()
        || Object.IsA<AStaticMeshActor>()
        || Object.IsA<ASkeletalMeshActor>()
        || Object.IsA<ACineCameraActor>()
        || Object.IsA<UActorComponent>();
}

bool UShotSequence::CanRebindPossessable( const FMovieScenePossessable& InPossessable ) const
{
    return !InPossessable.GetParent().IsValid();
}

void UShotSequence::LocateBoundObjects( const FGuid& ObjectId, UObject* Context, TArray<UObject*, TInlineAllocator<1>>& OutObjects ) const
{
    const FLevelSequenceBindingReference* Reference = CameraBindingIdToReferences.Find( ObjectId );
    if( Reference )
    {
        UObject* ResolvedObject = Reference->Resolve( Context, NAME_None );
        if( ResolvedObject && ResolvedObject->GetWorld() )
        {
            OutObjects.Add( ResolvedObject );
        }
        return;
    }

    Reference = PlanesBindingIdToReferences.Find( ObjectId );
    if( Reference )
    {
        UObject* ResolvedObject = Reference->Resolve( Context, NAME_None );
        if( ResolvedObject && ResolvedObject->GetWorld() )
        {
            OutObjects.Add( ResolvedObject );
        }
    }

    Reference = ActorsBindingIdToReferences.Find( ObjectId );
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
    if( UActorComponent* Component = Cast<UActorComponent>( Object ) )
    {
        return Component->GetOwner();
    }

    return nullptr;
}

void UShotSequence::UnbindPossessableObjects( const FGuid& ObjectId )
{
    CameraBindingIdToReferences.Remove( ObjectId );
    PlanesBindingIdToReferences.Remove( ObjectId );
    ActorsBindingIdToReferences.Remove( ObjectId );
}

void UShotSequence::UnbindObjects( const FGuid& ObjectId, const TArray<UObject*>& InObjects, UObject* Context )
{
    FLevelSequenceBindingReference* Reference = CameraBindingIdToReferences.Find( ObjectId );
    if( Reference )
    {
        UObject* ResolvedObject = Reference->Resolve( Context, NAME_None );
        if( InObjects.Contains( ResolvedObject ) )
        {
            *Reference = FLevelSequenceBindingReference();
        }

        return;
    }

    Reference = PlanesBindingIdToReferences.Find( ObjectId );
    if( Reference )
    {
        UObject* ResolvedObject = Reference->Resolve( Context, NAME_None );
        if( InObjects.Contains( ResolvedObject ) )
        {
            *Reference = FLevelSequenceBindingReference();
        }

        return;
    }

    Reference = ActorsBindingIdToReferences.Find( ObjectId );
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
    FLevelSequenceBindingReference* Reference = CameraBindingIdToReferences.Find( ObjectId );
    if( Reference )
    {
        UObject* ResolvedObject = Reference->Resolve( Context, NAME_None );
        if( !ResolvedObject || ResolvedObject->IsPendingKill() )
        {
            *Reference = FLevelSequenceBindingReference();
        }

        return;
    }

    Reference = PlanesBindingIdToReferences.Find( ObjectId );
    if( Reference )
    {
        UObject* ResolvedObject = Reference->Resolve( Context, NAME_None );
        if( !ResolvedObject || ResolvedObject->IsPendingKill() )
        {
            *Reference = FLevelSequenceBindingReference();
        }

        return;
    }

    Reference = ActorsBindingIdToReferences.Find( ObjectId );
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
    if( InTrackClass == UMovieSceneSingleCameraCutTrack::StaticClass() ||
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
//  return UMovieSceneSequence::GetDisplayName();
//}

void UShotSequence::GetAssetRegistryTags( TArray<FAssetRegistryTag>& OutTags ) const
{
    Super::GetAssetRegistryTags( OutTags );

    if( CameraBindingIdToReferences.Num() )
    {
        FString value;
        for( const TPair< FGuid, FLevelSequenceBindingReference >& pair : CameraBindingIdToReferences )
        {
            FGuid binding = pair.Key;
            //FLevelSequenceBindingReference reference = pair.Value;

            FMovieScenePossessable* possessable = MovieScene->FindPossessable( binding );
            if( possessable )
            {
                value = possessable->GetName();
                //value = MovieScene->GetObjectDisplayName( binding ).ToString();

                break; // Should be only one camera root
            }
        }

        FAssetRegistryTag Tag( "Camera", value, FAssetRegistryTag::TT_Alphabetical );
        OutTags.Add( Tag );
    }
    else
    {
        OutTags.Emplace( "Camera", "(None)", FAssetRegistryTag::TT_Alphabetical );
    }

    if( PlanesBindingIdToReferences.Num() )
    {
        int plane_count = 0;
        for( const TPair< FGuid, FLevelSequenceBindingReference >& pair : PlanesBindingIdToReferences )
        {
            FGuid binding = pair.Key;
            //FLevelSequenceBindingReference reference = pair.Value;

            FMovieScenePossessable* possessable = MovieScene->FindPossessable( binding );
            if( possessable && !possessable->GetParent().IsValid() /* to get only root planes */ )
            {
                plane_count++;
            }
        }

        FAssetRegistryTag Tag( "Planes", FString::FromInt( plane_count ), FAssetRegistryTag::TT_Alphabetical );
        //FAssetRegistryTag Tag( "Planes", FString::FromInt( PlanesBindingIdToReferences.Num() ), FAssetRegistryTag::TT_Alphabetical );
        OutTags.Add( Tag );
    }
    else
    {
        OutTags.Emplace( "Planes", "(0)", FAssetRegistryTag::TT_Alphabetical );
    }

    if( ActorsBindingIdToReferences.Num() )
    {
        int actor_count = 0;
        for( const TPair< FGuid, FLevelSequenceBindingReference >& pair : ActorsBindingIdToReferences )
        {
            FGuid binding = pair.Key;
            //FLevelSequenceBindingReference reference = pair.Value;

            FMovieScenePossessable* possessable = MovieScene->FindPossessable( binding );
            if( possessable && !possessable->GetParent().IsValid() /* to get only root actors */ )
            {
                actor_count++;
            }
        }

        FAssetRegistryTag Tag( "Actors", FString::FromInt( actor_count ), FAssetRegistryTag::TT_Alphabetical );
        //FAssetRegistryTag Tag( "Actors", FString::FromInt( ActorsBindingIdToReferences.Num() ), FAssetRegistryTag::TT_Alphabetical );
        OutTags.Add( Tag );
    }
    else
    {
        OutTags.Emplace( "Actors", "(0)", FAssetRegistryTag::TT_Alphabetical );
    }
}

void UShotSequence::GetAssetRegistryTagMetadata( TMap<FName, FAssetRegistryTagMetadata>& OutMetadata ) const
{
    Super::GetAssetRegistryTagMetadata( OutMetadata );

    OutMetadata.Add(
        "Camera",
        FAssetRegistryTagMetadata()
        .SetDisplayName( NSLOCTEXT( "ShotSequence", "Camera_Label", "Camera in shot" ) )
        .SetTooltip( NSLOCTEXT( "ShotSequence", "Camera_Tooltip", "The camera bound to this shot sequence" ) )
    );

    OutMetadata.Add(
        "Planes",
        FAssetRegistryTagMetadata()
        .SetDisplayName( NSLOCTEXT( "ShotSequence", "Planes_Label", "Planes in shot" ) )
        .SetTooltip( NSLOCTEXT( "ShotSequence", "Planes_Tooltip", "The planes bound to this shot sequence" ) )
    );

    OutMetadata.Add(
        "Actors",
        FAssetRegistryTagMetadata()
        .SetDisplayName( NSLOCTEXT( "ShotSequence", "Actors_Label", "Actors in shot" ) )
        .SetTooltip( NSLOCTEXT( "ShotSequence", "Actors_Tooltip", "The actors bound to this shot sequence" ) )
    );
}

#endif

//---

bool
UShotSequence::IsResizable() const //override
{
    return true;
}

void
UShotSequence::Resize( int32 iNewDuration ) //override
{
    check( IsResizable() );

    auto new_range = TRange<FFrameNumber>( 0, iNewDuration );

    UMovieScene* movie_scene = GetMovieScene();
    if( !movie_scene )
        return;

    movie_scene->SetPlaybackRange( new_range );

    UMovieSceneTrack* track = movie_scene->GetCameraCutTrack();
    if( track )
    {
        auto sections = track->GetAllSections();
        if( sections.Num() )
        {
            check( sections.Num() == 1 )

            UMovieSceneSection* section = sections[0];
            section->SetRange( new_range );
        }
    }
}

//---

void
UShotSequence::SectionResized( UMovieSceneSection* iSection ) //override
{
    BoardHelpers::ResizeParentSequenceRecursively( this );
}

void
UShotSequence::SectionAddedOrRemoved( UMovieSceneSection* iSection ) //override
{
    BoardHelpers::ResizeParentSequenceRecursively( this );
}
