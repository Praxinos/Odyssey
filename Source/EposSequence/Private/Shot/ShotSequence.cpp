// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
#include "UObject/AssetRegistryTagsContext.h"
#include "SubObjectLocator.h"
#include "UniversalObjectLocators/ActorLocatorFragment.h"

#include "Board/BoardHelpers.h"
#include "EposSequenceModule.h"
#include "INamingFormatter.h"
#include "PlaneActor.h"
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

            PlanesBindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
        }
        else
        {
            FUniversalObjectLocator NewLocator;
            NewLocator.AddFragment<FActorLocatorFragment>( MoveTemp( legacy_ref.ExternalObjectPath ) );

            PlanesBindingReferences.FMovieSceneBindingReferences::AddBinding( pair.Key, MoveTemp( NewLocator ) );
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
    // so the check of possible possessables object (Camera/Plane/...) must be done there
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
    else if( PossessedObject.IsA<APlaneActor>()
             || PossessedObject.IsA<UActorComponent>() && PossessedObject.GetTypedOuter<APlaneActor>() )
    {
        if( Context )
        {
            PlanesBindingReferences.AddBinding( ObjectId, &PossessedObject, Context );
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
        || ExactCast<AActor>( &Object ); // Empty Actor
}

bool UShotSequence::CanRebindPossessable( const FMovieScenePossessable& InPossessable ) const
{
    return !InPossessable.GetParent().IsValid();
}

void UShotSequence::LocateBoundObjects( const FGuid& ObjectId, UObject* Context, TArray<UObject*, TInlineAllocator<1>>& OutObjects ) const
{
    CameraBindingReferences.ResolveBinding( ObjectId, Context, OutObjects );
    PlanesBindingReferences.ResolveBinding( ObjectId, Context, OutObjects );
    ActorsBindingReferences.ResolveBinding( ObjectId, Context, OutObjects );
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
    CameraBindingReferences.RemoveBinding( ObjectId );
    PlanesBindingReferences.RemoveBinding( ObjectId );
    ActorsBindingReferences.RemoveBinding( ObjectId );
}

void UShotSequence::UnbindObjects( const FGuid& ObjectId, const TArray<UObject*>& InObjects, UObject* Context )
{
    CameraBindingReferences.RemoveObjects( ObjectId, InObjects, Context );
    PlanesBindingReferences.RemoveObjects( ObjectId, InObjects, Context );
    ActorsBindingReferences.RemoveObjects( ObjectId, InObjects, Context );
}

void UShotSequence::UnbindInvalidObjects( const FGuid& ObjectId, UObject* Context )
{
    CameraBindingReferences.RemoveInvalidObjects( ObjectId, Context );
    PlanesBindingReferences.RemoveInvalidObjects( ObjectId, Context );
    ActorsBindingReferences.RemoveInvalidObjects( ObjectId, Context );
}

#if WITH_EDITOR

ETrackSupport
UShotSequence::IsTrackSupported( TSubclassOf<class UMovieSceneTrack> InTrackClass ) const
{
    if( !UMovieScene::IsTrackClassAllowed( InTrackClass ) )
    {
        return ETrackSupport::NotSupported;
    }

    if( InTrackClass == UMovieSceneSingleCameraCutTrack::StaticClass() ||
        InTrackClass == UMovieSceneNoteTrack::StaticClass() ||
        InTrackClass == UMovieSceneAudioTrack::StaticClass() ||
        InTrackClass == UMovieSceneFadeTrack::StaticClass() ||
        InTrackClass == UMovieSceneMediaTrack::StaticClass() ||
        InTrackClass == UMovieSceneLevelVisibilityTrack::StaticClass() ||
        InTrackClass == UMovieSceneSkeletalAnimationTrack::StaticClass() )
    {
        return ETrackSupport::Supported;
    }

    return ETrackSupport::NotSupported;
    //return Super::IsTrackSupported( InTrackClass );
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

    if( PlanesBindingReferences.GetAllReferences().Num() )
    {
        int plane_count = 0;
        TArrayView<const FMovieSceneBindingReference> references = PlanesBindingReferences.GetAllReferences();
        for( const FMovieSceneBindingReference& reference : references )
        {
            FMovieScenePossessable* possessable = MovieScene->FindPossessable( reference.ID );
            if( possessable && !possessable->GetParent().IsValid() /* to get only root planes */ )
            {
                plane_count++;
            }
        }

        ioContext.AddTag( { "Planes", FString::FromInt( plane_count ), FAssetRegistryTag::TT_Alphabetical } );
    }
    else
    {
        ioContext.AddTag( { "Planes", "(0)", FAssetRegistryTag::TT_Alphabetical } );
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
