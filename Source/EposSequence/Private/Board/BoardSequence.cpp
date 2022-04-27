// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Board/BoardSequence.h"

#include "Animation/SkeletalMeshActor.h"
#include "Components/ActorComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "MovieScene.h"
#include "Sections/MovieSceneSubSection.h"
#include "MovieSceneMediaTrack.h"
#include "MovieSceneTimeHelpers.h"
#include "Tracks/MovieSceneFadeTrack.h"
#include "Tracks/MovieSceneLevelVisibilityTrack.h"
#include "Tracks/MovieSceneAudioTrack.h"

#include "Board/BoardHelpers.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceModule.h"
#include "INamingFormatter.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h" //TMP
#include "NoteTrack/MovieSceneNoteTrack.h"

//---

UBoardSequence::UBoardSequence(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , MovieScene(nullptr)
{
    bParentContextsAreSignificant = true;

    FEposSequenceModule& module = FModuleManager::LoadModuleChecked<FEposSequenceModule>( "EposSequence" );
    mNamingFormatter = module.GetNamingFormatter<UNamingFormatterBoard>();
}

void UBoardSequence::Initialize( FFrameRate iTickRate, FFrameRate iDisplayRate )
{
    MovieScene = NewObject<UMovieScene>(this, NAME_None, RF_Transactional);

    MovieScene->SetEvaluationType(EMovieSceneEvaluationType::WithSubFrames);

    MovieScene->SetTickResolutionDirectly( iTickRate );

    MovieScene->SetDisplayRate( iDisplayRate );
}

void UBoardSequence::BindPossessableObject(const FGuid& ObjectId, UObject& PossessedObject, UObject* Context)
{
    if( !CanPossessObject( PossessedObject, Context ) )
    {
        MovieScene->RemovePossessable( ObjectId );
        //UnbindPossessableObjects( ObjectId ); // Not necessary (?) as it can't have been added previously (?)

        return;
    }

    ActorsBindingIdToReferences.FindOrAdd( ObjectId ) = FLevelSequenceBindingReference( &PossessedObject, Context );
}

bool UBoardSequence::CanPossessObject(UObject& Object, UObject* InPlaybackContext) const
{
    return Object.IsA<AStaticMeshActor>()
        || Object.IsA<ASkeletalMeshActor>()
        || Object.IsA<UActorComponent>()
        || ExactCast<AActor>( &Object ); // Empty Actor
}

bool UBoardSequence::CanRebindPossessable( const FMovieScenePossessable& InPossessable ) const
{
    return !InPossessable.GetParent().IsValid();
}

void UBoardSequence::LocateBoundObjects(const FGuid& ObjectId, UObject* Context, TArray<UObject*, TInlineAllocator<1>>& OutObjects) const
{
    const FLevelSequenceBindingReference* Reference = ActorsBindingIdToReferences.Find( ObjectId );
    if( Reference )
    {
        UObject* ResolvedObject = Reference->Resolve( Context, NAME_None );
        if( ResolvedObject && ResolvedObject->GetWorld() )
        {
            OutObjects.Add( ResolvedObject );
        }
    }
}

UMovieScene* UBoardSequence::GetMovieScene() const
{
    return MovieScene;
}

UObject* UBoardSequence::GetParentObject(UObject* Object) const
{
    if( UActorComponent* Component = Cast<UActorComponent>( Object ) )
    {
        return Component->GetOwner();
    }

    return nullptr;
}

void UBoardSequence::UnbindPossessableObjects(const FGuid& ObjectId)
{
    ActorsBindingIdToReferences.Remove( ObjectId );
}

void UBoardSequence::UnbindObjects(const FGuid& ObjectId, const TArray<UObject*>& InObjects, UObject* Context)
{
    FLevelSequenceBindingReference* Reference = ActorsBindingIdToReferences.Find( ObjectId );
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

void UBoardSequence::UnbindInvalidObjects(const FGuid& ObjectId, UObject* Context)
{
    FLevelSequenceBindingReference* Reference = ActorsBindingIdToReferences.Find( ObjectId );
    if( Reference )
    {
        UObject* ResolvedObject = Reference->Resolve( Context, NAME_None );
        if( !IsValid( ResolvedObject ) )
        {
            *Reference = FLevelSequenceBindingReference();
        }

        return;
    }
}

#if WITH_EDITOR

ETrackSupport
UBoardSequence::IsTrackSupported( TSubclassOf<class UMovieSceneTrack> InTrackClass ) const
{
    if( InTrackClass == UMovieSceneCinematicBoardTrack::StaticClass() ||
        InTrackClass == UMovieSceneSingleCameraCutTrack::StaticClass() || // The board sequence needs to support this track type, otherwise when opening a board sequence, inner shot sequence won't have this track available (even if supported in this class), because during opening the toolkit, all the track editors are not called
        InTrackClass == UMovieSceneNoteTrack::StaticClass() ||
        InTrackClass == UMovieSceneAudioTrack::StaticClass() ||
        InTrackClass == UMovieSceneFadeTrack::StaticClass() ||
        InTrackClass == UMovieSceneMediaTrack::StaticClass() ||
        InTrackClass == UMovieSceneLevelVisibilityTrack::StaticClass() )
    {
        return ETrackSupport::Supported;
    }

    return ETrackSupport::NotSupported;
}

FText UBoardSequence::GetDisplayName() const
{
    //return UMovieSceneSequence::GetDisplayName();

    FString name = mNamingFormatter->FormatName( this );

    return FText::FromString( name );
}

//void UBoardSequence::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
//{
//  Super::GetAssetRegistryTags(OutTags);
//
//  if (BoundActorClass != nullptr)
//  {
//      FAssetRegistryTag Tag("BoundActorClass", BoundActorClass->GetName(), FAssetRegistryTag::TT_Alphabetical);
//      OutTags.Add(Tag);
//  }
//  else
//  {
//      OutTags.Emplace("BoundActorClass", "(None)", FAssetRegistryTag::TT_Alphabetical);
//  }
//}
//
//void UBoardSequence::GetAssetRegistryTagMetadata(TMap<FName, FAssetRegistryTagMetadata>& OutMetadata) const
//{
//  Super::GetAssetRegistryTagMetadata(OutMetadata);
//
//  OutMetadata.Add(
//      "BoundActorClass",
//      FAssetRegistryTagMetadata()
//          .SetDisplayName(NSLOCTEXT("TemplateSequence", "BoundActorClass_Label", "Bound Actor Class"))
//          .SetTooltip(NSLOCTEXT("TemplateSequence", "BoundActorClass_Tooltip", "The type of actor bound to this template sequence"))
//      );
//}

#endif

//---

bool
UBoardSequence::IsResizable() const //override
{
    if( !GetMovieScene() )
        return true;

    if( GetMovieScene()->GetAllSections().Num() == 0 )
        return true;

    return false;
}

void
UBoardSequence::Resize( int32 iNewDuration ) //override
{
    //check( IsResizable() ); // I don't know if SectionResized() should call this function recursively, if not, the line can be uncomment

    auto new_range = TRange<FFrameNumber>( 0, iNewDuration );

    UMovieScene* movie_scene = GetMovieScene();
    if( !movie_scene )
        return;

    movie_scene->SetPlaybackRange( new_range );
}

//---

void
UBoardSequence::SectionResized( UMovieSceneSection* iSection ) //override
{
    BoardHelpers::ResizeChildSequence( iSection );
    BoardHelpers::ResizeParentSequenceRecursively( this );
}

void
UBoardSequence::SectionAddedOrRemoved( UMovieSceneSection* iSection ) //override
{
    BoardHelpers::ResizeParentSequenceRecursively( this );
}
