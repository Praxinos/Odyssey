// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/EposSequenceTools.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "ISequencer.h"
#include "KeyframeTrackEditor.h"
#include "LevelEditorViewport.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"
#include "Tracks/MovieScene3DTransformTrack.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "PlaneActor.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools_Camera"

//static
ACineCameraActor*
BoardSequenceTools::GetCamera( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid* oCameraBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return nullptr;

    return ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, oCameraBinding );
}

//static
ACineCameraActor*
BoardSequenceTools::GetCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid* oCameraBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return nullptr;

    return ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, oCameraBinding );
}

//---

//static
ACineCameraActor*
ShotSequenceTools::GetCamera( ISequencer* iSequencer, FGuid* oCameraBinding )
{
    return ShotSequenceHelpers::GetCamera( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), oCameraBinding );
}

//---
//---
//---

//static
void
BoardSequenceTools::CreateCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, const FCameraArgs& iCameraArgs, const FPlaneArgs& iPlaneArgs )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    return ShotSequenceTools::CreateCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iCameraArgs, iPlaneArgs );
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

    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
    if( camera )
        return false;

    return true;
}

//static
void
ShotSequenceTools::CreateCamera( ISequencer* iSequencer, const FCameraArgs& iCameraArgs, const FPlaneArgs& iPlaneArgs )
{
    CreateCamera( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iCameraArgs, iPlaneArgs );
}

//static
bool
ShotSequenceTools::CanCreateCamera( ISequencer* iSequencer )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    FMovieSceneSequenceID sequence_id = iSequencer->GetFocusedTemplateID();
    if( !sequence )
        return false;

    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, sequence, sequence_id );
    if( camera )
        return false;

    return true;
}

//static
void
ShotSequenceTools::CreateCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FCameraArgs& iCameraArgs, const FPlaneArgs& iPlaneArgs )
{
    UMovieScene* movieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !movieScene )
        return;

    if( movieScene->IsReadOnly() )
    {
        //ShowReadOnlyError();
        return;
    }

    ACineCameraActor* ExistingCamera = ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID );
    if( ExistingCamera )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "transaction.create-storycamera-here", "Create Storyboard Camera Here" ) );

    cTemporarySwitchInner switch_to( iSequencer, iSequenceID );

    //---

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceTools::SpawnAndBindCamera( iSequencer, iSequence, iCameraArgs, iPlaneArgs, &camera_guid );
    if( !camera )
        return;

    ShotSequenceTools::CameraAdded( iSequencer, iSequence, camera_guid, camera, iSequencer.GetLocalTime().Time.FloorToFrame(), iPlaneArgs );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}


//static
ACineCameraActor*
ShotSequenceTools::SpawnCamera( UWorld* iWorld, const FTransform& iTransform )
{
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
    camera->GetCineCameraComponent()->CurrentAperture = settings->CameraSettings.CurrentAperture;
    camera->GetCineCameraComponent()->SetCurrentFocalLength( settings->CameraSettings.CurrentFocalLength ); // Use setter to trigger RecalcDerivedData(), so no need to call Pre/PostChange() and its huge syntax

    return camera;
}

//static
ACineCameraActor*
ShotSequenceTools::SpawnAndBindCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, const FCameraArgs& iCameraArgs, const FPlaneArgs& iPlaneArgs, FGuid* oGuid ) // From FSequencer::CreateCamera()
{
    if( !GCurrentLevelEditingViewportClient )
        return nullptr;

    UWorld* world = GCurrentLevelEditingViewportClient->GetWorld();
    FTransform transform( GCurrentLevelEditingViewportClient->GetViewTransform().GetRotation(), GCurrentLevelEditingViewportClient->GetViewTransform().GetLocation() );

    ACineCameraActor* camera = SpawnCamera( world, transform );

    //---

    FString camera_path;
    FString camera_name;
    NamingConvention::GenerateCameraActorPathName( iSequencer, iSequencer.GetRootMovieSceneSequence(), iSequence, camera_path, camera_name );

    if( !iCameraArgs.mName.IsEmpty() )
        camera_name = iCameraArgs.mName;

    camera->SetFolderPath( *camera_path );
    FActorLabelUtilities::RenameExistingActor( camera, camera_name, false ); // The shot name is displayed in another column in the world outliner

    camera_name = NamingConvention::GenerateCameraTrackName( iSequencer, iSequencer.GetRootMovieSceneSequence(), iSequence, camera );

    FGuid CameraGuid = iSequencer.CreateBinding( *camera, camera_name );
    if( !CameraGuid.IsValid() )
        return nullptr;

    iSequencer.OnActorAddedToSequencer().Broadcast( camera, CameraGuid );

    //---

    MovieSceneToolHelpers::LockCameraActorToViewport( iSequencer.AsShared(), camera );

    *oGuid = CameraGuid;
    return camera;
}

