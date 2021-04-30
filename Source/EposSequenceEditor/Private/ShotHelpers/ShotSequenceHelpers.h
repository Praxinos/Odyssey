// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"

#include "KeyParams.h"
#include "MovieSceneSequenceID.h"
#include "TransformData.h"

class AActor;
class ACineCameraActor;
class UMovieScene;
class UMovieSceneSequence;
class UMovieSceneTrack;
class IMovieScenePlayer;
class ISequencer;

class EPOSSEQUENCEEDITOR_API BoardSequenceToolHelpers
{
public:
    /**
    *  Find a Camera from the camera track
    *
    * @param ISequencer iSequencer to add Camera track and CameraCut track.
    */
    static ACineCameraActor* GetCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid* oCameraBinding = nullptr );

    static bool CanCreateCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Add a Camera track
    *
    * @param ISequencer iSequencer to add Camera track and CameraCut track.
    */
    static void CreateCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Add a Camera track
    *
    * @param ISequencer iSequencer to add Camera track and CameraCut track.
    */
    static void CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Add a drawing (material/texture)
    *
    * @param ISequencer iSequencer to add a drawing.
    */
    static void CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static bool CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static int32 GetPlanes( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<AStaticMeshActor*>* oPlanes = nullptr, TArray<FGuid>* oPlaneBindings = nullptr );

    /**
    *  Go to the previous drawing
    *
    * @param ISequencer iSequencer to add a drawing.
    */
    static void GotoPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool HasPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Go to the next drawing
    *
    * @param ISequencer iSequencer to add a drawing.
    */
    static void GotoNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool HasNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Update the camera location from the viewport
    *
    * @param ISequencer iSequencer to update camera.
    */
    static void SnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Stop piloting camera
    *
    * @param ISequencer iSequencer to add Camera track and CameraCut track.
    */
    static void StopPilotingCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform );
};

class EPOSSEQUENCEEDITOR_API ShotSequenceToolHelpers
{
public:
    /**
    *  Find a Camera from the camera track
    *
    * @param ISequencer iSequencer to add Camera track and CameraCut track.
    */
    static ACineCameraActor* GetCamera( ISequencer* iSequencer, FGuid* oCameraBinding = nullptr );

    /**
    *  Add a Camera track
    *
    * @param ISequencer iSequencer to add Camera track and CameraCut track.
    */
    static void CreateCamera( ISequencer* iSequencer );

    /**
    *  Add a Camera track
    *
    * @param ISequencer iSequencer to add a plane.
    */
    static void CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Add a drawing (material/texture)
    *
    * @param ISequencer iSequencer to add a drawing.
    */
    static void CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static bool CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static int32 GetPlanes( ISequencer* iSequencer, TArray<AStaticMeshActor*>* oPlanes = nullptr, TArray<FGuid>* oPlaneBindings = nullptr );

    /**
    *  Go to the previous drawing
    *
    * @param ISequencer iSequencer to add a drawing.
    */
    static void GotoPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool HasPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Go to the next drawing
    *
    * @param ISequencer iSequencer to add a drawing.
    */
    static void GotoNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool HasNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Update the camera location from the viewport
    *
    * @param ISequencer iSequencer to update camera.
    */
    static void SnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Stop piloting camera
    *
    * @param ISequencer iSequencer to add Camera track and CameraCut track.
    */
    static void StopPilotingCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform );

private:
    friend BoardSequenceToolHelpers;
    static void CreateCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static void CreatePlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );
    static void CreateDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding );
    static bool CanCreateDrawing( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding );
    static int32 GetPlanes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<AStaticMeshActor*>* oPlanes, TArray<FGuid>* oPlaneBindings );
    static void GotoPreviousDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );
    static void GotoNextDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );
    static TArray<FFrameNumber> GetAllMaterialTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static bool SnapCameraToViewport( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, ACineCameraActor* ioCamera, FGuid iCameraGuid, FFrameNumber iFrameNumber, const FTransform& iNewTransform, EMovieSceneKeyInterpolation iInterpolation );
    static void SnapCameraToViewport( ISequencer& iSequencer, UMovieSceneSequence* iSequence, ACineCameraActor* ioCamera, FGuid iCameraGuid, FFrameNumber iFrameNumber );
    static void StopPilotingCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform );

    static ACineCameraActor* SpawnAndBindCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid* oGuid );
    static void CameraAdded( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid CameraGuid, ACineCameraActor* iCamera, FFrameNumber FrameNumber );
    static void CreateCameraCut( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, FFrameNumber iFrameNumber );
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
