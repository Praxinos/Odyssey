// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "KeyParams.h"
#include "Misc/FrameNumber.h"
#include "MovieSceneSequenceID.h"
#include "TransformData.h"

#include "EposSequenceTools.generated.h"

class AActor;
class ACineCameraActor;
class APlaneActor;
class UMaterialInstanceConstant;
class UMaterialInterface;
class UMovieScene;
class UMovieSceneCinematicBoardSection;
class UMovieSceneCinematicBoardTrack;
class UMovieSceneNoteSection;
class UMovieSceneSection;
class UMovieSceneSequence;
class UMovieSceneSubSection;
class UMovieSceneTrack;
class UShotSequence;
class UTexture2D;
class IMovieScenePlayer;
class ISequencer;
struct FBoardSectionTake;
struct FDrawing; // but only as private functions
struct FMovieSceneChannelHandle;

struct FCameraArgs
{
    FString mName;
};

struct FPlaneArgs
{
    FString mName;
};

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
    //static void DuplicateSection( ISequencer* iSequencer, UMovieSceneCinematicBoardSection* iSection );

    /*
     * Clone board.
     *
     * @param iSequencer The sequencer
     * @param iSection The section to clone
     */
    static void CloneSection( ISequencer* iSequencer, UMovieSceneCinematicBoardSection* iSection, FFrameNumber iFrameNumber, bool iEmptyDrawings );

private:

    /**
     * Generate a new board package
     *
     * @param iRootMovieScene The root sequence movie scene
     * @param iFocusedMovieScene The sequence movie scene for the new board
     * @param ioNewBoardName The new board name
     * @return The new board path
     */
    //static FString GenerateNewSequencePath( UMovieScene* iRootMovieScene, UMovieScene* iFocusedMovieScene, FString& ioNewBoardName );

    /**
     * Generate a new board name
     *
     * @param iAllSections All the sections in the given board track
     * @param iTime The time to generate the new board name at
     * @return The new board name
     */
    //template<typename SequenceClass>
    //static FString GenerateNewSectionName( const TArray<UMovieSceneSection*>& iAllSections, FFrameNumber iTime );

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
    static UMovieSceneSubSection* CreateSequenceInternal( ISequencer* iSequencer, const FString& iNewSequencePath, const FString& iNewSequenceName, FFrameNumber iNewSectionStartTime, TOptional<int32> iDuration = TOptional<int32>(), UMovieSceneCinematicBoardSection* iSectionToDuplicate = nullptr );
};

//---

class EPOSTRACKSEDITOR_API BoardSequenceTools
{
// Inside EposSequenceTools_Board
public:
    /** Find a cinematic board track in the currently focused movie scene. */
    static UMovieSceneCinematicBoardTrack* FindCinematicBoardTrack( ISequencer* iSequencer );

    /** Find or create a cinematic board track in the currently focused movie scene. */
    static UMovieSceneCinematicBoardTrack* FindOrCreateCinematicBoardTrack( ISequencer* iSequencer );

    /** Stretch sequencer time range to view make the new range inside the view. */
    static void UpdateViewRange( ISequencer* iSequencer, TRange<FFrameNumber> iNewRange );

// Inside EposSequenceTools_Take
public:
    /** Create a new take (from the current subsequence) for the board section. */
    static FBoardSectionTake* CreateTake( ISequencer* iSequencer, UMovieSceneSubSection& iSubSection );

    /** Switch the current take to the new one for the board section. */
    static FBoardSectionTake* SwitchTake( ISequencer* iSequencer, UMovieSceneSubSection& iSubSection, FBoardSectionTake* iTake );

// Inside EposSequenceTools
public:
    static void RenameBinding( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iBinding, FString iNewLabel );

// Inside EposSequenceTools_Camera
public:
    /**
    *  Find the camera of the board section
    *
    * @param ISequencer             iSequencer to get the camera.
    * @param UMovieSceneSubSection  iSubSection to get the camera.
    * @param FGuid*                 oCameraBinding to get the camera binding.
    * @return ACineCameraActor* the camera actor.
    */
    static ACineCameraActor* GetCamera( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid* oCameraBinding = nullptr );

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
    static void CreateCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, const FCameraArgs& iCameraArgs = FCameraArgs(), const FPlaneArgs& iPlaneArgs = FPlaneArgs() );

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
    * Delete a transform key of the camera
    *
    * @param ISequencer             iSequencer to delete the camera key.
    */
    static void DeleteCameraKey( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles );


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