//static
void
ShotSequenceTools::CameraAdded( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid CameraGuid, ACineCameraActor* iCamera, FFrameNumber FrameNumber, const FPlaneArgs& iPlaneArgs )
{
    CreateCameraCut( iSequencer, iSequence, CameraGuid, FrameNumber );

    SpawnAndBindPlane( iSequencer, iSequence, CameraGuid, iCamera, FrameNumber, iPlaneArgs );
}

//static
void
ShotSequenceTools::CreateCameraCut( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, FFrameNumber /*iFrameNumber*/ ) // From MovieSceneToolHelpers::CameraAdded()
{
    UMovieScene* movieScene = iSequence->GetMovieScene();

    //---

    // If there's a cinematic shot track, no need to set this camera to a shot
    UMovieSceneTrack* CinematicShotTrack = movieScene->FindMasterTrack( UMovieSceneCinematicBoardTrack::StaticClass() );
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

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, &camera_guid );
    if( !camera )
        return;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    ShotSequenceTools::SnapCameraToViewport( *iSequencer, result.mInnerSequence, camera, camera_guid, inner_frame.GetFrame() );
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

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, &camera_guid );
    if( !camera )
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

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, &camera_guid );
    if( !camera )
        return;

    ShotSequenceTools::SnapCameraToViewport( *iSequencer, result.mInnerSequence, camera, camera_guid, result.mInnerTime.GetFrame() );
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

    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
    if( !camera )
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

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, sequence, sequence_id, &camera_guid );
    if( !camera )
        return;

    SnapCameraToViewport( *iSequencer, sequence, camera, camera_guid, iFrameNumber );
}

//static
bool
ShotSequenceTools::CanSnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    FMovieSceneSequenceID sequence_id = iSequencer->GetFocusedTemplateID();
    if( !sequence )
        return false;

    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, sequence, sequence_id );
    if( !camera )
        return false;

    return true;
}

