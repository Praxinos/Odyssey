// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/EposSequenceTools.h"

#include "Bindings/MovieSceneSpawnableActorBinding.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Framework/Notifications/NotificationManager.h"
#include "ISequencer.h"
#include "KeyframeTrackEditor.h"
#include "LevelEditorSubsystem.h"
#include "LevelEditorViewport.h"
#include "LevelUtils.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"
#include "SequencerUtilities.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "ActorHelpers.h"
#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "OdysseyAnimationActor.h"
#include "ScalingComponent.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools_Camera"

//---

//static
void
BoardSequenceTools::CreateCameraWithAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, const FCameraArgs& iCameraArgs, const FAnimationArgs& iAnimationArgs )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    return ShotSequenceTools::CreateCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iCameraArgs, &iAnimationArgs );
}

//static
void
BoardSequenceTools::CreateCameraWithAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, const FCameraArgs& iCameraArgs, const FAnimationArgs& iAnimationArgs )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    ShotSequenceTools::CreateCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iCameraArgs, &iAnimationArgs );
}

//static
bool
BoardSequenceTools::CanCreateCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
    if( camera_binding.IsValid() )
        return false;

    return true;
}

//static
void
ShotSequenceTools::CreateCameraWithAnimation( ISequencer* iSequencer, const FCameraArgs& iCameraArgs, const FAnimationArgs& iAnimationArgs )
{
    CreateCamera( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iCameraArgs, &iAnimationArgs );
}

//static
bool
ShotSequenceTools::CanCreateCamera( ISequencer* iSequencer )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    FMovieSceneSequenceID sequence_id = iSequencer->GetFocusedTemplateID();
    if( !sequence )
        return false;

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, sequence, sequence_id );
    if( camera_binding.IsValid() )
        return false;

    return true;
}

//static
void
ShotSequenceTools::CreateCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FCameraArgs& iCameraArgs, const FAnimationArgs* iAnimationArgs )
{
    UMovieScene* movieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !movieScene )
        return;

    if( movieScene->IsReadOnly() )
    {
        //ShowReadOnlyError();
        return;
    }

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( iSequencer, iSequence, iSequenceID );
    if( camera_binding.IsValid() )
        return;

    ULevelEditorSubsystem* levelEditorSubsystem = GEditor->GetEditorSubsystem<ULevelEditorSubsystem>();
    if( FLevelUtils::IsLevelLocked( levelEditorSubsystem->GetCurrentLevel() ) )
    {
        FNotificationInfo Info( LOCTEXT( "cant-spawn-camera-in-locked-level", "The requested operation could not be completed because the level is locked." ) );
        Info.ExpireDuration = 5.0f;
        TSharedPtr<SNotificationItem> notification = FSlateNotificationManager::Get().AddNotification( Info );
        if (notification)
            notification->SetCompletionState( SNotificationItem::CS_Fail );
        return;
    }

    //---

    const FScopedTransaction transaction( LOCTEXT( "transaction.create-storycamera-here", "Create Storyboard Camera Here" ) );

    TArray<AActor*> actors;
    TOptional<FFrameTime> start_sequence_in_storyboard;

    {
        cTemporarySwitchInner switch_to( iSequencer, iSequenceID );

        //---

        FGuid camera_guid;
        ACineCameraActor* camera = ShotSequenceTools::SpawnAndBindCamera( iSequencer, iSequence, iSequenceID, iCameraArgs, &camera_guid );
        if( !camera )
            return;

        actors = ShotSequenceTools::CameraAdded( iSequencer, iSequence, iSequenceID, camera_guid, camera, iSequencer.GetLocalTime().Time.FloorToFrame(), iAnimationArgs );

        //---

        FMovieSceneInverseSequenceTransform localToRootTransform = iSequencer.GetFocusedMovieSceneSequenceTransform().Inverse();
        start_sequence_in_storyboard = localToRootTransform.TryTransformTime( 0 );

        iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately );
    }

    // Must be done after the inner/outer sequence switch (that's why it is in its own block)
    // Otherwise it resets the selection if GEditor->SelectActor() is called inside CameraAdded()
    // Furthermore the hidden flag must also be set to true now
    iSequencer.EmptySelection();
    GEditor->SelectNone( true /*bNoteSelectionChange*/, true /*bDeselectBSPSurfs*/ );
    if( actors.Num() )
        GEditor->SelectActor( actors[0], true /*bInSelected*/, true /*bNotify*/, true /*bSelectEvenIfHidden*/ );

    if( start_sequence_in_storyboard )
        iSequencer.SetGlobalTime( *start_sequence_in_storyboard, true /* Evaluate */ );
}