// Inside EposSequenceTools_Plane
public:
    /**
    *  Create a new plane (actor & track) in the board section
    *
    * @param ISequencer     iSequencer to add a new plane.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static void CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber, const FPlaneArgs& iPlaneArgs = FPlaneArgs() );

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
    * @param FGuid          iPlaneBinding to detach.
    */
    static void DetachPlane( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings );
    static void DetachPlane( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    /**
    *  Detach a plane of the camera in the board section
    *
    * @param ISequencer             iSequencer to detach a plane.
    * @param UMovieSceneSubSection  iSubSection to detach a plane.
    * @param FGuid                  iPlaneBinding to detach.
    */
    static void DetachPlane( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iPlaneBindings );
    static void DetachPlane( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding );

    /**
    *  Detach a plane of the camera in the board section
    *
    * @param ISequencer             iSequencer to detach a plane.
    * @param UMovieSceneSubSection  iSubSection to detach a plane.
    * @param FGuid                  iPlaneBinding to detach.
    */
    static bool CanDetachPlane( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iPlaneBindings );
    static bool CanDetachPlane( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding );

    /**
    *  Get all planes (actor & track bindings) attached to the camera in the board section
    *
    * @param ISequencer     iSequencer to get planes.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param TArray<APlaneActor*>* oPlanes to get all plane actors.
    * @param TArray<FGuid>*        oPlaneBindings to get all plane bindings.
    */
    static int32 GetAttachedPlanes( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<APlaneActor*>* oPlanes = nullptr, TArray<FGuid>* oPlaneBindings = nullptr );

    /**
    *  Get a plane visiblity of the camera in the board section
    *
    * @param ISequencer     iSequencer to detach a plane.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iPlaneBinding to get visibility.
    */
    static bool IsPlaneVisible( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    /**
    *  Get a plane visiblity of the camera in the board section
    *
    * @param ISequencer             iSequencer to detach a plane.
    * @param UMovieSceneSubSection  iSubSection to detach a plane.
    * @param FGuid                  iPlaneBinding to get visibility.
    */
    static bool IsPlaneVisible( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding );

    /**
    *  Toggle a plane visiblity of the camera in the board section
    *
    * @param ISequencer     iSequencer to detach a plane.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iPlaneBinding to toggle visibility.
    */
    static void TogglePlaneVisibility( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    /**
    *  Toggle a plane visiblity of the camera in the board section
    *
    * @param ISequencer             iSequencer to detach a plane.
    * @param UMovieSceneSubSection  iSubSection to detach a plane.
    * @param FGuid                  iPlaneBinding to toggle visibility.
    */
    static void TogglePlaneVisibility( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iPlaneBindings, TOptional<FGuid> iPlaneReference = TOptional<FGuid>() );
    static void TogglePlaneVisibility( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding );

    /**
    *  Delete a plane (with its actor) of the camera in the board section
    *
    * @param ISequencer     iSequencer to delete a plane.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iPlaneBinding to detach.
    */
    static void DeletePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings );
    static void DeletePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    /**
    *  Delete a plane (with its actor) of the camera in the board section
    *
    * @param ISequencer             iSequencer to delete a plane.
    * @param UMovieSceneSubSection  iSubSection to delete a plane.
    * @param FGuid                  iPlaneBinding to delete.
    */
    static void DeletePlane( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iPlaneBindings );
    static void DeletePlane( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding );

    /**
    *  Select a plane in the board section (and unselect all other planes)
    *
    * @param ISequencer             iSequencer to select a plane.
    * @param UMovieSceneSubSection  iSubSection to select a plane.
    * @param FGuid                  iPlaneBinding to select.
    */
    static void SelectSinglePlane( ISequencer* iSequencer, UMovieSceneSubSection* iSubSection, FGuid iPlaneBinding );

    /**
    *  Select a plane in the board section (and keep already selected all other planes)
    *
    * @param ISequencer             iSequencer to select a plane.
    * @param UMovieSceneSubSection  iSubSection to select a plane.
    * @param FGuid                  iPlaneBinding to select.
    */
    static void SelectMultiPlane( ISequencer* iSequencer, UMovieSceneSubSection* iSubSection, FGuid iPlaneBinding );

// Inside EposSequenceTools_Drawing
public:
    /**
    *  Create a new drawing (material & texture) in a plane in the board section
    *
    * @param ISequencer     iSequencer to add a new drawing.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iPlaneBinding to get the plane track.
    */
    static void CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings );
    static void CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    /**
    *  Create a new drawing (material & texture) in a plane in the board section
    *
    * @param ISequencer     iSequencer to add a new drawing.
    * @param UMovieSceneSubSection  iSubSection to get the plane.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iPlaneBinding to get the plane track.
    */
    static void CreateDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings );
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
    static bool CanCreateDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings );
    static bool CanCreateDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static bool IsDrawingInEditionMode( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection );

    static void CloneDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, UMovieSceneSection* iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle, FFrameNumber iFrameNumber );

    static bool CanCloneDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );
    static bool CanCloneDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static void DeleteDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles );

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

