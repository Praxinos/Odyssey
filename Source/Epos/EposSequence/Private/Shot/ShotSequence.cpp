// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shot/ShotSequence.h"

#include "Animation/SkeletalMeshActor.h"
#include "CineCameraActor.h"
#include "Components/ActorComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "MediaPlate.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "MovieScene.h"
#include "MovieSceneBindingReferences.h"
#include "MovieSceneMediaTrack.h"
#include "MovieSceneTimeHelpers.h"
#include "NiagaraActor.h"
#include "PaperFlipbookActor.h"
#include "Sections/MovieSceneSubSection.h"
#include "Tracks/MovieSceneFadeTrack.h"
#include "Tracks/MovieSceneLevelVisibilityTrack.h"
#include "Tracks/MovieSceneAudioTrack.h"
#include "Tracks/MovieSceneSkeletalAnimationTrack.h"
#include "Tracks/MovieSceneVisibilityTrack.h"
#include "UObject/AssetRegistryTagsContext.h"
#include "SubObjectLocator.h"
#include "UniversalObjectLocators/ActorLocatorFragment.h"
#include "Sequencer/MovieSceneControlRigParameterTrack.h"
#include "Tracks/TemplateSequenceTrack.h"

#include "Board/BoardHelpers.h"
#include "EposSequenceModule.h"
#include "INamingFormatter.h"
#include "PlaneActor.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyAnimationTimelineTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "NoteTrack/MovieSceneNoteTrack.h"

//---