//static
ACineCameraActor*
ShotSequenceTools::SpawnCamera( UWorld* iWorld, const FTransform& iTransform )
{
    check( !FLevelUtils::IsLevelLocked( iWorld->GetCurrentLevel() ) );

    // Set new camera to match viewport
    FActorSpawnParameters SpawnParams;
    ACineCameraActor* camera = iWorld->SpawnActor<ACineCameraActor>( SpawnParams );
    if( !camera )
        return nullptr;

    camera->SetActorTransform( iTransform );
    //camera->CameraComponent->FieldOfView = ViewportClient->ViewFOV; //@todo set the focal length from this field of view

    const UEposTracksEditorSettings* settings = GetDefault<UEposTracksEditorSettings>();

    // https://udn.unrealengine.com/s/question/0D54z00006uhl34CAA/plugin-cuproperty-how-to-change-uproperty-and-trigger-prepostedit-
    //UCineCameraComponent* CameraComponent = camera->GetCineCameraComponent();
    //if( CameraComponent != nullptr )
    //{
    //    {
    //        FProperty* ChangedProperty = FindFProperty<FProperty>( UCineCameraComponent::StaticClass(), "LensSettings" );
    //        CameraComponent->PreEditChange( ChangedProperty );

    //        CameraComponent->LensSettings = settings->CameraSettings.LensSettings;

    //        FPropertyChangedEvent PropertyChangedEvent( ChangedProperty );
    //        //CameraComponent->PostEditChangeProperty( PropertyChangedEvent );
    //        FEditPropertyChain PropertyChain;
    //        PropertyChain.AddHead( ChangedProperty );
    //        FPropertyChangedChainEvent PropertyChainEvent( PropertyChain, PropertyChangedEvent );
    //        CameraComponent->PostEditChangeChainProperty( PropertyChainEvent );
    //    }
    //    ... do it for all properties to change
    //}

    camera->GetCineCameraComponent()->LensSettings = settings->CameraSettings.LensSettings;
    camera->GetCineCameraComponent()->Filmback = settings->CameraSettings.Filmback;
    camera->GetCineCameraComponent()->FocusSettings = settings->CameraSettings.FocusSettings;
    camera->GetCineCameraComponent()->CurrentAperture = settings->CameraSettings.CurrentAperture;
    camera->GetCineCameraComponent()->SetCurrentFocalLength( settings->CameraSettings.CurrentFocalLength ); // Use setter to trigger RecalcDerivedData(), so no need to call Pre/PostChange() and its huge syntax

    return camera;
}

