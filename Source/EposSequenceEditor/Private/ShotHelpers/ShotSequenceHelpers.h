// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"

#include "MovieSceneSequenceID.h"

class AActor;
class ACineCameraActor;
class UMovieScene;
class UMovieSceneTrack;
class ISequencer;

class EPOSSEQUENCEEDITOR_API ShotSequenceHelpers
{
public:
    /**
    *  Find a Camera from the camera track
    *
    * @param ISequencer iSequencer to add Camera track and CameraCut track.
    */
    static ACineCameraActor* GetCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid* oGuid );

    /**
    *  Add a Camera track
    *
    * @param ISequencer iSequencer to add Camera track and CameraCut track.
    */
    static void CreateCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );

    /**
    *  Update the camera location from the viewport
    *
    * @param ISequencer iSequencer to update camera.
    */
    static void SnapCameraToViewport( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );

    /**
    *  Add a Camera track
    *
    * @param ISequencer iSequencer to add Camera track and CameraCut track.
    */
    static void CreatePlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );

private:
    static ACineCameraActor* SpawnAndBindCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid* oGuid );
    static void CameraAdded( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid CameraGuid, ACineCameraActor* iCamera, FFrameNumber FrameNumber );
    static void CreateCameraCut( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, FFrameNumber iFrameNumber );
    static void SpawnAndBindPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, ACineCameraActor* iCamera, FFrameNumber iFrameNumber );

    static FVector ComputePlaneScale( const ACineCameraActor* iCamera, float iDistance );

private:
    class cTemporarySwitchInner
    {
    public:
        cTemporarySwitchInner( ISequencer& iSequencer, FMovieSceneSequenceIDRef iInnerID );
        ~cTemporarySwitchInner();
    private:
        ISequencer& mSequencer;
        FMovieSceneSequenceID mOriginalId;
        FFrameTime mOriginalGlobalTime;
    };

//---

public:
    /**
    *  Create all default inner sub tracks of added actor
    *
    * @param ISequencer iSequencer to add inner track.
    * @param AActor iActor corresponding to the binding.
    * @param FGuid iBinding  Guid of the track containing the actor.
    */
    static void CreateDefaultTracksForActor( ISequencer* iSequencer, AActor* iActor, const FGuid iBinding );

    /**
    *  Fix the binding of the cameracut track when a new camera is dropped
    *
    * @param ISequencer iSequencer to add inner track.
    * @param AActor iActor corresponding to the binding.
    * @param FGuid iBinding  Guid of the track containing the actor.
    */
    static void FixCameraBindingOnCameraCut( ISequencer* iSequencer, AActor* iActor, const FGuid iBinding );

    // PATCH
    static void PatchStandardCameraCutTrack( ISequencer* iSequencer, AActor* iActor, const FGuid iBinding );

private:
    static UMovieSceneTrack* CreateTrack( ISequencer* iSequencer, AActor* iActor, const FGuid& iBinding, UClass* iClass, int iMaterialTrackIndex = INDEX_NONE );
    static FGuid CreateComponentTrack( ISequencer* iSequencer, AActor* iActor, const FString& iComponentName );
    static void CreatePropertyTrack( ISequencer* iSequencer, AActor* iActor, const FGuid& iBinding, UClass* iClass, const FString& iComponentPath, const FString& iPropertyPath );
};
