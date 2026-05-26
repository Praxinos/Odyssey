// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shot/ShotSequence.h"

#include "Animation/SkeletalMeshActor.h"
#include "Bindings/MovieSceneSpawnableBinding.h"
#include "CineCameraActor.h"
#include "Components/ActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
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
#include "Tracks/MovieScene3DAttachTrack.h"
#include "Tracks/MovieSceneFadeTrack.h"
#include "Tracks/MovieSceneLevelVisibilityTrack.h"
#include "Tracks/MovieSceneAudioTrack.h"
#include "Tracks/MovieSceneSkeletalAnimationTrack.h"
#include "Tracks/MovieSceneSpawnTrack.h"
#include "Tracks/MovieSceneVisibilityTrack.h"
#include "UObject/AssetRegistryTagsContext.h"
#include "SubObjectLocator.h"
#include "UniversalObjectLocators/ActorLocatorFragment.h"
#include "UniversalObjectLocators/AnimInstanceLocatorFragment.h"
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

            BindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
        }
        else
        {
            FUniversalObjectLocator NewLocator;
            NewLocator.AddFragment<FActorLocatorFragment>( MoveTemp( legacy_ref.ExternalObjectPath ) );

            BindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
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

            BindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
        }
        else
        {
            FUniversalObjectLocator NewLocator;
            NewLocator.AddFragment<FActorLocatorFragment>( MoveTemp( legacy_ref.ExternalObjectPath ) );

            BindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
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

            BindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
        }
        else
        {
            FUniversalObjectLocator NewLocator;
            NewLocator.AddFragment<FActorLocatorFragment>( MoveTemp( legacy_ref.ExternalObjectPath ) );

            BindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
        }
    }

    ActorsBindingIdToReferences_DEPRECATED.Empty();

    //-

    for( FMovieSceneBindingReference reference : PlanesBindingReferences_DEPRECATED.GetAllReferences() )
    {
        FUniversalObjectLocator NewLocator( reference.Locator );
        BindingReferences.FMovieSceneBindingReferences::AddBinding( reference.ID, MoveTemp( NewLocator ), reference.ResolveFlags, reference.CustomBinding );
    }
    for( FMovieSceneBindingReference reference : PlanesBindingReferences_DEPRECATED.GetAllReferences() )
        PlanesBindingReferences_DEPRECATED.RemoveBinding( reference.ID );

    for( FMovieSceneBindingReference reference : CameraBindingReferences_DEPRECATED.GetAllReferences() )
    {
        FUniversalObjectLocator NewLocator( reference.Locator );
        BindingReferences.FMovieSceneBindingReferences::AddBinding( reference.ID, MoveTemp( NewLocator ), reference.ResolveFlags, reference.CustomBinding );
    }
    for( FMovieSceneBindingReference reference : CameraBindingReferences_DEPRECATED.GetAllReferences() )
        CameraBindingReferences_DEPRECATED.RemoveBinding( reference.ID );

    for( FMovieSceneBindingReference reference : AnimationsBindingReferences_DEPRECATED.GetAllReferences() )
    {
        FUniversalObjectLocator NewLocator( reference.Locator );
        BindingReferences.FMovieSceneBindingReferences::AddBinding( reference.ID, MoveTemp( NewLocator ), reference.ResolveFlags, reference.CustomBinding );
    }
    for( FMovieSceneBindingReference reference : AnimationsBindingReferences_DEPRECATED.GetAllReferences() )
        AnimationsBindingReferences_DEPRECATED.RemoveBinding( reference.ID );

    for( FMovieSceneBindingReference reference : ActorsBindingReferences_DEPRECATED.GetAllReferences() )
    {
        FUniversalObjectLocator NewLocator( reference.Locator );
        BindingReferences.FMovieSceneBindingReferences::AddBinding( reference.ID, MoveTemp( NewLocator ), reference.ResolveFlags, reference.CustomBinding );
    }
    for( FMovieSceneBindingReference reference : ActorsBindingReferences_DEPRECATED.GetAllReferences() )
        ActorsBindingReferences_DEPRECATED.RemoveBinding( reference.ID );

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
    //return nullptr;

    return &BindingReferences;
}

void UShotSequence::BindPossessableObject( const FGuid& ObjectId, UObject& PossessedObject, UObject* Context )
{
    if( Context )
    {
        BindingReferences.AddBinding( ObjectId, &PossessedObject, Context );
    }
}