//static
ACineCameraActor*
ShotSequenceTools::SpawnAndBindCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FCameraArgs& iCameraArgs, FGuid* oGuid ) // From FSequencer::CreateCamera()
{
    if( !GCurrentLevelEditingViewportClient )
        return nullptr;

    UWorld* world = GCurrentLevelEditingViewportClient->GetWorld();
    FTransform transform( GCurrentLevelEditingViewportClient->GetViewTransform().GetRotation(), GCurrentLevelEditingViewportClient->GetViewTransform().GetLocation() );

    check( !FLevelUtils::IsLevelLocked( world->GetCurrentLevel() ) );

    ACineCameraActor* camera = SpawnCamera( world, transform );

    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequence );
    check( epos_sequence );

    //---

    FString camera_path;
    FString camera_name;
    NamingConvention::GenerateCameraActorPathName( iSequencer, *epos_sequence, iSequenceID, !iCameraArgs.mSpawnable, camera_path, camera_name );

    if( !iCameraArgs.mName.IsEmpty() )
        camera_name = iCameraArgs.mName;

    camera->SetFolderPath( *camera_path );
    FActorLabelUtilities::RenameExistingActor( camera, camera_name, false ); // The shot name is displayed in another column in the world outliner

    camera_name = NamingConvention::GenerateCameraTrackName( iSequencer, *epos_sequence, iSequenceID, camera );

    UE::Sequencer::FCreateBindingParams BindingParams;
    BindingParams.BindingNameOverride = camera_name;
    BindingParams.bAllowCustomBinding = true;
    FGuid CameraGuid = iSequencer.CreateBinding( *camera, BindingParams );
    if( !CameraGuid.IsValid() )
        return nullptr;

    //---

    if( iCameraArgs.mSpawnable )
    {
        TSubclassOf<UMovieSceneCustomBinding> CustomBindingClass = UMovieSceneSpawnableActorBinding::StaticClass();

        const FMovieSceneBindingReferences* BindingReferences = iSequence->GetBindingReferences();

        if( BindingReferences )
        {
            for( const FMovieSceneBindingReference& Reference : BindingReferences->GetReferences( CameraGuid ) )
            {
                for( const TSubclassOf<UMovieSceneCustomBinding>& SupportedCustomBindingType : iSequencer.GetSupportedCustomBindingTypes() )
                {
                    if( SupportedCustomBindingType && SupportedCustomBindingType->IsChildOf( CustomBindingClass ) &&
                        SupportedCustomBindingType->GetDefaultObject<UMovieSceneCustomBinding>()->SupportsConversionFromBinding( Reference, camera ) )
                    {
                        FMovieScenePossessable* NewPossessable = FSequencerUtilities::ConvertToCustomBinding( iSequencer.AsShared(), CameraGuid, CustomBindingClass );

                        if( NewPossessable )
                        {
                            for( TWeakObjectPtr<> WeakObject : iSequencer.FindBoundObjects( NewPossessable->GetGuid(), iSequenceID ) )
                            {
                                ACineCameraActor* SpawnedActor = Cast<ACineCameraActor>( WeakObject.Get() );
                                if( SpawnedActor )
                                {
                                    camera = SpawnedActor;
                                }
                            }

                            CameraGuid = NewPossessable->GetGuid();
                        }
                        break;
                    }
                }
            }
        }
    }

    //---

    iSequencer.OnActorAddedToSequencer().Broadcast( camera, CameraGuid );

    //---

    MovieSceneToolHelpers::LockCameraActorToViewport( iSequencer.AsShared(), camera );

    *oGuid = CameraGuid;
    return camera;
}

//static
TArray<AActor*>
ShotSequenceTools::CameraAdded( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid CameraGuid, ACineCameraActor* iCamera, FFrameNumber FrameNumber, const FAnimationArgs* iAnimationArgs )
{
    TArray<AActor*> actors;

    CreateCameraCut( iSequencer, iSequence, CameraGuid, FrameNumber );

    if( iAnimationArgs )
    {
        AOdysseyAnimationActor* animation_actor = SpawnAndBindAnimation( iSequencer, iSequence, iSequenceID, CameraGuid, iCamera, FrameNumber, *iAnimationArgs, nullptr );

        actors.Add( animation_actor );
        // This actor can't be selected here because we still are in a cTemporarySwitchInner, so just return it
    }

    return actors;
}

//static
void
ShotSequenceTools::CreateCameraCut( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, FFrameNumber /*iFrameNumber*/ ) // From MovieSceneToolHelpers::CameraAdded()
{
    UMovieScene* movieScene = iSequence->GetMovieScene();

    //---

    // If there's a cinematic shot track, no need to set this camera to a shot
    UMovieSceneTrack* CinematicShotTrack = movieScene->FindTrack( UMovieSceneCinematicBoardTrack::StaticClass() );
    if( CinematicShotTrack )
        return;

    UMovieSceneTrack* CameraCutTrack = movieScene->GetCameraCutTrack();

    // If there's a camera cut track with at least one section, no need to change the section
    if( CameraCutTrack && CameraCutTrack->GetAllSections().Num() > 0 )
    {
        UMovieSceneSingleCameraCutSection* CameraCutSection = Cast<UMovieSceneSingleCameraCutSection>( CameraCutTrack->GetAllSections()[0] );

        CameraCutSection->Modify();
        CameraCutSection->SetCameraGuid( iCameraGuid );

        return;
    }

    if( !CameraCutTrack )
    {
        CameraCutTrack = movieScene->AddCameraCutTrack( UMovieSceneSingleCameraCutTrack::StaticClass() );
    }

    if( CameraCutTrack )
    {
        UMovieSceneSection* Section = MovieSceneHelpers::FindSectionAtTime( CameraCutTrack->GetAllSections(), 0 /*iFrameNumber*/ );
        UMovieSceneSingleCameraCutSection* CameraCutSection = Cast<UMovieSceneSingleCameraCutSection>( Section );

        if( CameraCutSection )
        {
            CameraCutSection->Modify();
            CameraCutSection->SetCameraGuid( iCameraGuid );
        }
        else
        {
            UMovieSceneSingleCameraCutTrack* single_cameracut_track = Cast<UMovieSceneSingleCameraCutTrack>( CameraCutTrack );

            single_cameracut_track->AddNewSingleCameraCut( UE::MovieScene::FRelativeObjectBindingID( iCameraGuid ), 0 /*iFrameNumber*/ ); // Like in UMovieSceneSingleCameraCutSection::SetCameraGuid()

            //CameraCutTrack->Modify();

            //UMovieSceneSingleCameraCutSection* NewSection = Cast<UMovieSceneSingleCameraCutSection>( CameraCutTrack->CreateNewSection() );
            //NewSection->SetRange( movieScene->GetPlaybackRange() );
            //NewSection->SetCameraGuid( iCameraGuid );
            //CameraCutTrack->AddSection( *NewSection );
        }
    }
}

