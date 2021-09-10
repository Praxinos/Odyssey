// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "KeyParams.h"
#include "Misc/FrameNumber.h"
#include "MovieSceneSequenceID.h"
#include "TransformData.h"

class AActor;
class ACineCameraActor;
class APlaneActor;
class UMaterialInstanceConstant;
class UMaterialInterface;
class UMovieScene;
class UMovieSceneCinematicBoardSection;
class UMovieSceneCinematicBoardTrack;
class UMovieSceneSection;
class UMovieSceneSequence;
class UMovieSceneSubSection;
class UMovieSceneTrack;
class UTexture2D;
class IMovieScenePlayer;
class ISequencer;

class EPOSTRACKSEDITOR_API CinematicBoardTrackTools
{
public:
    /*
     * Insert Board.
     *
     * @param iSequencer The sequencer
     * @param iFrameNumber The start frame of the new section/board
     * @param iDuration The (optional) duration of the new section/board
     */
    static UMovieSceneSubSection* InsertBoard( ISequencer* iSequencer, FFrameNumber iFrameNumber, TOptional<int32> iDuration = TOptional<int32>() );

    /*
     * Insert Shot.
     *
     * @param iSequencer The sequencer
     * @param iFrameNumber The start frame of the new section/shot
     * @param iDuration The (optional) duration of the new section/shot
     */
    static UMovieSceneSubSection* InsertShot( ISequencer* iSequencer, FFrameNumber iFrameNumber, TOptional<int32> iDuration = TOptional<int32>() );

    /*
     * Insert Filler.
     *
     * @param iSequencer The sequencer
     */
    //static void InsertFiller( ISequencer* iSequencer );

    /*
     * Duplicate board.
     *
     * @param iSequencer The sequencer
     * @param iSection The section to duplicate
     */
    static void DuplicateSection( ISequencer* iSequencer, UMovieSceneCinematicBoardSection* iSection );

private:

    /**
     * Generate a new board package
     *
     * @param iRootMovieScene The root sequence movie scene
     * @param iFocusedMovieScene The sequence movie scene for the new board
     * @param ioNewBoardName The new board name
     * @return The new board path
     */
    static FString GenerateNewSequencePath( UMovieScene* iRootMovieScene, UMovieScene* iFocusedMovieScene, FString& ioNewBoardName );

    /**
     * Generate a new board name
     *
     * @param iAllSections All the sections in the given board track
     * @param iTime The time to generate the new board name at
     * @return The new board name
     */
    template<typename SequenceClass>
    static FString GenerateNewSectionName( const TArray<UMovieSceneSection*>& iAllSections, FFrameNumber iTime );

    /*
     * Create board
     *
     * @param iSequencer The sequencer
     * @param ioNewSequenceName The new board name.
     * @param iNewSectionStartTime The time to start the new board at.
     * @param iDuration The (optional) duration of the new section/board
     * @param iSectionToDuplicate The board to duplicate.
     * @return The new board.
     */
    template<typename SequenceClass>
    static UMovieSceneSubSection* CreateSequenceInternal( ISequencer* iSequencer, FString& ioNewSequenceName, FFrameNumber iNewSectionStartTime, TOptional<int32> iDuration = TOptional<int32>(), UMovieSceneCinematicBoardSection* iSectionToDuplicate = nullptr );

    /*
     * Insert Board.
     *
     * @param iSequencer The sequencer
     * @param iFrameNumber The start frame of the new section/board
     * @param iDuration The (optional) duration of the new section/board
     */
    template<typename SequenceClass>
    static UMovieSceneSubSection* InsertSequence( ISequencer* iSequencer, FFrameNumber iFrameNumber, TOptional<int32> iDuration = TOptional<int32>() );
};

//---

class EPOSTRACKSEDITOR_API BoardSequenceTools
{
// Inside EspoSequenceTools_Board
public:
    /** Find a cinematic board track in the currently focused movie scene. */
    static UMovieSceneCinematicBoardTrack* FindCinematicBoardTrack( ISequencer* iSequencer );

    /** Find or create a cinematic board track in the currently focused movie scene. */
    static UMovieSceneCinematicBoardTrack* FindOrCreateCinematicBoardTrack( ISequencer* iSequencer );

    /** Stretch sequencer time range to view make the new range inside the view. */
    static void UpdateViewRange( ISequencer* iSequencer, TRange<FFrameNumber> iNewRange );

// Inside EspoSequenceTools_Camera
public:
    /**
    *  Find the camera of the board section
    *
    * @param ISequencer         iSequencer to get the camera.
    * @param FFrameNumber       iFrameNumber to get the board section.
    * @param FGuid*             oCameraBinding to get the camera binding.
    * @return ACineCameraActor* the camera actor.
    */
    static ACineCameraActor* GetCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid* oCameraBinding = nullptr );

    /**
    *  Can a camera be created in the board section ?
    *
    * @param ISequencer         iSequencer to get the camera.
    * @param FFrameNumber       iFrameNumber to get the board section.
    * @return bool
    */
    static bool CanCreateCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Create a new camera (actor & track & cameracut track) in the board section
    *
    * @param ISequencer     iSequencer to add a new camera.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static void CreateCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber );

public:
    /**
    *  Update the camera location from the viewport
    *
    * @param ISequencer     iSequencer to update camera.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static void SnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Check if camera can be snap to the viewport
    *
    * @param ISequencer             iSequencer to update camera.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static bool CanSnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Update the camera location from the viewport
    *
    * @param ISequencer             iSequencer to update camera.
    * @param UMovieSceneSubSection  iSubSection to update camera.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static void SnapCameraToViewport( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber );

    /**
    *  Check if camera can be snap to the viewport
    *
    * @param ISequencer             iSequencer to update camera.
    * @param UMovieSceneSubSection  iSubSection to update camera.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static bool CanSnapCameraToViewport( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber );

public:
    /**
    * Check if camera is in piloting mode in the subsection at the given frame
    *
    * @param ISequencer             iSequencer to update camera.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static bool IsPilotingCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    * Check if camera is in piloting mode in the given subsection
    *
    * @param ISequencer             iSequencer to update camera.
    * @param UMovieSceneSubSection  iSubSection to update camera.
    */
    static bool IsPilotingCamera( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection );

    /**
    * Start piloting the camera in the subsection at the given frame
    *
    * @param ISequencer             iSequencer to update camera.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static void PilotCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    * Start piloting the camera in the given subsection
    * (The given frame must be inside the subsection)
    *
    * @param ISequencer             iSequencer to update camera.
    * @param UMovieSceneSubSection  iSubSection to update camera.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static void PilotCamera( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber );

    /**
    *  Check if camera can be pilot in the subsection at the given frame
    *
    * @param ISequencer             iSequencer to update camera.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static bool CanPilotCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Check if camera can be pilot in the given subsection
    * (The given frame must be inside the subsection)
    *
    * @param ISequencer             iSequencer to update camera.
    * @param UMovieSceneSubSection  iSubSection to update camera.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static bool CanPilotCamera( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber );

    /**
    *  Eject the camera in the subsection at the given frame
    *
    * @param ISequencer             iSequencer to update camera.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static void EjectCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Eject the camera in the given subsection
    * (The given frame must be inside the subsection)
    *
    * @param ISequencer             iSequencer to update camera.
    * @param UMovieSceneSubSection  iSubSection to update camera.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static void EjectCamera( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber );

    /**
    *  Check if camera can be eject in the subsection at the given frame
    *
    * @param ISequencer             iSequencer to update camera.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static bool CanEjectCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Check if camera can be eject in the given subsection
    * (The given frame must be inside the subsection)
    *
    * @param ISequencer             iSequencer to update camera.
    * @param UMovieSceneSubSection  iSubSection to update camera.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static bool CanEjectCamera( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber );

    /**
    *  Stop piloting camera
    *
    * @param ISequencer                 iSequencer to stop piloting camera.
    * @param FFrameNumber               iFrameNumber to get the board section.
    * @param ACineCameraActor*          iCamera which was piloted.
    * @param TOptional<FTransformData>  iPreviousTransform to know where the camera was before piloting.
    * @param FTransformData             iNewTransform to know its new location.
    */
    static void StopPilotingCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform );

public:
    /**
    *  Go to the previous camera position in the board section
    *
    * @param ISequencer     iSequencer to find the previous drawing.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static void GotoPreviousCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Is there a previous camera position in the board section ?
    *
    * @param ISequencer     iSequencer to find the previous drawing.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static bool HasPreviousCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Go to the next camera position in the board section
    *
    * @param ISequencer     iSequencer to find the next drawing.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static void GotoNextCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Is there a next camera position in the board section ?
    *
    * @param ISequencer     iSequencer to find the next drawing.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static bool HasNextCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber );

// Inside EspoSequenceTools_Plane
public:
    /**
    *  Create a new plane (actor & track) in the board section
    *
    * @param ISequencer     iSequencer to add a new plane.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static void CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Can a plane be created in the board section ?
    *
    * @param ISequencer         iSequencer to get the camera.
    * @param FFrameNumber       iFrameNumber to get the board section.
    * @return bool
    */
    static bool CanCreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Get all planes (actor & track bindings) in the board section
    *
    * @param ISequencer     iSequencer to get planes.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param TArray<APlaneActor*>* oPlanes to get all plane actors.
    * @param TArray<FGuid>*        oPlaneBindings to get all plane bindings.
    */
    static int32 GetAllPlanes( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<APlaneActor*>* oPlanes = nullptr, TArray<FGuid>* oPlaneBindings = nullptr );

    /**
    *  Detach a plane of the camera in the board section
    *
    * @param ISequencer     iSequencer to detach a plane.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param APlaneActor*   iPlane to detach.
    */
    static void DetachPlane( ISequencer* iSequencer, FFrameNumber iFrameNumber, APlaneActor* iPlane );

    /**
    *  Get all planes (actor & track bindings) attached to the camera in the board section
    *
    * @param ISequencer     iSequencer to get planes.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param TArray<APlaneActor*>* oPlanes to get all plane actors.
    * @param TArray<FGuid>*        oPlaneBindings to get all plane bindings.
    */
    static int32 GetAttachedPlanes( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<APlaneActor*>* oPlanes = nullptr, TArray<FGuid>* oPlaneBindings = nullptr );