bool UShotSequence::CanPossessObject( UObject& Object, UObject* InPlaybackContext ) const
{
    // If one day we need to filter new object on existing actors in sequencer (as long as epic don't update this function or in a more global behavior)
    // https://epicgames.slack.com/archives/C085QAADA9J/p1769785438964089

    //if( Object.IsA<ACineCameraActor>() )
    //{
    //    int number_of_camera = 0;

    //    //using namespace UE::MovieScene;

    //    //TSharedRef<FSharedPlaybackState> TransientPlaybackState = MovieSceneHelpers::CreateTransientSharedPlaybackState( InPlaybackContext, const_cast<UShotSequence*>( this ) );

    //    //TArrayView<const FMovieSceneBindingReference> references = BindingReferences.GetAllReferences();
    //    //for( const FMovieSceneBindingReference& reference : references )
    //    //{
    //    //    UObject* object = MovieSceneHelpers::GetSingleBoundObject( const_cast<UShotSequence*>( this ), reference.ID, TransientPlaybackState );
    //    //    //MovieSceneHelpers::GetBoundObjectClass( this, reference.ID );

    //    //    if( object->IsA<ACineCameraActor>() )
    //    //        number_of_camera++;
    //    //}

    //    //if( number_of_camera >= 1 )
    //    //    return false;

    //    for( int i = 0; i < GetMovieScene()->GetPossessableCount(); i++ )
    //    {
    //        FMovieScenePossessable& possessable = GetMovieScene()->GetPossessable( i );

    //        if( possessable.GetPossessedObjectClass()->IsChildOf<ACineCameraActor>() )
    //            number_of_camera++;
    //    }

    //    if( number_of_camera >= 2 )
    //        return false;
    //}

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

// From LevelSequence.cpp
FGuid UShotSequence::FindBindingFromObject( UObject* InObject, TSharedRef<const UE::MovieScene::FSharedPlaybackState> SharedPlaybackState ) const
{
    if( InObject )
    {
        if( FMovieSceneEvaluationState* EvaluationState = SharedPlaybackState->FindCapability<FMovieSceneEvaluationState>() )
        {
            FMovieSceneSequenceID SequenceID = EvaluationState->FindSequenceId( this );
            return EvaluationState->FindCachedObjectId( *InObject, SequenceID, SharedPlaybackState );
        }
    }
    return FGuid();
}

// From LevelSequence.cpp
void UShotSequence::GatherExpiredObjects( const FMovieSceneObjectCache& InObjectCache, TArray<FGuid>& OutInvalidIDs ) const
{
    using namespace UE::UniversalObjectLocator;

    TArrayView<const FMovieSceneBindingReference> References = BindingReferences.GetAllReferences();
    for( int32 Index = 0; Index < References.Num(); ++Index )
    {
        const FMovieSceneBindingReference& Reference = References[Index];

        if( Reference.Locator.GetLastFragmentTypeHandle() == FAnimInstanceLocatorFragment::FragmentType )
        {
            // Get anim instance fragment to determine type.
            EAnimInstanceLocatorFragmentType LocatorType = EAnimInstanceLocatorFragmentType::AnimInstance;
            if( const FAnimInstanceLocatorFragment* AnimFragment = Reference.Locator.GetLastFragment()->GetPayloadAs( FAnimInstanceLocatorFragment::FragmentType ) )
            {
                LocatorType = AnimFragment->Type;
            }

            for( TWeakObjectPtr<> WeakObject : InObjectCache.IterateBoundObjects( Reference.ID ) )
            {
                UAnimInstance* AnimInstance = Cast<UAnimInstance>( WeakObject.Get() );
                UAnimInstance* AnimInstanceToTest = nullptr;
                if( AnimInstance && AnimInstance->GetOwningComponent() )
                {
                    //Check if we're a regular or post-process anim instance and if the owning component's anim instance has changed.
                    AnimInstanceToTest = LocatorType == EAnimInstanceLocatorFragmentType::PostProcessAnimInstance ?
                        AnimInstance->GetOwningComponent()->GetPostProcessInstance() :
                        AnimInstance->GetOwningComponent()->GetAnimInstance();
                }
                if( !AnimInstance || !AnimInstance->GetOwningComponent() || AnimInstanceToTest != AnimInstance )
                {
                    OutInvalidIDs.Add( Reference.ID );
                }
            }

            // Skip over subsequent matched IDs
            while( Index < References.Num() - 1 && References[Index + 1].ID == Reference.ID )
            {
                ++Index;
            }
        }
    }
}

UObject* UShotSequence::MakeSpawnableTemplateFromInstance( UObject& InSourceObject, FName ObjectName )
{
    return MovieSceneHelpers::MakeSpawnableTemplateFromInstance( InSourceObject, MovieScene, ObjectName );
}

bool UShotSequence::AllowsSpawnableObjects() const
{
    TArray<const TSubclassOf<UMovieSceneCustomBinding>> CustomBindingTypes;

    MovieSceneHelpers::GetPrioritySortedCustomBindingTypes( CustomBindingTypes );
    for( const TSubclassOf<UMovieSceneCustomBinding>& CustomBindingType : CustomBindingTypes )
    {
        const bool bIsCustomSpawnableBinding = CustomBindingType->IsChildOf<UMovieSceneSpawnableBindingBase>();
        if( bIsCustomSpawnableBinding )
        {
            return true;
        }
    }
    return false;
}

bool UShotSequence::AllowsCustomBindings() const
{
    return true;
}

#if WITH_EDITOR
FGuid UShotSequence::CreatePossessable( UObject* ObjectToPossess ) //override
{
    return FindOrAddBinding( ObjectToPossess );
}
// From LevelSequence.cpp
FGuid UShotSequence::CreateSpawnable( UObject* ObjectToSpawn ) //override
{
    if( !MovieScene || !ObjectToSpawn )
    {
        return FGuid();
    }

    FGuid NewGuid = MovieSceneHelpers::TryCreateCustomSpawnableBinding( this, ObjectToSpawn );

    UMovieSceneSpawnTrack* NewSpawnTrack = MovieScene->AddTrack<UMovieSceneSpawnTrack>( NewGuid );
    if( NewSpawnTrack )
    {
        NewSpawnTrack->Modify();

        NewSpawnTrack->AddSection( *NewSpawnTrack->CreateNewSection() );
    }
    return NewGuid;
}

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
    //CameraBindingReferences.RemoveBinding( ObjectId );
    //AnimationsBindingReferences.RemoveBinding( ObjectId );
    //ActorsBindingReferences.RemoveBinding( ObjectId );
    BindingReferences.RemoveBinding( ObjectId );
}