//---
//---
//---

//static
void
BoardSequenceTools::SnapCameraToViewport( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return;

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
    if( !camera_binding.IsValid() )
        return;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    ShotSequenceTools::SnapCameraToViewport( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, camera_binding, inner_frame.GetFrame() );
}

//static
bool
BoardSequenceTools::CanSnapCameraToViewport( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return false;

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
    if( !camera_binding.IsValid() )
        return false;

    return true;
}

//static
void
BoardSequenceTools::SnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
    if( !camera_binding.IsValid() )
        return;

    ShotSequenceTools::SnapCameraToViewport( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, camera_binding, result.mInnerTime.GetFrame() );
}

//static
bool
BoardSequenceTools::CanSnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
    if( !camera_binding.IsValid() )
        return false;

    return true;
}

//static
void
ShotSequenceTools::SnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    FMovieSceneSequenceID sequence_id = iSequencer->GetFocusedTemplateID();
    if( !sequence )
        return;

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, sequence, sequence_id );
    if( !camera_binding.IsValid() )
        return;

    SnapCameraToViewport( *iSequencer, sequence, sequence_id, camera_binding, iFrameNumber );
}

//static
bool
ShotSequenceTools::CanSnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    FMovieSceneSequenceID sequence_id = iSequencer->GetFocusedTemplateID();
    if( !sequence )
        return false;

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, sequence, sequence_id );
    if( !camera_binding.IsValid() )
        return false;

    return true;
}