//static
void
ShotSequenceTools::SnapCameraToViewport( ISequencer& iSequencer, UMovieSceneSequence* iSequence, ACineCameraActor* ioCamera, FGuid iCameraGuid, FFrameNumber iFrameNumber )
{
    if( !GCurrentLevelEditingViewportClient )
        return;

    const FScopedTransaction transaction( LOCTEXT( "transaction.snap-storycamera-to-viewport", "Snap Storyboard Camera To Viewport" ) );

    FTransform transform( GCurrentLevelEditingViewportClient->GetViewTransform().GetRotation(), GCurrentLevelEditingViewportClient->GetViewTransform().GetLocation() );
    bool snapped = SnapCameraToViewport( iSequencer, iSequence, ioCamera, iCameraGuid, iFrameNumber, transform, iSequencer.GetKeyInterpolation() );
    if( !snapped )
        return;

    MovieSceneToolHelpers::LockCameraActorToViewport( iSequencer.AsShared(), ioCamera );

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//static
bool
ShotSequenceTools::SnapCameraToViewport( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, ACineCameraActor* ioCamera, FGuid iCameraGuid, FFrameNumber iFrameNumber, const FTransform& iNewTransform, EMovieSceneKeyInterpolation iInterpolation )
{
    UMovieScene* movieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !movieScene || movieScene->IsReadOnly() )
    {
        //ShowReadOnlyError();
        return false;
    }

    UMovieSceneTrack* track = movieScene->FindTrack<UMovieScene3DTransformTrack>( iCameraGuid );
    UMovieSceneSection* section = track ? MovieSceneHelpers::FindSectionAtTime( track->GetAllSections(), iFrameNumber ) : nullptr;
    UMovieScene3DTransformSection* transform_section = Cast<UMovieScene3DTransformSection>( section );

    if( !transform_section )
        return false;

    //---

    ioCamera->SetActorTransform( iNewTransform );

//TODO: set all (?) planes ?

    //---

    transform_section->Modify();

    TArrayView<FMovieSceneDoubleChannel*> DoubleChannels = section->GetChannelProxy().GetChannels<FMovieSceneDoubleChannel>();

//TODO: maybe use the same object as StopPilotingCamera() ???

    AddKeyToChannel( DoubleChannels[0], iFrameNumber, ioCamera->GetActorLocation().X, iInterpolation );
    AddKeyToChannel( DoubleChannels[1], iFrameNumber, ioCamera->GetActorLocation().Y, iInterpolation );
    AddKeyToChannel( DoubleChannels[2], iFrameNumber, ioCamera->GetActorLocation().Z, iInterpolation );

    AddKeyToChannel( DoubleChannels[3], iFrameNumber, ioCamera->GetActorRotation().Euler().X, iInterpolation );
    AddKeyToChannel( DoubleChannels[4], iFrameNumber, ioCamera->GetActorRotation().Euler().Y, iInterpolation );
    AddKeyToChannel( DoubleChannels[5], iFrameNumber, ioCamera->GetActorRotation().Euler().Z, iInterpolation );

    //AddKeyToChannel( DoubleChannels[6], iFrameNumber, Scale.X, iInterpolation );
    //AddKeyToChannel( DoubleChannels[7], iFrameNumber, Scale.Y, iInterpolation );
    //AddKeyToChannel( DoubleChannels[8], iFrameNumber, Scale.Z, iInterpolation );

    if( DoubleChannels[0]->GetNumKeys() <= 1 )
        DoubleChannels[0]->SetDefault( ioCamera->GetActorLocation().X );
    if( DoubleChannels[1]->GetNumKeys() <= 1 )
        DoubleChannels[1]->SetDefault( ioCamera->GetActorLocation().Y );
    if( DoubleChannels[2]->GetNumKeys() <= 1 )
        DoubleChannels[2]->SetDefault( ioCamera->GetActorLocation().Z );

    if( DoubleChannels[3]->GetNumKeys() <= 1 )
        DoubleChannels[3]->SetDefault( ioCamera->GetActorRotation().Euler().X );
    if( DoubleChannels[4]->GetNumKeys() <= 1 )
        DoubleChannels[4]->SetDefault( ioCamera->GetActorRotation().Euler().Y );
    if( DoubleChannels[5]->GetNumKeys() <= 1 )
        DoubleChannels[5]->SetDefault( ioCamera->GetActorRotation().Euler().Z );

    //DoubleChannels[6]->SetDefault( Scale.X );
    //DoubleChannels[7]->SetDefault( Scale.Y );
    //DoubleChannels[8]->SetDefault( Scale.Z );

//TODO: set all (?) key planes ?

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

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, iSequence, iSequenceID, &camera_guid );
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

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, iSequence, iSequenceID, &camera_guid );
    if( !camera )
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

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, iSequence, iSequenceID, &camera_guid );
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

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, iSequence, iSequenceID, &camera_guid );
    if( !camera )
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

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, iSequence, iSequenceID, &camera_guid );
    if( !camera )
        return false;

    return true;
}

//---
//---
//---

static
float
UnwindChannel( const float& OldValue, float NewValue )
{
    while( NewValue - OldValue > 180.0f )
    {
        NewValue -= 360.0f;
    }
    while( NewValue - OldValue < -180.0f )
    {
        NewValue += 360.0f;
    }
    return NewValue;
}
static
FRotator
UnwindRotator( const FRotator& InOld, const FRotator& InNew )
{
    FRotator Result;
    Result.Pitch = UnwindChannel( InOld.Pitch, InNew.Pitch );
    Result.Yaw = UnwindChannel( InOld.Yaw, InNew.Yaw );
    Result.Roll = UnwindChannel( InOld.Roll, InNew.Roll );
    return Result;
}

