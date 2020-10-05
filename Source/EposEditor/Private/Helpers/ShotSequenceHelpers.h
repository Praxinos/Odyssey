// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"

class AActor;
class ACineCameraActor;
class UMovieScene;
class UMovieSceneTrack;
class ISequencer;

class EPOSEDITOR_API ShotSequenceHelpers
{
public:
    /**
    *  Find a Camera from the camera track
    *
    * @param TSharedPtr<ISequencer> iSequencer to add Camera track and CameraCut track.
    */
    static ACineCameraActor* GetCamera( TSharedPtr<ISequencer> iSequencer, FGuid* oGuid );

    /**
    *  Add a Camera track
    *
    * @param TSharedPtr<ISequencer> iSequencer to add Camera track and CameraCut track.
    */
    static void CreateCamera( TSharedPtr<ISequencer> iSequencer );

    /**
    *  Update the camera location from the viewport (and set 
    *
    * @param TSharedPtr<ISequencer> iSequencer to update camera.
    */
    static void SnapCameraToViewport( TSharedPtr<ISequencer> iSequencer );

    /**
    *  Add a Camera track
    *
    * @param TSharedPtr<ISequencer> iSequencer to add Camera track and CameraCut track.
    */
    static void CreatePlane( TSharedPtr<ISequencer> iSequencer );

private:
    static ACineCameraActor* CreateCamera( TSharedPtr<ISequencer> iSequencer, FGuid* oGuid );
    static void CameraAdded( TSharedPtr<ISequencer> iSequencer, FGuid CameraGuid, const ACineCameraActor* iCamera, FFrameNumber FrameNumber );
    static void CreateCameraCut( TSharedPtr<ISequencer> iSequencer, FGuid iCameraGuid, FFrameNumber iFrameNumber );
    static void CreatePlane( TSharedPtr<ISequencer> iSequencer, FGuid iCameraGuid, const ACineCameraActor* iCamera, FFrameNumber iFrameNumber );

    static FVector ComputePlaneScale( const ACineCameraActor* iCamera, float iDistance );

//---

public:
    /**
    *  Create all default inner sub tracks of added actor
    *
    * @param TSharedPtr<ISequencer> iSequencer to add inner track.
    * @param AActor iActor corresponding to the binding.
    * @param FGuid iBinding  Guid of the track containing the actor.
    */
    static void CreateDefaultTracksForActor( TSharedPtr<ISequencer> iSequencer, AActor* iActor, const FGuid iBinding );

private:
    static UMovieSceneTrack* CreateTrack( TSharedPtr<ISequencer> iSequencer, AActor* iActor, const FGuid& iBinding, UClass* iClass, int iMaterialTrackIndex = INDEX_NONE );
    static FGuid CreateComponentTrack( TSharedPtr<ISequencer> iSequencer, AActor* iActor, const FString& iComponentName );
    static void CreatePropertyTrack( TSharedPtr<ISequencer> iSequencer, AActor* iActor, const FString& iComponentPath, const FString& iPropertyPath );
};