//static
void
ShotSequenceTools::SnapCameraToViewport( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceId, FGuid iCameraGuid, FFrameNumber iFrameNumber )
{
    if( !GCurrentLevelEditingViewportClient )
        return;

    const FScopedTransaction transaction( LOCTEXT( "transaction.snap-storycamera-to-viewport", "Snap Storyboard Camera To Viewport" ) );

    FTransform transform( GCurrentLevelEditingViewportClient->GetViewTransform().GetRotation(), GCurrentLevelEditingViewportClient->GetViewTransform().GetLocation() );
    bool snapped = SnapCameraToViewport( iSequencer, iSequence, iSequenceId, iCameraGuid, iFrameNumber, transform, iSequencer.GetKeyInterpolation() );
    if( !snapped )
        return;

    ACineCameraActor* camera_actor = ShotSequenceHelpers::GetCameraSpawned( iSequencer, iSequence, iSequenceId, iCameraGuid );
    MovieSceneToolHelpers::LockCameraActorToViewport( iSequencer.AsShared(), camera_actor );

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//static
bool
ShotSequenceTools::SnapCameraToViewport( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceId, FGuid iCameraGuid, FFrameNumber iFrameNumber, const FTransform& iNewTransform, EMovieSceneKeyInterpolation iInterpolation )
{
    UMovieScene* movieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !movieScene || movieScene->IsReadOnly() )
    {
        //ShowReadOnlyError();
        return false;
    }

    ACineCameraActor* camera_actor = ShotSequenceHelpers::GetCameraSpawned( iPlayer, iSequence, iSequenceId, iCameraGuid );
    if( !ensureMsgf(camera_actor, TEXT("In case of a spawnable, it means it is not spawned, the sequence is not the focused one by the sequencer")) )
        return false;

    UMovieSceneTrack* track = movieScene->FindTrack<UMovieScene3DTransformTrack>( iCameraGuid );
    UMovieSceneSection* section = track ? MovieSceneHelpers::FindSectionAtTime( track->GetAllSections(), iFrameNumber ) : nullptr;
    UMovieScene3DTransformSection* transform_section = Cast<UMovieScene3DTransformSection>( section );

    if( !transform_section )
        return false;

    //---

    camera_actor->SetActorTransform( iNewTransform );

//TODO: set all (?) animations ?

    //---

    transform_section->Modify();

    TArrayView<FMovieSceneDoubleChannel*> DoubleChannels = section->GetChannelProxy().GetChannels<FMovieSceneDoubleChannel>();

//TODO: maybe use the same object as StopPilotingCamera() ???

    AddKeyToChannel( DoubleChannels[0], iFrameNumber, camera_actor->GetActorLocation().X, iInterpolation );
    AddKeyToChannel( DoubleChannels[1], iFrameNumber, camera_actor->GetActorLocation().Y, iInterpolation );
    AddKeyToChannel( DoubleChannels[2], iFrameNumber, camera_actor->GetActorLocation().Z, iInterpolation );

    AddKeyToChannel( DoubleChannels[3], iFrameNumber, camera_actor->GetActorRotation().Euler().X, iInterpolation );
    AddKeyToChannel( DoubleChannels[4], iFrameNumber, camera_actor->GetActorRotation().Euler().Y, iInterpolation );
    AddKeyToChannel( DoubleChannels[5], iFrameNumber, camera_actor->GetActorRotation().Euler().Z, iInterpolation );

    //AddKeyToChannel( DoubleChannels[6], iFrameNumber, Scale.X, iInterpolation );
    //AddKeyToChannel( DoubleChannels[7], iFrameNumber, Scale.Y, iInterpolation );
    //AddKeyToChannel( DoubleChannels[8], iFrameNumber, Scale.Z, iInterpolation );

    if( DoubleChannels[0]->GetNumKeys() <= 1 )
        DoubleChannels[0]->SetDefault( camera_actor->GetActorLocation().X );
    if( DoubleChannels[1]->GetNumKeys() <= 1 )
        DoubleChannels[1]->SetDefault( camera_actor->GetActorLocation().Y );
    if( DoubleChannels[2]->GetNumKeys() <= 1 )
        DoubleChannels[2]->SetDefault( camera_actor->GetActorLocation().Z );

    if( DoubleChannels[3]->GetNumKeys() <= 1 )
        DoubleChannels[3]->SetDefault( camera_actor->GetActorRotation().Euler().X );
    if( DoubleChannels[4]->GetNumKeys() <= 1 )
        DoubleChannels[4]->SetDefault( camera_actor->GetActorRotation().Euler().Y );
    if( DoubleChannels[5]->GetNumKeys() <= 1 )
        DoubleChannels[5]->SetDefault( camera_actor->GetActorRotation().Euler().Z );

    //DoubleChannels[6]->SetDefault( Scale.X );
    //DoubleChannels[7]->SetDefault( Scale.Y );
    //DoubleChannels[8]->SetDefault( Scale.Z );

//TODO: set all (?) key animations ?

    return true;
}

//---
//---
//---

//static
void
BoardSequenceTools::DeleteCameraKey( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    ShotSequenceTools::DeleteCameraKey( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iSections, iChannelHandles, iKeyHandles );
}

//static
void
ShotSequenceTools::DeleteCameraKey( ISequencer* iSequencer, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles )
{
    ShotSequenceTools::DeleteCameraKey( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iSections, iChannelHandles, iKeyHandles );
}

//static
void
ShotSequenceTools::DeleteCameraKey( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles )
{
    check( iSections.Num() == iChannelHandles.Num() && iChannelHandles.Num() == iKeyHandles.Num() );

    if( !iKeyHandles.Num() )
        return;

    const FScopedTransaction transaction( LOCTEXT( "transaction.delete-storycamera-transform", "Delete camera keys" ) );

    for( int i = 0; i < iKeyHandles.Num(); i++ )
    {
        TMovieSceneChannelHandle<FMovieSceneDoubleChannel> channel_handle = iChannelHandles[i].Cast<FMovieSceneDoubleChannel>();
        FMovieSceneDoubleChannel* double_channel = channel_handle.Get();
        if( !double_channel )
            return;

        //---

        iSections[i]->Modify();

        double_channel->DeleteKeys( iKeyHandles.Slice( i, 1 ) );
    }

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
    // iChannelHandles are invalid at this point
}

//---
//---
//---