// Inside EposSequenceTools_Opacity
public:
    static bool CanCreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );
    static bool CanCreateOpacity( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings );
    static bool CanCreateOpacity( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static void CreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings, float iOpacity );
    static void CreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding, float iOpacity );
    static void CreateOpacity( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings, float iOpacity );
    static void CreateOpacity( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iPlaneBinding, float iOpacity );

    static void DeleteOpacity( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles );

    static void SetOpacity( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles, float iOpacity );

// Inside EposSequenceTools_Note
public:
    /**
    * Create a note in the board section
    * (The given frame must be inside the subsection)
    *
    * @param ISequencer             iSequencer to create note.
    * @param UMovieSceneSubSection  iSubSection to create note.
    * @param FFrameNumber           iFrameNumber to create note.
    */
    static void CreateNote( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber );

    /**
    * Delete a note in the board section
    *
    * @param ISequencer             iSequencer to delete note.
    * @param UMovieSceneSubSection  iSubSection to delete note.
    * @param UMovieSceneSection     iNoteSection to delete.
    */
    static void DeleteNote( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TWeakObjectPtr<UMovieSceneSection> iNoteSection );

    static TArray<TWeakObjectPtr<UMovieSceneNoteSection>> GetAllNotes( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection );
};

//---

UENUM()
enum class EScalePlane : int32
{
    // The plane won't scale
    kNo                 UMETA( DisplayName = "No Scale" ),
    // The plane will scale relatively to its original size
    // If the plane is already 100% camera FOV, it will act as the option "100% Camera"
    kRelativeScale      UMETA( DisplayName = "Relative Scale" ),
    // The plane will auto-scale to match the 100% camera FOV
    kFitToCamera        UMETA( DisplayName = "Scale 100% Camera" ),
};

class EPOSTRACKSEDITOR_API ShotSequenceTools
{
private:
    friend class BoardSequenceTools;
    friend class ToolkitHelpers;

// Inside EposSequenceTools_Board
public:
    /**
    *  Clone the content of the section
    *
    * @param ISequencer             iSequencer to clone the section.
    * @param UMovieSceneSubSection* iSection   the section to clone.
    */
    static void CloneInnerContent( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, bool iEmptyDrawings );

private:
    static void CloneInnerPlane( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMovieScene* iMovieScene, bool iEmptyDrawings, APlaneActor* iPlaneToClone, FGuid iPlaneBinding, ACineCameraActor* iClonedCamera, bool iAttachPlaneToCamera );

// Inside EposSequenceTools
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

public:
    static void RenameBinding( ISequencer* iSequencer, FGuid iBinding, FString iNewLabel );

private:
    static void RenameBinding( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iBinding, FString iNewLabel );

// Inside EposSequenceTools_Camera
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
    static void CreateCamera( ISequencer* iSequencer, const FCameraArgs& iCameraArgs = FCameraArgs(), const FPlaneArgs& iPlaneArgs = FPlaneArgs() );

    static bool CanCreateCamera( ISequencer* iSequencer );

    /**
    *  Update the camera location from the viewport
    *
    * @param ISequencer iSequencer to update camera.
    */
    static void SnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool CanSnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static void DeleteCameraKey( ISequencer* iSequencer, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles );

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
    static void CreateCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FCameraArgs& iCameraArgs, const FPlaneArgs& iPlaneArgs );

    static ACineCameraActor* SpawnCamera( UWorld* iWorld, const FTransform& iTransform );
    static ACineCameraActor* SpawnAndBindCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, const FCameraArgs& iCameraArgs, const FPlaneArgs& iPlaneArgs, FGuid* oGuid );
    static void CameraAdded( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid CameraGuid, ACineCameraActor* iCamera, FFrameNumber FrameNumber, const FPlaneArgs& iPlaneArgs );
    static void CreateCameraCut( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, FFrameNumber iFrameNumber );

    static bool SnapCameraToViewport( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, ACineCameraActor* ioCamera, FGuid iCameraGuid, FFrameNumber iFrameNumber, const FTransform& iNewTransform, EMovieSceneKeyInterpolation iInterpolation );
    static void SnapCameraToViewport( ISequencer& iSequencer, UMovieSceneSequence* iSequence, ACineCameraActor* ioCamera, FGuid iCameraGuid, FFrameNumber iFrameNumber );
    static void DeleteCameraKey( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles );
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

public:
    static bool SetCameraFocalLengthAndScalePlane( TArray<TWeakObjectPtr<APlaneActor>> ioPlanes, ACineCameraActor* ioCamera, float iNewFocalLength, EScalePlane iScaleType );