UShotSequence::UShotSequence( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
    , MovieScene( nullptr )
{
    bParentContextsAreSignificant = true;

    FEposSequenceModule& module = FModuleManager::LoadModuleChecked<FEposSequenceModule>( "EposSequence" );
    mNamingFormatter = module.GetNamingFormatter<UNamingFormatterShot>();
}

void UShotSequence::Initialize( FFrameRate iTickRate, FFrameRate iDisplayRate )
{
    MovieScene = NewObject<UMovieScene>( this, NAME_None, RF_Transactional );

    MovieScene->SetEvaluationType( EMovieSceneEvaluationType::WithSubFrames );

    MovieScene->SetTickResolutionDirectly( iTickRate );

    MovieScene->SetDisplayRate( iDisplayRate );

#if WITH_EDITOR
    MovieScene->SetPlaybackRangeLocked( true );
#endif
}

void UShotSequence::PostLoad()
{
    Super::PostLoad();

    for( TPair< FGuid, FLevelSequenceBindingReference > pair : CameraBindingIdToReferences_DEPRECATED )
    {
        FLevelSequenceBindingReference legacy_ref = pair.Value;

        if( legacy_ref.ExternalObjectPath.IsNull() )
        {
            // Make a copy and add the object path
            FUniversalObjectLocator NewLocator;
            NewLocator.AddFragment<FSubObjectLocator>( MoveTemp( legacy_ref.ObjectPath ) );

            CameraBindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
        }
        else
        {
            FUniversalObjectLocator NewLocator;
            NewLocator.AddFragment<FActorLocatorFragment>( MoveTemp( legacy_ref.ExternalObjectPath ) );

            CameraBindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
        }
    }

    CameraBindingIdToReferences_DEPRECATED.Empty();

    for( TPair< FGuid, FLevelSequenceBindingReference > pair : PlanesBindingIdToReferences_DEPRECATED )
    {
        FLevelSequenceBindingReference legacy_ref = pair.Value;

        if( legacy_ref.ExternalObjectPath.IsNull() )
        {
            // Make a copy and add the object path
            FUniversalObjectLocator NewLocator;
            NewLocator.AddFragment<FSubObjectLocator>( MoveTemp( legacy_ref.ObjectPath ) );

            ActorsBindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
        }
        else
        {
            FUniversalObjectLocator NewLocator;
            NewLocator.AddFragment<FActorLocatorFragment>( MoveTemp( legacy_ref.ExternalObjectPath ) );

            ActorsBindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
        }
    }

    PlanesBindingIdToReferences_DEPRECATED.Empty();

    for( TPair< FGuid, FLevelSequenceBindingReference > pair : ActorsBindingIdToReferences_DEPRECATED )
    {
        FLevelSequenceBindingReference legacy_ref = pair.Value;

        if( legacy_ref.ExternalObjectPath.IsNull() )
        {
            // Make a copy and add the object path
            FUniversalObjectLocator NewLocator;
            NewLocator.AddFragment<FSubObjectLocator>( MoveTemp( legacy_ref.ObjectPath ) );

            ActorsBindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
        }
        else
        {
            FUniversalObjectLocator NewLocator;
            NewLocator.AddFragment<FActorLocatorFragment>( MoveTemp( legacy_ref.ExternalObjectPath ) );

            ActorsBindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
        }
    }

    ActorsBindingIdToReferences_DEPRECATED.Empty();

    //-

    for( FMovieSceneBindingReference reference : PlanesBindingReferences_DEPRECATED.GetAllReferences() )
    {
        FUniversalObjectLocator NewLocator( reference.Locator );
        ActorsBindingReferences.FMovieSceneBindingReferences::AddBinding( reference.ID, MoveTemp( NewLocator ), reference.ResolveFlags, reference.CustomBinding );
    }

    for( FMovieSceneBindingReference reference : PlanesBindingReferences_DEPRECATED.GetAllReferences() )
        PlanesBindingReferences_DEPRECATED.RemoveBinding( reference.ID );

#if WITH_EDITOR
#endif
}

const FMovieSceneBindingReferences* UShotSequence::GetBindingReferences() const //override
{
    // For the moment, don't use GetBindingReferences()
    // because in this case, BindPossessableObject() won't be called in the FSequencerUtilities::CreateBinding()#2650
    // as CreateGenericBinding() will be called instead of CreateImplementationDefinedBinding()
    //
    // and when CreateGenericBinding() is used, FMovieSceneBindingReferences::AddBinding() is called directly (without BindPossessableObject())
    // so the check of possible possessables object (Camera/Animation/...) must be done there
    // (No, because FMovieSceneBindingReferences::AddBinding() is not virtual...)
    //
    // (If GetBindingReferences() is not used (aka return nullptr), LocateBoundObjects() is required)
    return nullptr;

    //return &BindingReferences;
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
        FMovieScenePossessable* possessable = MovieScene->FindPossessable( ObjectId );
        // If it exists, it was added by FSequencer::CreateBinding()#1083, and then, juste remove everything about camera
        if( possessable )
        {
            // Temporary reference ids, otherwise UnbindPossessableObjects() modify CameraBindingReferences
            TArray<FGuid> old_references;
            for( const FMovieSceneBindingReference& reference : CameraBindingReferences.GetAllReferences() )
            {
                old_references.Add( reference.ID );
            }
            for( FGuid reference_id : old_references )
            {
                MovieScene->RemovePossessable( reference_id );
                UnbindPossessableObjects( reference_id );
            }

            check( CameraBindingReferences.GetAllReferences().IsEmpty() );

            //for( const FMovieSceneBindingReference& reference : BindingReferences.GetAllReferences() )
            //{
            //    const FUniversalObjectLocatorFragment* fragment = reference.Locator.GetLastFragment();
            //    const UE::UniversalObjectLocator::FFragmentType* fragment_type = reference.Locator.GetLastFragmentType();
            //    //fragment_type->FragmentTypeID
            //    const void* payload = fragment->GetPayload();
            //    const FActorLocatorFragment* p = fragment->GetPayloadAs( FActorLocatorFragment::FragmentType );
            //    UObject* object = p->Path.ResolveObject();


            //    UObject* object = reference.Locator.SyncFind( nullptr ); // nullptr ?????????????????
            //    //UE::UniversalObjectLocator::FResolveResult result = reference.Locator.Resolve(
            //    if( !object )
            //        continue;

            //    if( object->IsA<ACineCameraActor>() )
            //    {
            //        MovieScene->RemovePossessable( reference.ID );
            //        UnbindPossessableObjects( reference.ID );
            //    }
            //}
        }
        // Otherwise it comes from (at least) FSequencer::DoAssignActor()#7611,
        // and in this case, just add the new camera and let this function update everything to keep all existing components and remove the old one

        if( Context )
        {
            CameraBindingReferences.AddBinding( ObjectId, &PossessedObject, Context );
        }
    }
    else if( PossessedObject.IsA<UActorComponent>() && PossessedObject.GetTypedOuter<ACineCameraActor>() )
    {
        if( Context )
        {
            CameraBindingReferences.AddBinding( ObjectId, &PossessedObject, Context );
        }
    }
    else if( PossessedObject.IsA<AOdysseyAnimationActor>()
             || PossessedObject.IsA<UActorComponent>() && PossessedObject.GetTypedOuter<AOdysseyAnimationActor>() )
    {
        if( Context )
        {
            AnimationsBindingReferences.AddBinding( ObjectId, &PossessedObject, Context );
        }
    }
    else
    {
        if( Context )
        {
            ActorsBindingReferences.AddBinding( ObjectId, &PossessedObject, Context );
        }
    }
}