void UShotSequence::UnbindObjects( const FGuid& ObjectId, const TArray<UObject*>& InObjects, UObject* Context )
{
    //CameraBindingReferences.RemoveObjects( ObjectId, InObjects, Context );
    //AnimationsBindingReferences.RemoveObjects( ObjectId, InObjects, Context );
    //ActorsBindingReferences.RemoveObjects( ObjectId, InObjects, Context );
    BindingReferences.RemoveObjects( ObjectId, InObjects, Context );
}

void UShotSequence::UnbindInvalidObjects( const FGuid& ObjectId, UObject* Context )
{
    //CameraBindingReferences.RemoveInvalidObjects( ObjectId, Context );
    //AnimationsBindingReferences.RemoveInvalidObjects( ObjectId, Context );
    //ActorsBindingReferences.RemoveInvalidObjects( ObjectId, Context );
    BindingReferences.RemoveInvalidObjects( ObjectId, Context );
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
        InTrackClass == UTemplateSequenceTrack::StaticClass() ||
        InTrackClass == UMovieSceneSpawnTrack::StaticClass() ||
        InTrackClass == UMovieScene3DAttachTrack::StaticClass() )
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

    FString camera_name;
    int animation_count = 10;
    int actor_count = 10;
    if( BindingReferences.GetAllReferences().Num() )
    {
        TArrayView<const FMovieSceneBindingReference> references = BindingReferences.GetAllReferences();
        for( const FMovieSceneBindingReference& reference : references )
        {
            //FMovieSceneBinding* binding = MovieScene->FindBinding( reference.ID );
            FMovieScenePossessable* possessable = MovieScene->FindPossessable( reference.ID );

            if( !possessable || possessable->GetParent().IsValid() ) // not a possessable or a component
                continue;

            if( !ensure( possessable->GetLoadedPossessedObjectClass() ) )
                continue;

            if( possessable->GetLoadedPossessedObjectClass()->IsChildOf<ACineCameraActor>() )
                camera_name = possessable->GetName();
            else if( possessable->GetLoadedPossessedObjectClass()->IsChildOf<AOdysseyAnimationActor>() )
                animation_count++;
            else
                actor_count++;
        }

        if( camera_name.IsEmpty() )
            camera_name = TEXT("(None)");

        ioContext.AddTag( { "Camera", camera_name, FAssetRegistryTag::TT_Alphabetical } );
        ioContext.AddTag( { "Animations", FString::FromInt( animation_count ), FAssetRegistryTag::TT_Alphabetical } );
        ioContext.AddTag( { "Actors", FString::FromInt( actor_count ), FAssetRegistryTag::TT_Alphabetical } );
    }
    else
    {
        ioContext.AddTag( { "Camera", TEXT("(Nothing)"), FAssetRegistryTag::TT_Alphabetical } );
        ioContext.AddTag( { "Animations", TEXT("(Nothing)"), FAssetRegistryTag::TT_Alphabetical } );
        ioContext.AddTag( { "Actors", TEXT("(Nothing)"), FAssetRegistryTag::TT_Alphabetical } );
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
