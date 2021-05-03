// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Helpers/EposSequenceToolHelpers.h"

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
#include "Board/BoardSequenceHelpers.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Shot/ShotSequenceHelpers.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

#define LOCTEXT_NAMESPACE "EposSequenceToolHelpers_Camera"

//static
bool
BoardSequenceToolHelpers::CanCreateCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    return !ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
ACineCameraActor*
BoardSequenceToolHelpers::GetCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid* oCameraBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return nullptr;

    return ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, oCameraBinding );
}

//---

//static
ACineCameraActor*
ShotSequenceToolHelpers::GetCamera( ISequencer* iSequencer, FGuid* oCameraBinding )
{
    return ShotSequenceHelpers::GetCamera( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), oCameraBinding );
}

//---
//---
//---

//static
void
BoardSequenceToolHelpers::CreateCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    return ShotSequenceToolHelpers::CreateCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
void
ShotSequenceToolHelpers::CreateCamera( ISequencer* iSequencer )
{
    CreateCamera( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID() );
}

//static
void
ShotSequenceToolHelpers::CreateCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    UMovieScene* movieScene = iSequence->GetMovieScene();
    if( !movieScene )
        return;

    if( movieScene->IsReadOnly() )
    {
        //ShowReadOnlyError();
        return;
    }

    FGuid CameraGuid;
    ACineCameraActor* ExistingCamera = ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID, &CameraGuid );
    if( ExistingCamera )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "CreateStoryCameraHere", "Create Storyboard Camera Here" ) );

    cTemporarySwitchInner switch_to( iSequencer, iSequenceID );

    //---

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceToolHelpers::SpawnAndBindCamera( iSequencer, iSequence, &camera_guid );

    ShotSequenceToolHelpers::CameraAdded( iSequencer, iSequence, camera_guid, camera, iSequencer.GetLocalTime().Time.FloorToFrame() );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}


//static
ACineCameraActor*
ShotSequenceToolHelpers::SpawnCamera( UWorld* iWorld, const FTransform& iTransform )
{
    // Set new camera to match viewport
    FActorSpawnParameters SpawnParams;
    ACineCameraActor* camera = iWorld->SpawnActor<ACineCameraActor>( SpawnParams );
    if( !camera )
        return nullptr;

    camera->SetActorTransform( iTransform );
    //camera->CameraComponent->FieldOfView = ViewportClient->ViewFOV; //@todo set the focal length from this field of view

    const UEposSequenceEditorSettings* settings = GetDefault<UEposSequenceEditorSettings>();

    // https://udn.unrealengine.com/s/question/0D54z00006uhl34CAA/plugin-cuproperty-how-to-change-uproperty-and-trigger-prepostedit-
    //UCineCameraComponent* CameraComponent = camera->GetCineCameraComponent();
    //if( CameraComponent != NULL )
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
ShotSequenceToolHelpers::SpawnAndBindCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid* oGuid ) // From FSequencer::CreateCamera()
{
    UWorld* world = GCurrentLevelEditingViewportClient->GetWorld();
    FTransform transform( GCurrentLevelEditingViewportClient->GetViewTransform().GetRotation(), GCurrentLevelEditingViewportClient->GetViewTransform().GetLocation() );

    ACineCameraActor* camera = SpawnCamera( world, transform );

    //---

    camera->SetFolderPath( *FPaths::GetBaseFilename( iSequencer.GetRootMovieSceneSequence()->GetPathName() ) );
    FActorLabelUtilities::SetActorLabelUnique( camera, TEXT( "Camera_1" ) ); // The shot name is displayed in another column in the world outliner

    FGuid CameraGuid = iSequencer.CreateBinding( *camera, camera->GetActorLabel() );
    if( !CameraGuid.IsValid() )
        return nullptr;

    iSequencer.OnActorAddedToSequencer().Broadcast( camera, CameraGuid );

    //---
    // From FSequencer::NewCameraAdded( CameraGuid, camera )

    // an option ?

    iSequencer.SetPerspectiveViewportCameraCutEnabled( false );

    // Lock the viewport to this camera
    if( camera && camera->GetLevel() )
    {
        GCurrentLevelEditingViewportClient->SetMatineeActorLock( nullptr );
        GCurrentLevelEditingViewportClient->SetActorLock( camera );
        GCurrentLevelEditingViewportClient->bLockedCameraView = true;
        GCurrentLevelEditingViewportClient->UpdateViewForLockedActor();
        GCurrentLevelEditingViewportClient->Invalidate();
    }

    *oGuid = CameraGuid;
    return camera;
}