// From ...\UE_4.26\Engine\Source\Editor\MovieSceneTools\Private\TrackEditors\TransformTrackEditor.cpp
static
void
GetTransformKeys( ISequencer& iSequencer, const TOptional<FTransformData>& LastTransform, const FTransformData& CurrentTransform, EMovieSceneTransformChannel ChannelsToKey, UObject* Object, UMovieSceneSection* Section, FGeneratedTrackKeys& OutGeneratedKeys )
{
    UMovieScene3DTransformSection* TransformSection = Cast<UMovieScene3DTransformSection>( Section );
    EMovieSceneTransformChannel TransformMask = TransformSection->GetMask().GetChannels();

    using namespace UE::MovieScene;

    bool bLastVectorIsValid = LastTransform.IsSet();

    // If key all is enabled, for a key on all the channels
    if( iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyAll )
    {
        bLastVectorIsValid = false;
        ChannelsToKey = EMovieSceneTransformChannel::All;
    }

    //FBuiltInComponentTypes* BuiltInComponents = FBuiltInComponentTypes::Get();

    //FTransformData RecomposedTransform = RecomposeTransform( CurrentTransform, Object, Section );

    // Set translation keys/defaults
    {
        bool bKeyX = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::TranslationX );
        bool bKeyY = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::TranslationY );
        bool bKeyZ = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::TranslationZ );

        if( bLastVectorIsValid )
        {
            bKeyX &= !FMath::IsNearlyEqual( LastTransform->Translation.X, CurrentTransform.Translation.X );
            bKeyY &= !FMath::IsNearlyEqual( LastTransform->Translation.Y, CurrentTransform.Translation.Y );
            bKeyZ &= !FMath::IsNearlyEqual( LastTransform->Translation.Z, CurrentTransform.Translation.Z );
        }

        if( iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyGroup && ( bKeyX || bKeyY || bKeyZ ) )
        {
            bKeyX = bKeyY = bKeyZ = true;
        }

        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::TranslationX ) )
        {
            bKeyX = false;
        }
        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::TranslationY ) )
        {
            bKeyY = false;
        }
        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::TranslationZ ) )
        {
            bKeyZ = false;
        }

        FVector KeyVector = CurrentTransform.Translation;
        //FVector KeyVector = RecomposedTransform.Translation;

        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 0, KeyVector.X, bKeyX ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 1, KeyVector.Y, bKeyY ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 2, KeyVector.Z, bKeyZ ) );
    }

    // Set rotation keys/defaults
    {
        bool bKeyX = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::RotationX );
        bool bKeyY = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::RotationY );
        bool bKeyZ = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::RotationZ );

        FRotator KeyRotator = CurrentTransform.Rotation;
        if( bLastVectorIsValid )
        {
            KeyRotator = UnwindRotator( LastTransform->Rotation, CurrentTransform.Rotation );

            bKeyX &= !FMath::IsNearlyEqual( LastTransform->Rotation.Roll, KeyRotator.Roll );
            bKeyY &= !FMath::IsNearlyEqual( LastTransform->Rotation.Pitch, KeyRotator.Pitch );
            bKeyZ &= !FMath::IsNearlyEqual( LastTransform->Rotation.Yaw, KeyRotator.Yaw );
        }

        if( iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyGroup && ( bKeyX || bKeyY || bKeyZ ) )
        {
            bKeyX = bKeyY = bKeyZ = true;
        }

        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::RotationX ) )
        {
            bKeyX = false;
        }
        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::RotationY ) )
        {
            bKeyY = false;
        }
        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::RotationZ ) )
        {
            bKeyZ = false;
        }

        // Do we need to unwind re-composed rotations?
        //KeyRotator = UnwindRotator( CurrentTransform.Rotation, RecomposedTransform.Rotation );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 3, KeyRotator.Roll, bKeyX ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 4, KeyRotator.Pitch, bKeyY ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 5, KeyRotator.Yaw, bKeyZ ) );

    }

    // Set scale keys/defaults
    {
        bool bKeyX = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::ScaleX );
        bool bKeyY = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::ScaleY );
        bool bKeyZ = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::ScaleZ );

        if( bLastVectorIsValid )
        {
            bKeyX &= !FMath::IsNearlyEqual( LastTransform->Scale.X, CurrentTransform.Scale.X );
            bKeyY &= !FMath::IsNearlyEqual( LastTransform->Scale.Y, CurrentTransform.Scale.Y );
            bKeyZ &= !FMath::IsNearlyEqual( LastTransform->Scale.Z, CurrentTransform.Scale.Z );
        }

        if( iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyGroup && ( bKeyX || bKeyY || bKeyZ ) )
        {
            bKeyX = bKeyY = bKeyZ = true;
        }

        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::ScaleX ) )
        {
            bKeyX = false;
        }
        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::ScaleY ) )
        {
            bKeyY = false;
        }
        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::ScaleZ ) )
        {
            bKeyZ = false;
        }

        FVector KeyVector = CurrentTransform.Scale;
        //FVector KeyVector = RecomposedTransform.Scale;
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 6, KeyVector.X, bKeyX ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 7, KeyVector.Y, bKeyY ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 8, KeyVector.Z, bKeyZ ) );
    }
}

