// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShotSequenceHelpers.h"

#include "Channels/MovieSceneFloatChannel.h"
#include "CineCameraActor.h"
#include "ISequencer.h"
#include "LevelEditorActions.h"
#include "LevelEditorViewport.h"
#include "MovieScene.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Tracks/MovieSceneCinematicShotTrack.h"

#include "Editor/EditorEngine.h"

#define LOCTEXT_NAMESPACE "ShotSequenceHelpers_Camera"

//static
ACineCameraActor*
ShotSequenceHelpers::GetCamera( TSharedPtr<ISequencer> iSequencer, FGuid* oGuid )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    if( !sequence )
        return nullptr;
    UMovieScene* movieScene = sequence->GetMovieScene();
    if( !movieScene )
        return nullptr;

    ACineCameraActor* ExistingCamera = nullptr;
    for( int i = 0; i < movieScene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = movieScene->GetPossessable( i );

        for( auto Object : iSequencer->FindObjectsInCurrentSequence( possessable.GetGuid() ) )
        {
            ExistingCamera = Cast<ACineCameraActor>( Object.Get() );

            if( ExistingCamera )
            {
                if( oGuid )
                    *oGuid = possessable.GetGuid();

                return ExistingCamera;
            }
        }
    }

    return nullptr;
}

//---

//static
void
ShotSequenceHelpers::CreateCameraAndCameraCut( TSharedPtr<ISequencer> iSequencer ) // From FSequencer::CreateCamera()
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    if( !sequence )
        return;
    UMovieScene* movieScene = sequence->GetMovieScene();
    if( !movieScene )
        return;

    if( movieScene->IsReadOnly() )
    {
        //ShowReadOnlyError();
        return;
    }

    UWorld* World = GCurrentLevelEditingViewportClient ? GCurrentLevelEditingViewportClient->GetWorld() : nullptr;
    if( !World )
        return;

    FGuid CameraGuid;
    ACineCameraActor* ExistingCamera = ShotSequenceHelpers::GetCamera( iSequencer, &CameraGuid );

    if( ExistingCamera )
        return;

    //---

    const FScopedTransaction Transaction( LOCTEXT( "CreateStoryCameraHere", "Create Storyboard Camera Here" ) );

    // Set new camera to match viewport
    FActorSpawnParameters SpawnParams;
    ACineCameraActor* NewCamera = World->SpawnActor<ACineCameraActor>( SpawnParams );
    if( !NewCamera )
        return;

    CameraGuid = iSequencer->CreateBinding( *NewCamera, NewCamera->GetActorLabel() );

    if( !CameraGuid.IsValid() )
        return;

    NewCamera->SetActorLocation( GCurrentLevelEditingViewportClient->GetViewLocation(), false );
    NewCamera->SetActorRotation( GCurrentLevelEditingViewportClient->GetViewRotation() );
    //pNewCamera->CameraComponent->FieldOfView = ViewportClient->ViewFOV; //@todo set the focal length from this field of view

    iSequencer->OnActorAddedToSequencer().Broadcast( NewCamera, CameraGuid );

    //---
    // From FSequencer::NewCameraAdded( CameraGuid, NewCamera )

    //iSequencer->SetPerspectiveViewportCameraCutEnabled( false );

    // Lock the viewport to this camera
    if( NewCamera && NewCamera->GetLevel() )
    {
        // an option ?

        //GCurrentLevelEditingViewportClient->SetMatineeActorLock( nullptr );
        //GCurrentLevelEditingViewportClient->SetActorLock( NewCamera );
        //GCurrentLevelEditingViewportClient->bLockedCameraView = true;
        //GCurrentLevelEditingViewportClient->UpdateViewForLockedActor();
        GCurrentLevelEditingViewportClient->Invalidate();
    }

    ShotSequenceHelpers::CameraAdded( movieScene, CameraGuid, iSequencer->GetLocalTime().Time.FloorToFrame() );
    //---

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}