//static
bool
BoardSequenceTools::IsPilotingCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    return ShotSequenceTools::IsPilotingCamera( iSequencer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
bool
BoardSequenceTools::IsPilotingCamera( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    //FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    return ShotSequenceTools::IsPilotingCamera( iSequencer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
bool
ShotSequenceTools::IsPilotingCamera( ISequencer* iSequencer )
{
    return ShotSequenceTools::IsPilotingCamera( iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID() );
}

//static
bool
ShotSequenceTools::IsPilotingCamera( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    check( iSequence->IsA<UShotSequence>() );

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, iSequence, iSequenceID );
    ACineCameraActor* camera = ShotSequenceHelpers::GetCameraSpawned( *iSequencer, iSequence, iSequenceID, camera_binding );
    if( !camera )
        return false;

    if( !GCurrentLevelEditingViewportClient )
        return false;

    return GCurrentLevelEditingViewportClient->IsActorLocked( camera );
}

//-

//static
void
BoardSequenceTools::PilotCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    ShotSequenceTools::PilotCamera( iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
void
BoardSequenceTools::PilotCamera( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    ShotSequenceTools::PilotCamera( iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame() );
}

//static
void
ShotSequenceTools::PilotCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    ShotSequenceTools::PilotCamera( iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
void
ShotSequenceTools::PilotCamera( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    check( iSequence->IsA<UShotSequence>() );

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, iSequence, iSequenceID );
    ACineCameraActor* camera = ShotSequenceHelpers::GetCameraSpawned( *iSequencer, iSequence, iSequenceID, camera_binding );
    if( !ensureMsgf(camera, TEXT("In case of a spawnable, it means it is not spawned, the sequence is not the focused one by the sequencer")) )
        return;

    if( GCurrentLevelEditingViewportClient && GCurrentLevelEditingViewportClient->GetViewMode() != VMI_Unknown && GCurrentLevelEditingViewportClient->AllowsCinematicControl() )
    {
        MovieSceneToolHelpers::LockCameraActorToViewport( iSequencer->AsShared(), camera );
    }
}

//-

//static
bool
BoardSequenceTools::CanPilotCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    return ShotSequenceTools::CanPilotCamera( iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
bool
BoardSequenceTools::CanPilotCamera( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return false;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    return ShotSequenceTools::CanPilotCamera( iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame() );
}

//static
bool
ShotSequenceTools::CanPilotCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    return ShotSequenceTools::CanPilotCamera( iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
bool
ShotSequenceTools::CanPilotCamera( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    check( iSequence->IsA<UShotSequence>() );

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, iSequence, iSequenceID );
    ACineCameraActor* camera = ShotSequenceHelpers::GetCameraSpawned( *iSequencer, iSequence, iSequenceID, camera_binding );
    if( !camera )
        return false;

    return true;
}

//-

//static
void
BoardSequenceTools::EjectCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    ShotSequenceTools::EjectCamera( iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
void
BoardSequenceTools::EjectCamera( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    ShotSequenceTools::EjectCamera( iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame() );
}

//static
void
ShotSequenceTools::EjectCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    ShotSequenceTools::EjectCamera( iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
void
ShotSequenceTools::EjectCamera( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    check( iSequence->IsA<UShotSequence>() );

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, iSequence, iSequenceID );
    ACineCameraActor* camera = ShotSequenceHelpers::GetCameraSpawned( *iSequencer, iSequence, iSequenceID, camera_binding );
    if( !ensureMsgf(camera, TEXT("In case of a spawnable, it means it is not spawned, the sequence is not the focused one by the sequencer")) )
        return;

    if( GCurrentLevelEditingViewportClient && GCurrentLevelEditingViewportClient->GetViewMode() != VMI_Unknown && GCurrentLevelEditingViewportClient->AllowsCinematicControl() )
    {
        GCurrentLevelEditingViewportClient->SetCinematicActorLock( nullptr );
        GCurrentLevelEditingViewportClient->SetActorLock( nullptr );
        GCurrentLevelEditingViewportClient->bLockedCameraView = false;
        GCurrentLevelEditingViewportClient->ViewFOV = GCurrentLevelEditingViewportClient->FOVAngle;
        GCurrentLevelEditingViewportClient->RemoveCameraRoll();
        GCurrentLevelEditingViewportClient->UpdateViewForLockedActor();
        GCurrentLevelEditingViewportClient->Invalidate();
    }
}

//-

//static
bool
BoardSequenceTools::CanEjectCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    return ShotSequenceTools::CanEjectCamera( iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
bool
BoardSequenceTools::CanEjectCamera( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return false;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    return ShotSequenceTools::CanEjectCamera( iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame() );
}

//static
bool
ShotSequenceTools::CanEjectCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    return ShotSequenceTools::CanEjectCamera( iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
bool
ShotSequenceTools::CanEjectCamera( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    check( iSequence->IsA<UShotSequence>() );

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, iSequence, iSequenceID );
    ACineCameraActor* camera = ShotSequenceHelpers::GetCameraSpawned( *iSequencer, iSequence, iSequenceID, camera_binding );
    if( !camera )
        return false;

    return true;
}

//static
void
BoardSequenceTools::StopPilotingCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::StopPilotingCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iCamera, iPreviousTransform, iNewTransform );
}

//static
void
ShotSequenceTools::StopPilotingCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform )
{
    // It's not necessary as it is naturally done by the sequencer options
    //StopPilotingCamera( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iCamera, iPreviousTransform, iNewTransform );
}

//static
void
ShotSequenceTools::StopPilotingCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform )
{
    //TODO: this function is not necessary when we are inside a shot sequence, the stop piloting already create an new key
    // but not for board sequence, see how to call it only for shot section in board track ? and see where it goes when we are in a shot sequence to see if we can connect to it ?

    UMovieScene* movieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !movieScene || movieScene->IsReadOnly() )
    {
        //ShowReadOnlyError();
        return;
    }

    FGuid Binding = iSequencer.FindObjectId( *iCamera, iSequenceID );
    if( !Binding.IsValid() )
        return;

    UMovieScene3DTransformTrack* transform_track = movieScene->FindTrack<UMovieScene3DTransformTrack>( Binding );
    if( !transform_track )
        return;

    UMovieSceneSection* section = MovieSceneHelpers::FindSectionAtTime( transform_track->GetAllSections(), iFrameNumber );
    if( !section )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "transaction.stop-piloting-storycamera", "Stop Piloting Storyboard Camera" ) );

    //---

    FGeneratedTrackKeys generated_keys;
    GetTransformKeys( iSequencer, iPreviousTransform, iNewTransform, EMovieSceneTransformChannel::All, iCamera, section, generated_keys );

    //---