//static
void
ShotSequenceToolHelpers::CameraAdded( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid CameraGuid, ACineCameraActor* iCamera, FFrameNumber FrameNumber)
{
    CreateCameraCut( iSequencer, iSequence, CameraGuid, FrameNumber );

    SpawnAndBindPlane( iSequencer, iSequence, CameraGuid, iCamera, FrameNumber );
}

//static
void
ShotSequenceToolHelpers::CreateCameraCut( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, FFrameNumber /*iFrameNumber*/ ) // From MovieSceneToolHelpers::CameraAdded()
{
    UMovieScene* movieScene = iSequence->GetMovieScene();

    //---

    // If there's a cinematic shot track, no need to set this camera to a shot
    UMovieSceneTrack* CinematicShotTrack = movieScene->FindMasterTrack( UMovieSceneCinematicShotTrack::StaticClass() );
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
            FMovieSceneObjectBindingID binding_id( iCameraGuid, MovieSceneSequenceID::Root, EMovieSceneObjectBindingSpace::Local ); // Like in UMovieSceneSingleCameraCutSection::SetCameraGuid()

            single_cameracut_track->AddNewSingleCameraCut( binding_id, 0 /*iFrameNumber*/ );

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
BoardSequenceToolHelpers::SnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, &camera_guid );
    if( !camera )
        return;

    ShotSequenceToolHelpers::SnapCameraToViewport( *iSequencer, result.mInnerSequence, camera, camera_guid, result.mInnerTime.GetFrame() );
}

//static
void
ShotSequenceToolHelpers::SnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    FMovieSceneSequenceID sequence_id = iSequencer->GetFocusedTemplateID();
    if( !sequence )
        return;

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, sequence, sequence_id, &camera_guid );
    if( !camera )
        return;

    ShotSequenceToolHelpers::SnapCameraToViewport( *iSequencer, sequence, camera, camera_guid, iFrameNumber );
}

