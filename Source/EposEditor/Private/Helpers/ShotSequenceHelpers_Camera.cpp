// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShotSequenceHelpers.h"

#include "CineCameraActor.h"
#include "ISequencer.h"
#include "LevelEditorViewport.h"
#include "MovieScene.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "Tracks/MovieSceneCinematicShotTrack.h"

#define LOCTEXT_NAMESPACE "ShotSequenceHelpers_Camera"

//static
void
ShotSequenceHelpers::CreateCameraAndCameraCut( TSharedPtr<ISequencer> iSequencer ) // From FSequencer::CreateCamera()
{
    UMovieScene* FocusedMovieScene = iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene();
    if( !FocusedMovieScene )
        return;

    if( FocusedMovieScene->IsReadOnly() )
    {
        //ShowReadOnlyError();
        return;
    }

    UWorld* World = GCurrentLevelEditingViewportClient ? GCurrentLevelEditingViewportClient->GetWorld() : nullptr;
    if( !World )
        return;

    const FScopedTransaction Transaction( LOCTEXT( "CreateStoryCameraHere", "Create Storyboard Camera Here" ) );

//TODO: check if a camera alreay exist and set it to new current coordinates ... and all (?) planes ?

        // Set new camera to match viewport
    FActorSpawnParameters SpawnParams;
    ACineCameraActor* NewCamera = World->SpawnActor<ACineCameraActor>( SpawnParams );
    if( !NewCamera )
        return;

    FGuid CameraGuid = iSequencer->CreateBinding( *NewCamera, NewCamera->GetActorLabel() );

    if( !CameraGuid.IsValid() )
        return;

    NewCamera->SetActorLocation( GCurrentLevelEditingViewportClient->GetViewLocation(), false );
    NewCamera->SetActorRotation( GCurrentLevelEditingViewportClient->GetViewRotation() );
    //pNewCamera->CameraComponent->FieldOfView = ViewportClient->ViewFOV; //@todo set the focal length from this field of view

    iSequencer->OnActorAddedToSequencer().Broadcast( NewCamera, CameraGuid );

    //---
    //NewCameraAdded( CameraGuid, NewCamera ); // From FSequencer::NewCameraAdded()
    {
        iSequencer->SetPerspectiveViewportCameraCutEnabled( false );

        // Lock the viewport to this camera
        if( NewCamera && NewCamera->GetLevel() )
        {
            GCurrentLevelEditingViewportClient->SetMatineeActorLock( nullptr );
            GCurrentLevelEditingViewportClient->SetActorLock( NewCamera );
            GCurrentLevelEditingViewportClient->bLockedCameraView = true;
            GCurrentLevelEditingViewportClient->UpdateViewForLockedActor();
            GCurrentLevelEditingViewportClient->Invalidate();
        }

        UMovieSceneSequence* Sequence = iSequencer->GetFocusedMovieSceneSequence();
        UMovieScene* OwnerMovieScene = Sequence->GetMovieScene();

        ShotSequenceHelpers::CameraAdded( OwnerMovieScene, CameraGuid, iSequencer->GetLocalTime().Time.FloorToFrame() );
    }

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
	{
		return;
	}

	UMovieSceneTrack* CameraCutTrack = OwnerMovieScene->GetCameraCutTrack();

	// If there's a camera cut track with at least one section, no need to change the section
	if (CameraCutTrack && CameraCutTrack->GetAllSections().Num() > 0)
	{
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

#undef LOCTEXT_NAMESPACE