// Inside EposSequenceTools_Plane
public:
    /**
    *  Add a Camera track
    *
    * @param ISequencer iSequencer to add a plane.
    */
    static void CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber, const FPlaneArgs& iPlaneArgs = FPlaneArgs() );

    static bool CanCreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static int32 GetAllPlanes( ISequencer* iSequencer, TArray<APlaneActor*>* oPlanes = nullptr, TArray<FGuid>* oPlaneBindings = nullptr );

    static void DetachPlane( ISequencer* iSequencer, TArray<FGuid> iPlaneBindings );
    static void DetachPlane( ISequencer* iSequencer, FGuid iPlaneBinding );

    static bool CanDetachPlane( ISequencer* iSequencer, FGuid iPlaneBinding );

    static int32 GetAttachedPlanes( ISequencer* iSequencer, TArray<APlaneActor*>* oPlanes = nullptr, TArray<FGuid>* oPlaneBindings = nullptr );

    static bool IsPlaneVisible( ISequencer* iSequencer, FGuid iPlaneBinding );
    static void TogglePlaneVisibility( ISequencer* iSequencer, TArray<FGuid> iPlaneBindings, TOptional<FGuid> iPlaneReference = TOptional<FGuid>() );
    static void TogglePlaneVisibility( ISequencer* iSequencer, FGuid iPlaneBinding );

    static void DeletePlane( ISequencer* iSequencer, TArray<FGuid> iPlaneBindings );
    static void DeletePlane( ISequencer* iSequencer, FGuid iPlaneBinding );

    static bool MoveAndScalePlane( APlaneActor* ioPlane, const ACineCameraActor* iCamera, float iNewDistance, EScalePlane iScaleType );
    static bool CanMoveAndScalePlane( const APlaneActor* iPlane, const ACineCameraActor* iCamera );

private:
    static void CreatePlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, const FPlaneArgs& iPlaneArgs );
    static void DetachPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iPlaneBinding );
    static bool CanDetachPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iPlaneBindings );
    static bool IsPlaneVisible( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding );
    static void TogglePlaneVisibility( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iPlaneBindings, TOptional<FGuid> iPlaneReference = TOptional<FGuid>() );
    static void DeletePlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iPlaneBindings );
    static void SelectSinglePlane( ISequencer& iSequencer, UMovieSceneSubSection* iSubSection, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding );
    static void SelectMultiPlane( ISequencer& iSequencer, UMovieSceneSubSection* iSubSection, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding );

    static APlaneActor* SpawnPlane( UWorld* iWorld, ACineCameraActor* iCamera );
    static void SpawnAndBindPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, ACineCameraActor* iCamera, FFrameNumber iFrameNumber, const FPlaneArgs& iPlaneArgs );

// Inside EposSequenceTools_Drawing
public:
    /**
    *  Add a drawing (material/texture)
    *
    * @param ISequencer iSequencer to add a drawing.
    */
    static void CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings );
    static void CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static bool CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static void CloneDrawing( ISequencer* iSequencer, UMovieSceneSection* iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle, FFrameNumber iFrameNumber );

    static bool CanCloneDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static void DeleteDrawing( ISequencer* iSequencer, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles );

private:
    static void CreateDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings );
    // FDrawing is used as parameter but only because the function is private, otherwise it should be UMovieSceneSection/FMovieSceneChannelHandle/FKeyHandle
    static bool IsDrawingInEditionMode( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FDrawing& iDrawing );
    static void CloneDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMovieSceneSection* iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle, FFrameNumber iFrameNumber );
    static void DeleteDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles );

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

// Inside EposSequenceTools_Opacity
public:
    static bool CanCreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static void CreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings, float iOpacity );
    static void CreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding, float iOpacity );

    static void DeleteOpacity( ISequencer* iSequencer, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles );

    static void SetOpacity( ISequencer* iSequencer, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles, float iOpacity );

private:
    static void CreateOpacity( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings, float iOpacity );
    static void DeleteOpacity( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles );
    static void SetOpacity( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles, float iOpacity );

// Inside EposSequenceTools_Note
public:
    static void CreateNote( ISequencer* iSequencer, FFrameNumber iFrameNumber );
    static void DeleteNote( ISequencer* iSequencer, TWeakObjectPtr<UMovieSceneSection> iNoteSection );

    static TArray<TWeakObjectPtr<UMovieSceneNoteSection>> GetAllNotes( ISequencer* iSequencer );

private:
    static void CreateNote( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );
    static void DeleteNote( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TWeakObjectPtr<UMovieSceneSection> iNoteSection );

    static TArray<TWeakObjectPtr<UMovieSceneNoteSection>> GetAllNotes( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
};