bool UShotSequence::CanPossessObject( UObject& Object, UObject* InPlaybackContext ) const
{
    return Object.IsA<APlaneActor>()
        || Object.IsA<AStaticMeshActor>()
        || Object.IsA<ASkeletalMeshActor>()
        || Object.IsA<ACineCameraActor>()
        || Object.IsA<UActorComponent>()
        || Object.IsA<ANiagaraActor>()
        || Object.IsA<APaperFlipbookActor>()
        || Object.IsA<AMediaPlate>()
        || Object.IsA<AOdysseyAnimationActor>()
        || ExactCast<AActor>( &Object ); // Empty Actor
}

bool UShotSequence::CanRebindPossessable( const FMovieScenePossessable& InPossessable ) const
{
    return !InPossessable.GetParent().IsValid();
}

void UShotSequence::LocateBoundObjects( const FGuid& ObjectId, UObject* Context, TArray<UObject*, TInlineAllocator<1>>& OutObjects ) const
{
    CameraBindingReferences.ResolveBinding( ObjectId, Context, OutObjects );
    AnimationsBindingReferences.ResolveBinding( ObjectId, Context, OutObjects );
    ActorsBindingReferences.ResolveBinding( ObjectId, Context, OutObjects );
}

#if WITH_EDITOR
FGuid UShotSequence::CreatePossessable( UObject* ObjectToPossess ) //override
{
    return FindOrAddBinding( ObjectToPossess );
}
//FGuid UShotSequence::CreateSpawnable( UObject* ObjectToSpawn ) //override
//{
//}

// From LevelSequence.cpp
FGuid UShotSequence::FindOrAddBinding( UObject* InObject )
{
    using namespace UE::MovieScene;

    UObject* PlaybackContext = InObject ? InObject->GetWorld() : nullptr;
    if( !InObject || !PlaybackContext )
    {
        return FGuid();
    }

    AActor* Actor = Cast<AActor>( InObject );
    //if( Actor && Actor->ActorHasTag( "SequencerActor" ) )
    //{
    //    TOptional<FMovieSceneSpawnableAnnotation> Annotation = FMovieSceneSpawnableAnnotation::Find( Actor );
    //    if( Annotation.IsSet() && Annotation->OriginatingSequence == this )
    //    {
    //        return Annotation->ObjectBindingID;
    //    }

    //    // If this actor is a spawnable and is not in the same originating sequence, it's likely a spawnable that will be possessed.
    //    // SetSpawnableObjectBindingID will need to be called on that possessable.
    //}

    UObject* ParentObject = GetParentObject( InObject );
    FGuid    ParentGuid = ParentObject ? FindOrAddBinding( ParentObject ) : FGuid();

    if( ParentObject && !ParentGuid.IsValid() )
    {
        //UE_LOG( LogLevelSequence, Error, TEXT( "Unable to possess object '%s' because it's parent could not be bound." ), *InObject->GetName() );
        return FGuid();
    }

    // Perform a potentially slow lookup of every possessable binding in the sequence to see if we already have this
    {
        FSharedPlaybackStateCreateParams CreateParams;
        CreateParams.PlaybackContext = PlaybackContext;
        TSharedRef<FSharedPlaybackState> TransientPlaybackState = MakeShared<FSharedPlaybackState>( *this, CreateParams );

        FMovieSceneEvaluationState State;
        TransientPlaybackState->AddCapabilityRaw( &State );
        State.AssignSequence( MovieSceneSequenceID::Root, *this, TransientPlaybackState );

        FGuid ExistingID = State.FindObjectId( *InObject, MovieSceneSequenceID::Root, TransientPlaybackState );
        if( ExistingID.IsValid() )
        {
            return ExistingID;
        }
    }

    // We have to possess this object
    if( !CanPossessObject( *InObject, PlaybackContext ) )
    {
        return FGuid();
    }

    FString NewName = Actor ? Actor->GetActorLabel() : InObject->GetName();

    const FGuid NewGuid = MovieScene->AddPossessable( NewName, InObject->GetClass() );

    // Attempt to use the parent as a context if necessary
    UObject* BindingContext = ParentObject && AreParentContextsSignificant() ? ParentObject : PlaybackContext;

    // Set up parent/child guids for possessables within spawnables
    if( ParentGuid.IsValid() )
    {
        FMovieScenePossessable* ChildPossessable = MovieScene->FindPossessable( NewGuid );
        if( ensure( ChildPossessable ) )
        {
            ChildPossessable->SetParent( ParentGuid, MovieScene );
        }
    }

    BindPossessableObject( NewGuid, *InObject, BindingContext );

    return NewGuid;
}
#endif

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
    CameraBindingReferences.RemoveBinding( ObjectId );
    AnimationsBindingReferences.RemoveBinding( ObjectId );
    ActorsBindingReferences.RemoveBinding( ObjectId );
}

