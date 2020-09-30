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
    *  Camera track was added, we usually do extra things, like add a Camera Cut tracks
    *
    * @param MovieScene MovieScene to add Camera.
    * @param CameraGuid CameraGuid  Guid of the camera that was added.
    * @param FrameNumber FrameNumber it's added at.
    */
    static void CameraAdded(UMovieScene* MovieScene, FGuid CameraGuid, FFrameNumber FrameNumber);

    /**
    *  Add a Camera track
    *
    * @param TSharedPtr<ISequencer> iSequencer to add Camera track and CameraCut track.
    */
    static void CreateCameraAndCameraCut( TSharedPtr<ISequencer> iSequencer );

    /**
    *  Update the camera location from the viewport (and set 
    *
    * @param TSharedPtr<ISequencer> iSequencer to update camera.
    */
    static void SnapCameraToViewport( TSharedPtr<ISequencer> iSequencer );

//---

public:
    /**
    *  Create all default inner sub tracks of added actor
    *
    * @param TSharedPtr<ISequencer> iSequencer to add inner track.
    * @param AActor iActor corresponding to the binding.
    * @param FGuid iBinding  Guid of the track containing the actor.
    */
    static void CreateDefaultInnerTrack( TSharedPtr<ISequencer> iSequencer, AActor* iActor, const FGuid iBinding );

private:
    static UMovieSceneTrack* CreateTrack( TSharedPtr<ISequencer> iSequencer, AActor* iActor, const FGuid& iBinding, UClass* iClass, int iMaterialTrackIndex = INDEX_NONE );
    static FGuid CreateComponentTrack( TSharedPtr<ISequencer> iSequencer, AActor* iActor, const FString& iComponentName );
    static void CreatePropertyTrack( TSharedPtr<ISequencer> iSequencer, AActor* iActor, const FString& iComponentPath, const FString& iPropertyPath );
};