//static
void
ShotSequenceToolHelpers::SnapCameraToViewport( ISequencer& iSequencer, UMovieSceneSequence* iSequence, ACineCameraActor* ioCamera, FGuid iCameraGuid, FFrameNumber iFrameNumber )
{
    const FScopedTransaction transaction( LOCTEXT( "SnapStoryCameraToViewport", "Snap Storyboard Camera To Viewport" ) );

    FTransform transform( GCurrentLevelEditingViewportClient->GetViewTransform().GetRotation(), GCurrentLevelEditingViewportClient->GetViewTransform().GetLocation() );
    bool snapped = SnapCameraToViewport( iSequencer, iSequence, ioCamera, iCameraGuid, iFrameNumber, transform, iSequencer.GetKeyInterpolation() );
    if( !snapped )
        return;

    //---
    // From FSequencer::NewCameraAdded( CameraGuid, NewCamera )

    //iSequencer->SetPerspectiveViewportCameraCutEnabled( false );

    // Lock the viewport to this camera
    if( ioCamera && ioCamera->GetLevel() )
    {
        //GCurrentLevelEditingViewportClient->SetMatineeActorLock( nullptr );
        //GCurrentLevelEditingViewportClient->SetActorLock( ExistingCamera );
        //GCurrentLevelEditingViewportClient->bLockedCameraView = true;
        //GCurrentLevelEditingViewportClient->UpdateViewForLockedActor();
        GCurrentLevelEditingViewportClient->Invalidate();
    }
    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//static
bool
ShotSequenceToolHelpers::SnapCameraToViewport( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, ACineCameraActor* ioCamera, FGuid iCameraGuid, FFrameNumber iFrameNumber, const FTransform& iNewTransform, EMovieSceneKeyInterpolation iInterpolation )
{
    UMovieScene* movieScene = iSequence->GetMovieScene();
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

    TArrayView<FMovieSceneFloatChannel*> FloatChannels = section->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>();

//TODO: maybe use the same object as StopPilotingCamera() ???

    AddKeyToChannel( FloatChannels[0], iFrameNumber, ioCamera->GetActorLocation().X, iInterpolation );
    AddKeyToChannel( FloatChannels[1], iFrameNumber, ioCamera->GetActorLocation().Y, iInterpolation );
    AddKeyToChannel( FloatChannels[2], iFrameNumber, ioCamera->GetActorLocation().Z, iInterpolation );

    AddKeyToChannel( FloatChannels[3], iFrameNumber, ioCamera->GetActorRotation().Euler().X, iInterpolation );
    AddKeyToChannel( FloatChannels[4], iFrameNumber, ioCamera->GetActorRotation().Euler().Y, iInterpolation );
    AddKeyToChannel( FloatChannels[5], iFrameNumber, ioCamera->GetActorRotation().Euler().Z, iInterpolation );

    //AddKeyToChannel( FloatChannels[6], iFrameNumber, Scale.X, iInterpolation );
    //AddKeyToChannel( FloatChannels[7], iFrameNumber, Scale.Y, iInterpolation );
    //AddKeyToChannel( FloatChannels[8], iFrameNumber, Scale.Z, iInterpolation );

    if( FloatChannels[0]->GetNumKeys() <= 1 )
        FloatChannels[0]->SetDefault( ioCamera->GetActorLocation().X );
    if( FloatChannels[1]->GetNumKeys() <= 1 )
        FloatChannels[1]->SetDefault( ioCamera->GetActorLocation().Y );
    if( FloatChannels[2]->GetNumKeys() <= 1 )
        FloatChannels[2]->SetDefault( ioCamera->GetActorLocation().Z );

    if( FloatChannels[3]->GetNumKeys() <= 1 )
        FloatChannels[3]->SetDefault( ioCamera->GetActorRotation().Euler().X );
    if( FloatChannels[4]->GetNumKeys() <= 1 )
        FloatChannels[4]->SetDefault( ioCamera->GetActorRotation().Euler().Y );
    if( FloatChannels[5]->GetNumKeys() <= 1 )
        FloatChannels[5]->SetDefault( ioCamera->GetActorRotation().Euler().Z );

    //FloatChannels[6]->SetDefault( Scale.X );
    //FloatChannels[7]->SetDefault( Scale.Y );
    //FloatChannels[8]->SetDefault( Scale.Z );

//TODO: set all (?) key planes ?

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

        FVector KeyVector = CurrentTransform.Translation;
        //FVector KeyVector = RecomposedTransform.Translation;

        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 0, KeyVector.X, bKeyX ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 1, KeyVector.Y, bKeyY ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 2, KeyVector.Z, bKeyZ ) );
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

        // Do we need to unwind re-composed rotations?
        //KeyRotator = UnwindRotator( CurrentTransform.Rotation, RecomposedTransform.Rotation );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 3, KeyRotator.Roll, bKeyX ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 4, KeyRotator.Pitch, bKeyY ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 5, KeyRotator.Yaw, bKeyZ ) );

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

        FVector KeyVector = CurrentTransform.Scale;
        //FVector KeyVector = RecomposedTransform.Scale;
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 6, KeyVector.X, bKeyX ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 7, KeyVector.Y, bKeyY ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 8, KeyVector.Z, bKeyZ ) );
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
BoardSequenceToolHelpers::StopPilotingCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceToolHelpers::StopPilotingCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iCamera, iPreviousTransform, iNewTransform );
}

//static
void
ShotSequenceToolHelpers::StopPilotingCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform )
{
    StopPilotingCamera( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iCamera, iPreviousTransform, iNewTransform );
}

//static
void
ShotSequenceToolHelpers::StopPilotingCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform )
{
    const FScopedTransaction transaction( LOCTEXT( "StopPilotingCamera", "Stop Piloting Storyboard Camera" ) );

    FGuid Binding = iSequencer.FindObjectId( *iCamera, iSequenceID );
    if( !Binding.IsValid() )
        return;

    UMovieScene3DTransformTrack* transform_track = iSequence->GetMovieScene()->FindTrack<UMovieScene3DTransformTrack>( Binding );

    //---

    UMovieSceneSection* section = MovieSceneHelpers::FindSectionAtTime( transform_track->GetAllSections(), iFrameNumber );
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

#undef LOCTEXT_NAMESPACE