//static
void
ShotSequenceHelpers::CameraAdded(UMovieScene* OwnerMovieScene, FGuid CameraGuid, FFrameNumber FrameNumber) // From MovieSceneToolHelpers::CameraAdded()
{
	// If there's a cinematic shot track, no need to set this camera to a shot
	UMovieSceneTrack* CinematicShotTrack = OwnerMovieScene->FindMasterTrack(UMovieSceneCinematicShotTrack::StaticClass());
	if (CinematicShotTrack)
		return;

	UMovieSceneTrack* CameraCutTrack = OwnerMovieScene->GetCameraCutTrack();

	// If there's a camera cut track with at least one section, no need to change the section
	if (CameraCutTrack && CameraCutTrack->GetAllSections().Num() > 0)
	{
        UMovieSceneSingleCameraCutSection* CameraCutSection = Cast<UMovieSceneSingleCameraCutSection>( CameraCutTrack->GetAllSections()[0] );

        CameraCutSection->Modify();
        CameraCutSection->SetCameraGuid( CameraGuid );

		return;
	}

	if (!CameraCutTrack)
	{
		CameraCutTrack = OwnerMovieScene->AddCameraCutTrack(UMovieSceneSingleCameraCutTrack::StaticClass());
	}

	if (CameraCutTrack)
	{
		UMovieSceneSection* Section = MovieSceneHelpers::FindSectionAtTime(CameraCutTrack->GetAllSections(), FrameNumber);
		UMovieSceneSingleCameraCutSection* CameraCutSection = Cast<UMovieSceneSingleCameraCutSection>(Section);

		if (CameraCutSection)
		{
			CameraCutSection->Modify();
			CameraCutSection->SetCameraGuid(CameraGuid);
		}
		else
		{
			CameraCutTrack->Modify();

			UMovieSceneSingleCameraCutSection* NewSection = Cast<UMovieSceneSingleCameraCutSection>(CameraCutTrack->CreateNewSection());
			NewSection->SetRange(OwnerMovieScene->GetPlaybackRange());
			NewSection->SetCameraGuid(CameraGuid);
			CameraCutTrack->AddSection(*NewSection);
		}
	}
}

//---

//static
void
ShotSequenceHelpers::SnapCameraToViewport( TSharedPtr<ISequencer> iSequencer )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    if( !sequence )
        return;
    UMovieScene* movieScene = sequence->GetMovieScene();
    if( !movieScene )
        return;

    if( movieScene->IsReadOnly() )
    {
        //ShowReadOnlyError();
        return;
    }

    FGuid CameraGuid;
    ACineCameraActor* ExistingCamera = ShotSequenceHelpers::GetCamera( iSequencer, &CameraGuid );

    if( !ExistingCamera )
        return;

    UMovieSceneTrack* track = movieScene->FindTrack( UMovieScene3DTransformTrack::StaticClass(), CameraGuid );
    UMovieScene3DTransformTrack* transform_track = Cast<UMovieScene3DTransformTrack>( track );

    if( !transform_track )
        return;

    //---

    if( !transform_track->GetAllSections().Num() )
        return;

    //---

    UMovieSceneSection* section = transform_track->GetAllSections()[0];
    UMovieScene3DTransformSection* transform_section = Cast<UMovieScene3DTransformSection>( section );

    if( !transform_section )
        return;


    //---

    FVector new_location = GCurrentLevelEditingViewportClient->GetViewLocation();
    FRotator new_rotation = GCurrentLevelEditingViewportClient->GetViewRotation();
    //FVector Scale = iActor->GetActorScale();

    TArrayView<FMovieSceneFloatChannel*> FloatChannels = transform_section->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>();
    FloatChannels[0]->SetDefault( new_location.X );
    FloatChannels[1]->SetDefault( new_location.Y );
    FloatChannels[2]->SetDefault( new_location.Z );

    FloatChannels[3]->SetDefault( new_rotation.Euler().X );
    FloatChannels[4]->SetDefault( new_rotation.Euler().Y );
    FloatChannels[5]->SetDefault( new_rotation.Euler().Z );

    //FloatChannels[6]->SetDefault( Scale.X );
    //FloatChannels[7]->SetDefault( Scale.Y );
    //FloatChannels[8]->SetDefault( Scale.Z );


//TODO: set all (?) planes ?


    ExistingCamera->SetActorLocation( new_location, false );
    ExistingCamera->SetActorRotation( new_rotation );

    //---
    // From FSequencer::NewCameraAdded( CameraGuid, NewCamera )

    //iSequencer->SetPerspectiveViewportCameraCutEnabled( false );

    // Lock the viewport to this camera
    if( ExistingCamera && ExistingCamera->GetLevel() )
    {
        //GCurrentLevelEditingViewportClient->SetMatineeActorLock( nullptr );
        //GCurrentLevelEditingViewportClient->SetActorLock( ExistingCamera );
        //GCurrentLevelEditingViewportClient->bLockedCameraView = true;
        //GCurrentLevelEditingViewportClient->UpdateViewForLockedActor();
        GCurrentLevelEditingViewportClient->Invalidate();
    }
    //---

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//---

#undef LOCTEXT_NAMESPACE