//TODO: set all (?) animations ?

    //---

    bool key_created = AddKeysToSection( iSequencer, section, iFrameNumber, generated_keys, ESequencerKeyMode::AutoKey, EKeyFrameTrackEditorSetDefault::SetDefault );

//TODO: set all (?) key animations ?

    //---

    if( key_created )
        iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//---
//---
//---

//static
void
BoardSequenceTools::GotoPreviousCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    ShotSequenceTools::GotoPreviousCameraPosition( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
bool
BoardSequenceTools::HasPreviousCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    TArray<FFrameNumber> times = ShotSequenceHelpers::GetCameraTransformTimes( result.mInnerSequence );
    int32 index = times.FindLastByPredicate( [result]( FFrameNumber iCurrentFrame ) { return iCurrentFrame < result.mInnerTime.GetFrame(); } );

    return index != INDEX_NONE;
}

//static
void
ShotSequenceTools::GotoPreviousCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    ShotSequenceTools::GotoPreviousCameraPosition( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
bool
ShotSequenceTools::HasPreviousCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UShotSequence>() );

    TArray<FFrameNumber> times = ShotSequenceHelpers::GetCameraTransformTimes( iSequencer->GetFocusedMovieSceneSequence() );
    int32 index = times.FindLastByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame < iFrameNumber; } );

    return index != INDEX_NONE;
}

//static
void
ShotSequenceTools::GotoPreviousCameraPosition( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    check( iSequence->IsA<UShotSequence>() );

    TArray<FFrameNumber> times = ShotSequenceHelpers::GetCameraTransformTimes( iSequence );

    int32 index = times.FindLastByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame < iFrameNumber; } );
    if( index == INDEX_NONE )
        return;

    FFrameNumber previous_time = times[index];

    const FMovieSceneSequenceHierarchy* hierarchy = iSequencer.GetSharedPlaybackState()->GetHierarchy();
    const FMovieSceneSubSequenceData* subdata = hierarchy->FindSubData( iSequenceID );

    FMovieSceneInverseSequenceTransform localToRootTransform = subdata->RootToSequenceTransform.Inverse();
    TOptional<FFrameTime> previous_time_in_root = localToRootTransform.TryTransformTime( previous_time );
    if( !previous_time_in_root )
        return;

    iSequencer.SetGlobalTime( *previous_time_in_root );
}

//-

