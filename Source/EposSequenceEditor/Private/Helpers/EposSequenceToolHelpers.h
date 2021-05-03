// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"

#include "KeyParams.h"
#include "Misc/FrameNumber.h"
#include "MovieSceneSequenceID.h"
#include "TransformData.h"

#include "Helpers/ToolkitHelpers.h"

class AActor;
class ACineCameraActor;
class UMaterialInstanceConstant;
class UMaterialInterface;
class UMovieScene;
class UMovieSceneSequence;
class UMovieSceneTrack;
class UTexture2D;
class IMovieScenePlayer;
class ISequencer;

class BoardSequenceToolHelpers
{
// Inside EspoSequenceToolHelpers_Camera
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

// Inside EspoSequenceToolHelpers_Plane
public:
    /**
    *  Add a Camera track
    *
    * @param ISequencer iSequencer to add Camera track and CameraCut track.
    */
    static void CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static int32 GetPlanes( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<AStaticMeshActor*>* oPlanes = nullptr, TArray<FGuid>* oPlaneBindings = nullptr );

// Inside EspoSequenceToolHelpers_Drawing
public:
    /**
    *  Add a drawing (material/texture)
    *
    * @param ISequencer iSequencer to add a drawing.
    */
    static void CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static bool CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

public:
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
};

//---

class ShotSequenceToolHelpers
{
private:
    friend BoardSequenceToolHelpers;
    friend ToolkitHelpers;

// Inside EspoSequenceToolHelpers_Camera
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
    static void CreateCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );

    static ACineCameraActor* SpawnCamera( UWorld* iWorld, const FTransform& iTransform );
    static ACineCameraActor* SpawnAndBindCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid* oGuid );
    static void CameraAdded( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid CameraGuid, ACineCameraActor* iCamera, FFrameNumber FrameNumber );
    static void CreateCameraCut( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, FFrameNumber iFrameNumber );

    static bool SnapCameraToViewport( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, ACineCameraActor* ioCamera, FGuid iCameraGuid, FFrameNumber iFrameNumber, const FTransform& iNewTransform, EMovieSceneKeyInterpolation iInterpolation );
    static void SnapCameraToViewport( ISequencer& iSequencer, UMovieSceneSequence* iSequence, ACineCameraActor* ioCamera, FGuid iCameraGuid, FFrameNumber iFrameNumber );
    static void StopPilotingCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform );

// Inside EspoSequenceToolHelpers_Plane
public:
    /**
    *  Add a Camera track
    *
    * @param ISequencer iSequencer to add a plane.
    */
    static void CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static int32 GetPlanes( ISequencer* iSequencer, TArray<AStaticMeshActor*>* oPlanes = nullptr, TArray<FGuid>* oPlaneBindings = nullptr );

private:
    static void CreatePlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );

    static FVector ComputePlaneScale( const ACineCameraActor* iCamera, float iDistance );
    static AStaticMeshActor* SpawnPlane( UWorld* iWorld, ACineCameraActor* iCamera, UMaterialInstanceConstant* iMaterial );
    static void SpawnAndBindPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, ACineCameraActor* iCamera, FFrameNumber iFrameNumber );

    static UMaterialInstanceConstant* CreateMaterialInstanceConstantAsset( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName );
    static UTexture2D* CreateTexture2DAsset( UMovieSceneSequence* iSequence, UMaterialInterface* iMaterial, FString& oPackageName, FString& oAssetName );

// Inside EspoSequenceToolHelpers_Drawing
public:
    /**
    *  Add a drawing (material/texture)
    *
    * @param ISequencer iSequencer to add a drawing.
    */
    static void CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static bool CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

private:
    static void CreateDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

public:
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

private:
    static void GotoPreviousDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );
    static void GotoNextDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );

//---

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
};
