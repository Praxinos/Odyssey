// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Board/BoardSequence.h"

#include "Animation/SkeletalMeshActor.h"
#include "Bindings/MovieSceneSpawnableBinding.h"
#include "Components/ActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "MediaPlate.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "MovieScene.h"
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
#include "SubObjectLocator.h"
#include "UniversalObjectLocators/ActorLocatorFragment.h"
#include "UniversalObjectLocators/AnimInstanceLocatorFragment.h"
#include "Sequencer/MovieSceneControlRigParameterTrack.h"
#include "Tracks/TemplateSequenceTrack.h"

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

#if WITH_EDITOR
    MovieScene->SetPlaybackRangeLocked( true );
#endif
}

void UBoardSequence::PostLoad()
{
    Super::PostLoad();

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

#if WITH_EDITOR
#endif
}

const FMovieSceneBindingReferences* UBoardSequence::GetBindingReferences() const //override
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

void UBoardSequence::BindPossessableObject(const FGuid& ObjectId, UObject& PossessedObject, UObject* Context)
{
    if( Context )
    {
        BindingReferences.AddBinding( ObjectId, &PossessedObject, Context );
    }
}

bool UBoardSequence::CanPossessObject(UObject& Object, UObject* InPlaybackContext) const
{
    return Object.IsA<AStaticMeshActor>()
        || Object.IsA<ASkeletalMeshActor>()
        || Object.IsA<UActorComponent>()
        || Object.IsA<ANiagaraActor>()
        || Object.IsA<APaperFlipbookActor>()
        || Object.IsA<AMediaPlate>()
        || ExactCast<AActor>( &Object ); // Empty Actor
}

bool UBoardSequence::CanRebindPossessable( const FMovieScenePossessable& InPossessable ) const
{
    return !InPossessable.GetParent().IsValid();
}

// From LevelSequence.cpp
FGuid UBoardSequence::FindBindingFromObject( UObject* InObject, TSharedRef<const UE::MovieScene::FSharedPlaybackState> SharedPlaybackState ) const
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
void UBoardSequence::GatherExpiredObjects( const FMovieSceneObjectCache& InObjectCache, TArray<FGuid>& OutInvalidIDs ) const
{
    using namespace UE::UniversalObjectLocator;

    TArrayView<const FMovieSceneBindingReference> References = BindingReferences.GetAllReferences();
    for( int32 Index = 0; Index < References.Num(); ++Index )
    {
        const FMovieSceneBindingReference& Reference = References[Index];

        if( Reference.Locator.GetLastFragmentTypeHandle() == FAnimInstanceLocatorFragment::FragmentType )
        {
            for( TWeakObjectPtr<> WeakObject : InObjectCache.IterateBoundObjects( Reference.ID ) )
            {
                UAnimInstance* AnimInstance = Cast<UAnimInstance>( WeakObject.Get() );
                if( !AnimInstance || !AnimInstance->GetOwningComponent() || AnimInstance->GetOwningComponent()->GetAnimInstance() != AnimInstance )
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

UObject* UBoardSequence::MakeSpawnableTemplateFromInstance( UObject& InSourceObject, FName ObjectName )
{
    return MovieSceneHelpers::MakeSpawnableTemplateFromInstance( InSourceObject, MovieScene, ObjectName );
}

bool UBoardSequence::AllowsSpawnableObjects() const
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

bool UBoardSequence::AllowsCustomBindings() const
{
    return true;
}

#if WITH_EDITOR
FGuid UBoardSequence::CreatePossessable( UObject* ObjectToPossess ) //override
{
    return FindOrAddBinding( ObjectToPossess );
}
// From LevelSequence.cpp
FGuid UBoardSequence::CreateSpawnable( UObject* ObjectToSpawn ) //override
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
FGuid UBoardSequence::FindOrAddBinding( UObject* InObject )
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
    BindingReferences.RemoveBinding( ObjectId );
}

void UBoardSequence::UnbindObjects(const FGuid& ObjectId, const TArray<UObject*>& InObjects, UObject* Context)
{
    BindingReferences.RemoveObjects( ObjectId, InObjects, Context );
}

void UBoardSequence::UnbindInvalidObjects(const FGuid& ObjectId, UObject* Context)
{
    BindingReferences.RemoveInvalidObjects( ObjectId, Context );
}

#if WITH_EDITOR

ETrackSupport
UBoardSequence::IsTrackSupportedImpl( TSubclassOf<class UMovieSceneTrack> InTrackClass ) const
{
    if( InTrackClass == UMovieSceneCinematicBoardTrack::StaticClass() ||
        InTrackClass == UMovieSceneSingleCameraCutTrack::StaticClass() || // The board sequence needs to support this track type, otherwise when opening a board sequence, inner shot sequence won't have this track available (even if supported in this class), because during opening the toolkit, all the track editors are not called
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
UBoardSequence::IsFilterSupportedImpl( const FString& iFilterName ) const //override
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

        // Filters usable only inside board sequence
        TEXT( "CinematicBoard" ),

        // Filters usable only inside shot sequence
        // But the filter is always builded on the root sequence of the hierarchy
        // So add those filters here
        FSequencerTrackFilter_Camera::StaticName(),
        TEXT( "SingleCameraCut" ),
    };

    return validTrackFilters.Contains( iFilterName );
}

FText UBoardSequence::GetDisplayName() const
{
    //return UMovieSceneSequence::GetDisplayName();

    FString name;
    bool is_formatted = mNamingFormatter->FormatName( this, name );
    if( !is_formatted )
        name = GetName();

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