//static
void
BoardSequenceTools::GotoNextCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    ShotSequenceTools::GotoNextCameraPosition( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
bool
BoardSequenceTools::HasNextCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    TArray<FFrameNumber> times = ShotSequenceHelpers::GetCameraTransformTimes( result.mInnerSequence );
    FFrameNumber* next_time = times.FindByPredicate( [result]( FFrameNumber iCurrentFrame ) { return iCurrentFrame > result.mInnerTime.GetFrame(); } );

    return !!next_time;
}

//static
void
ShotSequenceTools::GotoNextCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    ShotSequenceTools::GotoNextCameraPosition( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
bool
ShotSequenceTools::HasNextCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UShotSequence>() );

    TArray<FFrameNumber> times = ShotSequenceHelpers::GetCameraTransformTimes( iSequencer->GetFocusedMovieSceneSequence() );
    FFrameNumber* next_time = times.FindByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame > iFrameNumber; } );

    return !!next_time;
}

//static
void
ShotSequenceTools::GotoNextCameraPosition( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    check( iSequence->IsA<UShotSequence>() );

    TArray<FFrameNumber> times = ShotSequenceHelpers::GetCameraTransformTimes( iSequence );

    FFrameNumber* next_time = times.FindByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame > iFrameNumber; } );
    if( !next_time )
        return;

    const FMovieSceneSequenceHierarchy* hierarchy = iSequencer.GetSharedPlaybackState()->GetHierarchy();
    const FMovieSceneSubSequenceData* subdata = hierarchy->FindSubData( iSequenceID );

    FMovieSceneInverseSequenceTransform localToRootTransform = subdata->RootToSequenceTransform.Inverse();
    TOptional<FFrameTime> next_time_in_root = localToRootTransform.TryTransformTime( *next_time );
    if( !next_time_in_root )
        return;

    iSequencer.SetGlobalTime( *next_time_in_root );
}

//---

//static
bool
ShotSequenceTools::SetCameraFocalLengthAndScaleActor( TArray<TWeakObjectPtr<AActor>> ioActors, ACineCameraActor* ioCamera, float iNewFocalLength, EScaleActor iScaleType, TSharedPtr<ISequencer> iSequencer )
{
    //const FScopedTransaction transaction( LOCTEXT( "transaction.set-camera-focal-length-and-scale-actor", "Set Camera Focal Length and Scale Actor" ) );

    //---

    struct FParameterCache
    {
        TWeakObjectPtr<AActor> mActor;
        float mCurrentDistance;
        FVector mActorScale;
        FVector mCameraViewSize;
    };

    TArray<FParameterCache> old_parameters;
    for( auto actor : ioActors )
    {
        if( !actor.IsValid() || !ShotSequenceTools::CanMoveAndScaleActor( actor.Get(), ioCamera ) )
            continue;

        FParameterCache parameter;
        parameter.mActor = actor;
        parameter.mCurrentDistance = FVector::Distance( ioCamera->GetActorLocation(), actor->GetActorLocation() );
        parameter.mActorScale = actor->GetActorScale3D();
        parameter.mCameraViewSize = ActorHelpers::ComputeSizeOfCameraView( ioCamera, parameter.mCurrentDistance );

        old_parameters.Add( parameter );
    }

    ioCamera->GetCineCameraComponent()->SetCurrentFocalLength( iNewFocalLength );

    for( const FParameterCache& old_parameter : old_parameters )
    {
        check( old_parameter.mActor.IsValid() );

        FVector new_camera_view_size = ActorHelpers::ComputeSizeOfCameraView( ioCamera, old_parameter.mCurrentDistance );

        bool update_channels = false;

        switch( iScaleType )
        {
            // Until EScaleActor::kFitToCamera will be removed
            PRAGMA_DISABLE_DEPRECATION_WARNINGS
            case EScaleActor::kFitToCamera:
            PRAGMA_ENABLE_DEPRECATION_WARNINGS
            case EScaleActor::kRelativeScale:
            {
                FVector ratio = new_camera_view_size / old_parameter.mCameraViewSize;

                FVector new_actor_scale = old_parameter.mActorScale * ratio;

                old_parameter.mActor->SetActorScale3D( new_actor_scale );

                update_channels = true;
            }
            break;

            case EScaleActor::kNo:
                // nothing to do
                break;

            default: checkNoEntry();
        }

        if( iSequencer )
        {
            if( update_channels )
                UpdateChannel( iSequencer, old_parameter.mActor.Get(), ioCamera, EMovieSceneTransformChannel::Scale );
        }
    }

    return true;
}

#undef LOCTEXT_NAMESPACE