// Inside EspoSequenceTools_Drawing
public:
    /**
    *  Create a new drawing (material & texture) in a plane in the board section
    *
    * @param ISequencer     iSequencer to add a new drawing.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iPlaneBinding to get the plane track.
    */
    static void CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    /**
    *  Create a new drawing (material & texture) in a plane in the board section
    *
    * @param ISequencer     iSequencer to add a new drawing.
    * @param UMovieSceneSubSection  iSubSection to get the plane.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iPlaneBinding to get the plane track.
    */
    static void CreateDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    /**
    *  Can a drawing be created in the board section ?
    *
    * @param ISequencer     iSequencer to get the plane.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iPlaneBinding to get the plane track.
    * @return bool
    */
    static bool CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    /**
    *  Can a drawing be created in the board section ?
    *
    * @param ISequencer             iSequencer to get the plane.
    * @param UMovieSceneSubSection  iSubSection to get the plane.
    * @param FFrameNumber           iFrameNumber to get the board section.
    * @param FGuid                  iPlaneBinding to get the plane track.
    * @return bool
    */
    static bool CanCreateDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static void CloneDrawing( ISequencer* iSequencer, UMaterialInstance* iMaterialToClone, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static void DeleteDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

public:
    /**
    *  Go to the previous drawing in the board section
    *
    * @param ISequencer     iSequencer to find the previous drawing.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static void GotoPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Is there a previous drawing in the board section ?
    *
    * @param ISequencer     iSequencer to find the previous drawing.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static bool HasPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Go to the next drawing in the board section
    *
    * @param ISequencer     iSequencer to find the next drawing.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static void GotoNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Is there a next drawing in the board section ?
    *
    * @param ISequencer     iSequencer to find the next drawing.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static bool HasNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber );
};

//---

class EPOSTRACKSEDITOR_API ShotSequenceTools
{
private:
    friend class BoardSequenceTools;
    friend class ToolkitHelpers;

// Inside EspoSequenceTools_Camera
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

    static bool CanCreateCamera( ISequencer* iSequencer );

    /**
    *  Update the camera location from the viewport
    *
    * @param ISequencer iSequencer to update camera.
    */
    static void SnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool CanSnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool IsPilotingCamera( ISequencer* iSequencer );

    static void PilotCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool CanPilotCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static void EjectCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool CanEjectCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber );

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
    static bool IsPilotingCamera( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static void PilotCamera( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );
    static bool CanPilotCamera( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );
    static void EjectCamera( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );
    static bool CanEjectCamera( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );
    static void StopPilotingCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform );

public:
    /**
    *  Go to the previous camera position
    *
    * @param ISequencer iSequencer to get camera.
    */
    static void GotoPreviousCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool HasPreviousCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Go to the next camera position
    *
    * @param ISequencer iSequencer to get camera.
    */
    static void GotoNextCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool HasNextCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber );

private:
    static void GotoPreviousCameraPosition( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );
    static void GotoNextCameraPosition( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );

// Inside EspoSequenceTools_Plane
public:
    /**
    *  Add a Camera track
    *
    * @param ISequencer iSequencer to add a plane.
    */
    static void CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool CanCreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static int32 GetAllPlanes( ISequencer* iSequencer, TArray<APlaneActor*>* oPlanes = nullptr, TArray<FGuid>* oPlaneBindings = nullptr );

    static void DetachPlane( ISequencer* iSequencer, APlaneActor* iPlane );

    static int32 GetAttachedPlanes( ISequencer* iSequencer, TArray<APlaneActor*>* oPlanes = nullptr, TArray<FGuid>* oPlaneBindings = nullptr );

private:
    static void CreatePlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );
    static void DetachPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, APlaneActor* iPlane );

    static FVector ComputePlaneScale( const ACineCameraActor* iCamera, float iDistance );
    static APlaneActor* SpawnPlane( UWorld* iWorld, ACineCameraActor* iCamera, UMaterialInstanceConstant* iMaterial );
    static void SpawnAndBindPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, ACineCameraActor* iCamera, FFrameNumber iFrameNumber );

// Inside EspoSequenceTools_Drawing
public:
    /**
    *  Add a drawing (material/texture)
    *
    * @param ISequencer iSequencer to add a drawing.
    */
    static void CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static bool CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static void CloneDrawing( ISequencer* iSequencer, UMaterialInstance* iMaterialToClone, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static void DeleteDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

private:
    static void CreateDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding );
    static void CloneDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMaterialInstance* iMaterialToClone, FFrameNumber iFrameNumber, FGuid iPlaneBinding );
    static void DeleteDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

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