// From ...\UE_4.26\Engine\Source\Editor\MovieSceneTools\Public\KeyframeTrackEditor.h
static
void
AddKeysToSection( ISequencer& iSequencer, UMovieSceneSection* Section, FFrameNumber KeyTime, const FGeneratedTrackKeys& Keys, ESequencerKeyMode KeyMode )
{
    EAutoChangeMode AutoChangeMode = iSequencer.GetAutoChangeMode();

    FMovieSceneChannelProxy& Proxy = Section->GetChannelProxy();

    const bool bSetDefaults = iSequencer.GetAutoSetTrackDefaults();

    if( KeyMode != ESequencerKeyMode::AutoKey || AutoChangeMode == EAutoChangeMode::AutoKey || AutoChangeMode == EAutoChangeMode::All )
    {
        EMovieSceneKeyInterpolation InterpolationMode = iSequencer.GetKeyInterpolation();

        const bool bKeyEvenIfUnchanged =
            KeyMode == ESequencerKeyMode::ManualKeyForced ||
            iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyAll ||
            iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyGroup;

        const bool bKeyEvenIfEmpty =
            ( KeyMode == ESequencerKeyMode::AutoKey && AutoChangeMode == EAutoChangeMode::All ) ||
            KeyMode == ESequencerKeyMode::ManualKeyForced;

        for( const FMovieSceneChannelValueSetter& GeneratedKey : Keys )
        {
            GeneratedKey->Apply( Section, Proxy, KeyTime, InterpolationMode, bKeyEvenIfUnchanged, bKeyEvenIfEmpty );
        }
    }

    if( bSetDefaults )
    {
        for( const FMovieSceneChannelValueSetter& GeneratedKey : Keys )
        {
            GeneratedKey->ApplyDefault( Section, Proxy );
        }
    }
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

//TODO: set all (?) planes ?

    //---

    AddKeysToSection( iSequencer, section, iFrameNumber, generated_keys, ESequencerKeyMode::AutoKey );

//TODO: set all (?) key planes ?

    //---

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

    const FMovieSceneSequenceHierarchy* hierarchy = iSequencer.GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( iSequencer.GetEvaluationTemplate().GetCompiledDataID() );
    const FMovieSceneSubSequenceData* subdata = hierarchy->FindSubData( iSequenceID );

    iSequencer.SetGlobalTime( previous_time * subdata->RootToSequenceTransform.InverseLinearOnly() );
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

    const FMovieSceneSequenceHierarchy* hierarchy = iSequencer.GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( iSequencer.GetEvaluationTemplate().GetCompiledDataID() );
    const FMovieSceneSubSequenceData* subdata = hierarchy->FindSubData( iSequenceID );

    iSequencer.SetGlobalTime( *next_time * subdata->RootToSequenceTransform.InverseLinearOnly() );
}

//---

//static
bool
ShotSequenceTools::SetCameraFocalLengthAndScalePlane( TArray<TWeakObjectPtr<APlaneActor>> ioPlanes, ACineCameraActor* ioCamera, float iNewFocalLength, EScalePlane iScaleType )
{
    TArray<TWeakObjectPtr<APlaneActor>> planes;
    TArray<float> current_distances;
    TArray<FVector> old_scales;
    TArray<FVector> old_scales_camera100;
    for( auto plane : ioPlanes )
    {
        if( !plane.IsValid() || !ShotSequenceTools::CanMoveAndScalePlane( plane.Get(), ioCamera ) )
            continue;

        planes.Add( plane );
        current_distances.Add( FVector::Distance( ioCamera->GetActorLocation(), plane->GetActorLocation() ) );
        old_scales.Add( plane->GetActorScale3D() );
        old_scales_camera100.Add( plane->ComputePlaneScaleWithScaleAndMargin( ioCamera, current_distances.Last() ) );
    }

    ioCamera->GetCineCameraComponent()->SetCurrentFocalLength( iNewFocalLength );

    for( int i = 0; i < planes.Num(); i++ )
    {
        APlaneActor* plane = planes[i].Get();
        check( plane );
        float current_distance = current_distances[i];
        FVector old_scale = old_scales[i];
        FVector old_scale_camera100 = old_scales_camera100[i];

        switch( iScaleType )
        {
            case EScalePlane::kFitToCamera:
            {
                FVector scale = plane->ComputePlaneScaleWithScaleAndMargin( ioCamera, current_distance );
                plane->SetActorScale3D( scale );
            }
            break;

            case EScalePlane::kRelativeScale:
            {
                FVector new_scale_camera100 = plane->ComputePlaneScaleWithScaleAndMargin( ioCamera, current_distance );
                FVector ratio = new_scale_camera100 / old_scale_camera100;
                FVector new_scale = old_scale * ratio;

                plane->SetActorScale3D( new_scale );
            }
            break;

            case EScalePlane::kNo:
                // nothing to do
                break;

            default: checkNoEntry();
        }
    }

    return true;
}

#undef LOCTEXT_NAMESPACE
