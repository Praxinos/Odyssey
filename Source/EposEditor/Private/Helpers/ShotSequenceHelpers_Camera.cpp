// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShotSequenceHelpers.h"

#include "Channels/MovieSceneFloatChannel.h"
#include "CineCameraActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
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
ShotSequenceHelpers::CreateCamera( TSharedPtr<ISequencer> iSequencer )
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

    //---

    FGuid camera_guid;
    ACineCameraActor* camera = CreateCamera( iSequencer, &camera_guid );

    ShotSequenceHelpers::CameraAdded( iSequencer, camera_guid, camera, iSequencer->GetLocalTime().Time.FloorToFrame() );

    //---

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}

//static
ACineCameraActor*
ShotSequenceHelpers::CreateCamera( TSharedPtr<ISequencer> iSequencer, FGuid* oGuid ) // From FSequencer::CreateCamera()
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    UMovieScene* movieScene = sequence->GetMovieScene();
    UWorld* World = GCurrentLevelEditingViewportClient->GetWorld();

    //---

    FGuid CameraGuid;
    ACineCameraActor* ExistingCamera = ShotSequenceHelpers::GetCamera( iSequencer, &CameraGuid );

    if( ExistingCamera )
        return nullptr;

    //---

    const FScopedTransaction Transaction( LOCTEXT( "CreateStoryCameraHere", "Create Storyboard Camera Here" ) );

    // Set new camera to match viewport
    FActorSpawnParameters SpawnParams;
    ACineCameraActor* NewCamera = World->SpawnActor<ACineCameraActor>( SpawnParams );
    if( !NewCamera )
        return nullptr;

    CameraGuid = iSequencer->CreateBinding( *NewCamera, NewCamera->GetActorLabel() );
    if( !CameraGuid.IsValid() )
        return nullptr;

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

    *oGuid = CameraGuid;
    return NewCamera;
}

//static
void
ShotSequenceHelpers::CameraAdded( TSharedPtr<ISequencer> iSequencer, FGuid CameraGuid, const ACineCameraActor* iCamera, FFrameNumber FrameNumber)
{
    CreateCameraCut( iSequencer, CameraGuid, FrameNumber );

    CreatePlanes( iSequencer, CameraGuid, iCamera, FrameNumber );
}

//static
void
ShotSequenceHelpers::CreateCameraCut( TSharedPtr<ISequencer> iSequencer, FGuid iCameraGuid, FFrameNumber iFrameNumber ) // From MovieSceneToolHelpers::CameraAdded()
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    UMovieScene* movieScene = sequence->GetMovieScene();
    UWorld* World = GCurrentLevelEditingViewportClient->GetWorld();

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
        UMovieSceneSection* Section = MovieSceneHelpers::FindSectionAtTime( CameraCutTrack->GetAllSections(), iFrameNumber );
        UMovieSceneSingleCameraCutSection* CameraCutSection = Cast<UMovieSceneSingleCameraCutSection>( Section );

        if( CameraCutSection )
        {
            CameraCutSection->Modify();
            CameraCutSection->SetCameraGuid( iCameraGuid );
        }
        else
        {
            CameraCutTrack->Modify();

            UMovieSceneSingleCameraCutSection* NewSection = Cast<UMovieSceneSingleCameraCutSection>( CameraCutTrack->CreateNewSection() );
            NewSection->SetRange( movieScene->GetPlaybackRange() );
            NewSection->SetCameraGuid( iCameraGuid );
            CameraCutTrack->AddSection( *NewSection );
        }
    }
}

//static
void
ShotSequenceHelpers::CreatePlanes( TSharedPtr<ISequencer> iSequencer, FGuid iCameraGuid, const ACineCameraActor* iCamera, FFrameNumber iFrameNumber )
{
    UWorld* World = GCurrentLevelEditingViewportClient->GetWorld();

    FTransform camera_transform = iCamera->GetRootComponent()->GetComponentTransform();

    //---

    FVector const CamLocation = camera_transform.GetLocation();
    FVector const CamDir = camera_transform.GetRotation().Vector();
    FRotator const CamRot = camera_transform.Rotator();

    //---

    // Make a function ComputePlaneLocation(...)
    float FocusDistance = 200;
    FVector plane_location = CamLocation + CamDir * FocusDistance;

    // Make a function ComputePlaneScale(...)
    FVector plane_scale( 1.5f, 1.f, 1.f );

    //---

    UStaticMesh* Mesh = LoadObject<UStaticMesh>( nullptr, TEXT( "/Engine/BasicShapes/Plane.Plane" ) );
    check( Mesh );

    FActorSpawnParameters SpawnParams;
    AStaticMeshActor* plane = World->SpawnActor<AStaticMeshActor>( SpawnParams );

    plane->GetStaticMeshComponent()->SetStaticMesh( Mesh );

    plane->SetActorScale3D( plane_scale );
    plane->SetActorLocation( plane_location );
    plane->SetActorRotation( FRotator( 0.f, 90.f, 90.f ) );
    plane->AddActorWorldRotation( CamRot );

    //---

    FGuid planeGuid = iSequencer->CreateBinding( *plane, plane->GetActorLabel() );

    iSequencer->OnActorAddedToSequencer().Broadcast( plane, planeGuid );
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