void UShotSequence::UnbindObjects( const FGuid& ObjectId, const TArray<UObject*>& InObjects, UObject* Context )
{
    CameraBindingReferences.RemoveObjects( ObjectId, InObjects, Context );
    AnimationsBindingReferences.RemoveObjects( ObjectId, InObjects, Context );
    ActorsBindingReferences.RemoveObjects( ObjectId, InObjects, Context );
}

void UShotSequence::UnbindInvalidObjects( const FGuid& ObjectId, UObject* Context )
{
    CameraBindingReferences.RemoveInvalidObjects( ObjectId, Context );
    AnimationsBindingReferences.RemoveInvalidObjects( ObjectId, Context );
    ActorsBindingReferences.RemoveInvalidObjects( ObjectId, Context );
}

#if WITH_EDITOR

ETrackSupport
UShotSequence::IsTrackSupportedImpl( TSubclassOf<class UMovieSceneTrack> InTrackClass ) const
{
    if( InTrackClass == UMovieSceneSingleCameraCutTrack::StaticClass() ||
        InTrackClass == UMovieSceneNoteTrack::StaticClass() ||
        InTrackClass == UMovieSceneAudioTrack::StaticClass() ||
        InTrackClass == UMovieSceneFadeTrack::StaticClass() ||
        InTrackClass == UMovieSceneMediaTrack::StaticClass() ||
        InTrackClass == UMovieSceneLevelVisibilityTrack::StaticClass() ||
        InTrackClass == UMovieSceneControlRigParameterTrack::StaticClass() ||
        InTrackClass == UMovieSceneSkeletalAnimationTrack::StaticClass() ||
        InTrackClass == UTemplateSequenceTrack::StaticClass() )
    {
        return ETrackSupport::Supported;
    }

    return ETrackSupport::NotSupported;
    //return Super::IsTrackSupported( InTrackClass );
}

#include "Filters/Filters/SequencerTrackFilters.h"
//#include "MediaSequencerFilters.h"
//#include "Sequencer/NiagaraSequence/NiagaraSequencerFilters.h"

// They can't be included, because a dependencies loop appears
// EposNamingConvention -> EposSequence -> EposTracksEditor -> EposNameingConvention
//#include "Filters/CinematicBoardSequencerFilters.h"
//#include "Filters/NoteSequencerFilters.h"
//#include "Filters/SingleCameraCutSequencerFilters.h"

bool
UShotSequence::IsFilterSupportedImpl( const FString& iFilterName ) const //override
{
    TArray<FString> validTrackFilters = {
        // Filters usable inside both board and shot sequence
        FSequencerTrackFilter_Audio::StaticName(),
        FSequencerTrackFilter_LevelVisibility::StaticName(),
        FSequencerTrackFilter_Fade::StaticName(),
        FSequencerTrackFilter_SkeletalMesh::StaticName(),
        FSequencerTrackFilter_Folder::StaticName(),
        TEXT( "Media" ), //FSequencerTrackFilter_Media::StaticName(), // private and header in cpp
        TEXT( "Niagara" ), //FSequencerTrackFilter_Niagara::StaticName(), // private and header in cpp
        TEXT( "Note" ),

        // Filters usable only inside shot sequence
        FSequencerTrackFilter_Camera::StaticName(),
        TEXT( "SingleCameraCut" ),
    };

    return validTrackFilters.Contains( iFilterName );
}

FText UShotSequence::GetDisplayName() const
{
    //return UMovieSceneSequence::GetDisplayName();

    FString name;
    bool is_formatted = mNamingFormatter->FormatName( this, name );
    if( !is_formatted )
        name = GetName();

    return FText::FromString( name );
}

void UShotSequence::GetAssetRegistryTags( FAssetRegistryTagsContext ioContext ) const
{
    Super::GetAssetRegistryTags( ioContext );

    if( CameraBindingReferences.GetAllReferences().Num() )
    {
        FString value;
        TArrayView<const FMovieSceneBindingReference> references = CameraBindingReferences.GetAllReferences();
        for( const FMovieSceneBindingReference& reference : references )
        {
            FMovieScenePossessable* possessable = MovieScene->FindPossessable( reference.ID );
            if( possessable )
            {
                value = possessable->GetName();
                //value = MovieScene->GetObjectDisplayName( binding ).ToString();

                break; // Should be only one camera root
            }
        }

        ioContext.AddTag( { "Camera", value, FAssetRegistryTag::TT_Alphabetical } );
    }
    else
    {
        ioContext.AddTag( { "Camera", "(None)", FAssetRegistryTag::TT_Alphabetical } );
    }

    if( AnimationsBindingReferences.GetAllReferences().Num() )
    {
        int animation_count = 0;
        TArrayView<const FMovieSceneBindingReference> references = AnimationsBindingReferences.GetAllReferences();
        for( const FMovieSceneBindingReference& reference : references )
        {
            FMovieScenePossessable* possessable = MovieScene->FindPossessable( reference.ID );
            if( possessable && !possessable->GetParent().IsValid() /* to get only root animations */ )
            {
                animation_count++;
            }
        }

        ioContext.AddTag( { "Animations", FString::FromInt( animation_count ), FAssetRegistryTag::TT_Alphabetical } );
    }
    else
    {
        ioContext.AddTag( { "Animations", "(0)", FAssetRegistryTag::TT_Alphabetical } );
    }

    if( ActorsBindingReferences.GetAllReferences().Num() )
    {
        int actor_count = 0;
        TArrayView<const FMovieSceneBindingReference> references = ActorsBindingReferences.GetAllReferences();
        for( const FMovieSceneBindingReference& reference : references )
        {
            FMovieScenePossessable* possessable = MovieScene->FindPossessable( reference.ID );
            if( possessable && !possessable->GetParent().IsValid() /* to get only root actors */ )
            {
                actor_count++;
            }
        }

        ioContext.AddTag( { "Actors", FString::FromInt( actor_count ), FAssetRegistryTag::TT_Alphabetical } );
    }
    else
    {
        ioContext.AddTag( { "Actors", "(0)", FAssetRegistryTag::TT_Alphabetical } );
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
        "Animations",
        FAssetRegistryTagMetadata()
        .SetDisplayName( NSLOCTEXT( "ShotSequence", "Animations_Label", "Animations in shot" ) )
        .SetTooltip( NSLOCTEXT( "ShotSequence", "Animations_Tooltip", "The animations bound to this shot sequence" ) )
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

    //---

    TArray<UMovieSceneSection*> sections_to_stretch;
    TArray<UMovieSceneTrack*> filtered_tracks;

    // "Master" tracks if needed
    //TArray<UMovieSceneTrack*> tracks = movie_scene->GetTracks();

    TArray<FMovieSceneBinding> bindings = ((const UMovieScene*)movie_scene)->GetBindings();
    for( FMovieSceneBinding binding : bindings )
    {
        TArray<UMovieSceneTrack*> tracks = binding.GetTracks();
        for( UMovieSceneTrack* track : tracks )
        {
            UOdysseyAnimationTimelineTrack* timeline_track = Cast<UOdysseyAnimationTimelineTrack>( track );
            if( timeline_track )
                filtered_tracks.Add( timeline_track );

            UMovieSceneVisibilityTrack* visibility_track = Cast<UMovieSceneVisibilityTrack>( track );
            if( visibility_track )
                filtered_tracks.Add( visibility_track );
        }
    }

    for( UMovieSceneTrack* track : filtered_tracks )
    {
        TArray<UMovieSceneSection*> sections = track->GetAllSections();
        for( UMovieSceneSection* section : sections )
        {
            if( movie_scene->GetPlaybackRange() == section->GetTrueRange() )
                sections_to_stretch.Add( section );
        }
    }

    //---

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

    //---

    for( UMovieSceneSection* section_to_stretch : sections_to_stretch )
    {
        section_to_stretch->SetRange( new_range );
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
