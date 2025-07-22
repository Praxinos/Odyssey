// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Curves/KeyHandle.h"
#include "Misc/Guid.h"
#include "KeyParams.h"
#include "Misc/FrameNumber.h"
#include "MovieSceneSequenceID.h"
#include "TransformData.h"

#include "EposSequenceTools.generated.h"

class AActor;
class ACineCameraActor;
class AOdysseyAnimationActor;
class UBoardSequence;
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
class UOdysseyAnimation;
class UShotSequence;
class UTexture2D;
class IMovieScenePlayer;
class ISequencer;
struct FBoardSectionTake;
struct FMovieSceneChannelHandle;

void EjectAnyActor();

struct FCameraArgs
{
    FString mName;
};

struct FAnimationArgs
{
    FString mName;
    TOptional<float> mMargin;
    TWeakObjectPtr<UOdysseyAnimation> mAnimation;
};

struct FAnimationCutArgs
{
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
    static UMovieSceneSubSection* CloneSection( ISequencer* iSequencer, UMovieSceneCinematicBoardSection* iSection, FFrameNumber iFrameNumber, bool iEmptyDrawings );

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

    /** Create a new board sequence. */
    static UBoardSequence* CreateBoard( const FString& iNewBoardPath, const FString& iNewBoardName );

// Inside EposSequenceTools_Take
public:
    /** Create a new take (from the current subsequence) for the board section. */
    static FBoardSectionTake* CreateTake( ISequencer* iSequencer, UMovieSceneSubSection& iSubSection );

    /** Switch the current take to the new one for the board section. */
    static FBoardSectionTake* SwitchTake( ISequencer* iSequencer, UMovieSceneSubSection& iSubSection, const FBoardSectionTake* iTake );

// Inside EposSequenceTools
public:
    /**
    *  Guess to actor to auto select from the currently selected actors and the actor history selection
    *
    * @param ISequencer*                iSequencer to get the auto-select actor.
    * @param iFrameNumber               iFrameNumber to get the auto-select actor.
    * @return AActor* the best actor to auto-select.
    */
    static AActor* GuessActorToSelect( ISequencer* iSequencer, const FFrameNumber& iFrameNumber );

private:
    static AActor* GuessActorToSelect( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceId, const FFrameNumber& iFrameNumber, const TArray<AActor*>& iLastSelectedActors );

public:
    /**  Add an actor to an history list, to be able to know later which actor best fit during auto-selection actor */
    static void AddSelectedActorToHistory( AActor* iActor );
protected:
    static TArray<AActor*> mDirectActorsSelectedHistory;

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
    static void CreateCameraWithAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, const FCameraArgs& iCameraArgs = FCameraArgs(), const FAnimationArgs& iAnimationArgs = FAnimationArgs() );

    /**
    *  Create a new camera (actor & track & cameracut track) in the board section
    *
    * @param ISequencer             iSequencer to add a new camera.
    * @param UMovieSceneSubSection  iSubSection to add a new camera.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static void CreateCameraWithAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, const FCameraArgs& iCameraArgs = FCameraArgs(), const FAnimationArgs& iAnimationArgs = FAnimationArgs() );

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
    * @param ISequencer     iSequencer to find the previous camera position.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static void GotoPreviousCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Is there a previous camera position in the board section ?
    *
    * @param ISequencer     iSequencer to find the previous camera position.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static bool HasPreviousCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Go to the next camera position in the board section
    *
    * @param ISequencer     iSequencer to find the next camera position.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static void GotoNextCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Is there a next camera position in the board section ?
    *
    * @param ISequencer     iSequencer to find the next camera position.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static bool HasNextCameraPosition( ISequencer* iSequencer, FFrameNumber iFrameNumber );

// Inside EposSequenceTools_Animation
public:
    /**
    *  Create a new animation (actor & track) in the board section
    *
    * @param ISequencer     iSequencer to add a new animation.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static void CreateAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, const FAnimationArgs& iAnimationArgs = FAnimationArgs() );

    /**
    *  Create a new animation (actor & track) in the board section
    *
    * @param ISequencer             iSequencer to add a new animation.
    * @param UMovieSceneSubSection  iSubSection to add a new animation.
    * @param FFrameNumber           iFrameNumber to get the board section.
    */
    static void CreateAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, const FAnimationArgs& iAnimationArgs = FAnimationArgs() );

    /**
    *  Can a animation be created in the board section ?
    *
    * @param ISequencer         iSequencer to get the camera.
    * @param FFrameNumber       iFrameNumber to get the board section.
    * @return bool
    */
    static bool CanCreateAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Get all animations (actor & track bindings) in the board section
    *
    * @param ISequencer     iSequencer to get animations.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param TArray<AOdysseyAnimationActor*>*   oAnimations to get all animation actors.
    * @param TArray<FGuid>*                     oAnimationBindings to get all animation bindings.
    */
    static int32 GetAllAnimations( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<AOdysseyAnimationActor*>* oAnimations = nullptr, TArray<FGuid>* oAnimationBindings = nullptr );

    /**
    *  Detach a animation of the camera in the board section
    *
    * @param ISequencer     iSequencer to detach a animation.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iAnimationBinding to detach.
    */
    static void DetachAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings );
    static void DetachAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding );

    /**
    *  Detach a animation of the camera in the board section
    *
    * @param ISequencer             iSequencer to detach a animation.
    * @param UMovieSceneSubSection  iSubSection to detach a animation.
    * @param FGuid                  iAnimationBinding to detach.
    */
    static void DetachAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iAnimationBindings );
    static void DetachAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding );

    /**
    *  Detach a animation of the camera in the board section
    *
    * @param ISequencer             iSequencer to detach a animation.
    * @param UMovieSceneSubSection  iSubSection to detach a animation.
    * @param FGuid                  iAnimationBinding to detach.
    */
    static bool CanDetachAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iAnimationBindings );
    static bool CanDetachAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding );

    /**
    *  Get all animations (actor & track bindings) attached to the camera in the board section
    *
    * @param ISequencer     iSequencer to get animations.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param TArray<AOdysseyAnimationActor*>*   oAnimations to get all animation actors.
    * @param TArray<FGuid>*                     oAnimationBindings to get all animation bindings.
    */
    static int32 GetAttachedAnimations( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<AOdysseyAnimationActor*>* oAnimations = nullptr, TArray<FGuid>* oAnimationBindings = nullptr );

    /**
    *  Get all animations (actor & track bindings) attached to the camera in the board section
    *
    * @param ISequencer     iSequencer to get animations.
    * @param UMovieSceneSubSection  iSubSection to check all animations.
    */
    static bool IsAnimationInEditionMode( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection );

    /**
    *  Get a animation visiblity of the camera in the board section
    *
    * @param ISequencer     iSequencer to detach a animation.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iAnimationBinding to get visibility.
    */
    static bool IsAnimationVisible( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding );

    /**
    *  Get a animation visiblity of the camera in the board section
    *
    * @param ISequencer             iSequencer to detach a animation.
    * @param UMovieSceneSubSection  iSubSection to detach a animation.
    * @param FGuid                  iAnimationBinding to get visibility.
    */
    static bool IsAnimationVisible( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding );

    /**
    *  Toggle a animation visiblity of the camera in the board section
    *
    * @param ISequencer     iSequencer to detach a animation.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iAnimationBinding to toggle visibility.
    */
    static void ToggleAnimationVisibility( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding );

    /**
    *  Toggle a animation visiblity of the camera in the board section
    *
    * @param ISequencer             iSequencer to detach a animation.
    * @param UMovieSceneSubSection  iSubSection to detach a animation.
    * @param FGuid                  iAnimationBinding to toggle visibility.
    */
    static void ToggleAnimationVisibility( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iAnimationBindings, TOptional<FGuid> iAnimationReference = TOptional<FGuid>() );
    static void ToggleAnimationVisibility( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding );

    /**
    *  Delete a animation (with its actor) of the camera in the board section
    *
    * @param ISequencer     iSequencer to delete a animation.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iAnimationBinding to detach.
    */
    static void DeleteAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings );
    static void DeleteAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding );

    /**
    *  Delete a animation (with its actor) of the camera in the board section
    *
    * @param ISequencer             iSequencer to delete a animation.
    * @param UMovieSceneSubSection  iSubSection to delete a animation.
    * @param FGuid                  iAnimationBinding to delete.
    */
    static void DeleteAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iAnimationBindings );
    static void DeleteAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding );

    /**
    *  Select a animation in the board section (and unselect all other animations)
    *
    * @param ISequencer             iSequencer to select a animation.
    * @param UMovieSceneSubSection  iSubSection to select a animation.
    * @param FGuid                  iAnimationBinding to select.
    */
    static void SelectSingleAnimation( ISequencer* iSequencer, UMovieSceneSubSection* iSubSection, FGuid iAnimationBinding );

    /**
    *  Select a animation in the board section (and keep already selected all other animations)
    *
    * @param ISequencer             iSequencer to select a animation.
    * @param UMovieSceneSubSection  iSubSection to select a animation.
    * @param FGuid                  iAnimationBinding to select.
    */
    static void SelectMultiAnimation( ISequencer* iSequencer, UMovieSceneSubSection* iSubSection, FGuid iAnimationBinding );

// Inside EposSequenceTools_AnimationCut
public:
    /**
    *  Create a new animation cut (material & texture) in an animation in the board section
    *
    * @param ISequencer     iSequencer to add a new animation cut.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iAnimationBinding to get the animation track.
    */
    static void CreateAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings, const FAnimationCutArgs& iAnimationCutArgs = FAnimationCutArgs() );
    static void CreateAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding, const FAnimationCutArgs& iAnimationCutArgs = FAnimationCutArgs() );

    /**
    *  Create a new animation cut (material & texture) in an animation in the board section
    *
    * @param ISequencer     iSequencer to add a new animation cut.
    * @param UMovieSceneSubSection  iSubSection to get the animation.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iAnimationBinding to get the animation track.
    */
    static void CreateAnimationCut( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings, const FAnimationCutArgs& iAnimationCutArgs = FAnimationCutArgs() );
    static void CreateAnimationCut( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iAnimationBinding, const FAnimationCutArgs& iAnimationCutArgs = FAnimationCutArgs() );

    /**
    *  Can a animation cut be created in the board section ?
    *
    * @param ISequencer     iSequencer to get the animation.
    * @param FFrameNumber   iFrameNumber to get the board section.
    * @param FGuid          iAnimationBinding to get the animation track.
    * @return bool
    */
    static bool CanCreateAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding );

    /**
    *  Can a animation cut be created in the board section ?
    *
    * @param ISequencer             iSequencer to get the animation.
    * @param UMovieSceneSubSection  iSubSection to get the animation.
    * @param FFrameNumber           iFrameNumber to get the board section.
    * @param FGuid                  iAnimationBinding to get the animation track.
    * @return bool
    */
    static bool CanCreateAnimationCut( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings );
    static bool CanCreateAnimationCut( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iAnimationBinding );

public:
    /**
    *  Go to the previous animation cut in the board section
    *
    * @param ISequencer     iSequencer to find the previous animation cut.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static void GotoPreviousAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Is there a previous animation cut in the board section ?
    *
    * @param ISequencer     iSequencer to find the previous animation cut.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static bool HasPreviousAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Go to the next animation cut in the board section
    *
    * @param ISequencer     iSequencer to find the next animation cut.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static void GotoNextAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Is there a next animation cut in the board section ?
    *
    * @param ISequencer     iSequencer to find the next animation cut.
    * @param FFrameNumber   iFrameNumber to get the board section.
    */
    static bool HasNextAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber );

// Inside EposSequenceTools_Opacity
public:
    static bool CanCreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iActorBinding );
    static bool CanCreateOpacity( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, TArray<FGuid> iActorBindings );
    static bool CanCreateOpacity( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iActorBinding );

    static void CreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iActorBindings, float iOpacity );
    static void CreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iActorBinding, float iOpacity );
    static void CreateOpacity( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, TArray<FGuid> iActorBindings, float iOpacity );
    static void CreateOpacity( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iActorBinding, float iOpacity );

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


UENUM( BlueprintType )
enum class EScaleActor : uint8
{
    // The animation won't scale
    kNo                 UMETA( DisplayName = "No Scale" ),
    // The animation will scale relatively to its original size
    // If the animation is already 100% camera FOV, it will act as the option "100% Camera"
    kRelativeScale      UMETA( DisplayName = "Relative Scale" ),
    // The animation will auto-scale to match the 100% camera FOV
    kFitToCamera        UMETA( DisplayName = "Scale 100% Camera" ),
};

class EPOSTRACKSEDITOR_API ShotSequenceTools
{
private:
    friend class BoardSequenceTools;
    friend class CinematicBoardTrackTools;
    friend class ToolkitHelpers;

// Inside EposSequenceTools_Board
private:
    /**
    *  Clone the content of the section
    *
    * @param ISequencer             iSequencer to clone the section.
    * @param UMovieSceneSubSection* iSection   the section to clone.
    */
    static void CloneInnerContent( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, bool iEmptyDrawings );

public:
    static void StepToNextShot( ISequencer* iSequencer );
    static void StepToPreviousShot( ISequencer* iSequencer );

private:
    static void StepToNextShot( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static void StepToPreviousShot( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );

    static UMovieSceneSubSection* FindNextOrPreviousShot( UMovieSceneSequence* iSequence, FFrameNumber iSearchFromTime, bool iNextShot );

// Inside EposSequenceTools
private:
    class EPOSTRACKSEDITOR_API cTemporarySwitchInner
    {
    public:
        cTemporarySwitchInner( ISequencer& iSequencer, FMovieSceneSequenceIDRef iInnerID );
        ~cTemporarySwitchInner();
    private:
        ISequencer& mSequencer;
        FMovieSceneSequenceID mOriginalId;
        FFrameTime mOriginalGlobalTime;
    };

    friend class FImportImageSequenceConverter;

public:
    static void RenameBinding( ISequencer* iSequencer, FGuid iBinding, FString iNewLabel );

private:
    static void RenameBinding( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iBinding, FString iNewLabel );

public:
    static void SortBindings( TArray<AOdysseyAnimationActor*> iAnimationActors, TArray<FGuid> iBindings, UMovieScene* iMovieScene, TArray<AOdysseyAnimationActor*>* oOrderedAnimationActors, TArray<FGuid>* oOrderedBindings );

public:
    static bool MoveAndScaleActor( AActor* ioActor, const ACineCameraActor* iCamera, float iNewDistance, EScaleActor iScaleType );
    static bool CanMoveAndScaleActor( const AActor* iActor, const ACineCameraActor* iCamera );

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
    static void CreateCameraWithAnimation( ISequencer* iSequencer, const FCameraArgs& iCameraArgs = FCameraArgs(), const FAnimationArgs& iAnimationArgs = FAnimationArgs() );

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
    static void CreateCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FCameraArgs& iCameraArgs, const FAnimationArgs* iAnimationArgs );

    static ACineCameraActor* SpawnCamera( UWorld* iWorld, const FTransform& iTransform );
    static ACineCameraActor* SpawnAndBindCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FCameraArgs& iCameraArgs, FGuid* oGuid );
    static TArray<AActor*> CameraAdded( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid CameraGuid, ACineCameraActor* iCamera, FFrameNumber FrameNumber, const FAnimationArgs* iAnimationArgs );
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
    static bool SetCameraFocalLengthAndScaleActor( TArray<TWeakObjectPtr<AActor>> ioActors, ACineCameraActor* ioCamera, float iNewFocalLength, EScaleActor iScaleType );

// Inside EposSequenceTools_Animation
public:
    static void CreateAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, const FAnimationArgs& iAnimationArgs = FAnimationArgs() );

    static bool CanCreateAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static int32 GetAllAnimations( ISequencer* iSequencer, TArray<AOdysseyAnimationActor*>* oAnimations = nullptr, TArray<FGuid>* oAnimationBindings = nullptr );

    static void DetachAnimation( ISequencer* iSequencer, TArray<FGuid> iAnimationBindings );
    static void DetachAnimation( ISequencer* iSequencer, FGuid iAnimationBinding );

    static bool CanDetachAnimation( ISequencer* iSequencer, FGuid iAnimationBinding );

    static int32 GetAttachedAnimations( ISequencer* iSequencer, TArray<AOdysseyAnimationActor*>* oAnimations = nullptr, TArray<FGuid>* oAnimationBindings = nullptr );

    static bool IsAnimationInEditionMode( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UOdysseyAnimation* iAnimation );

    static bool IsAnimationVisible( ISequencer* iSequencer, FGuid iAnimationBinding );
    static void ToggleAnimationVisibility( ISequencer* iSequencer, TArray<FGuid> iAnimationBindings, TOptional<FGuid> iAnimationReference = TOptional<FGuid>() );
    static void ToggleAnimationVisibility( ISequencer* iSequencer, FGuid iAnimationBinding );

    static void DeleteAnimation( ISequencer* iSequencer, TArray<FGuid> iAnimationBindings );
    static void DeleteAnimation( ISequencer* iSequencer, FGuid iAnimationBinding );

private:
    static void CreateAnimation( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, const FAnimationArgs& iAnimationArgs );
    static void DetachAnimation( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iAnimationBinding );
    static bool CanDetachAnimation( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iAnimationBindings );
    static bool IsAnimationVisible( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding );
    static void ToggleAnimationVisibility( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iAnimationBindings, TOptional<FGuid> iAnimationReference = TOptional<FGuid>() );
    static void DeleteAnimation( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iAnimationBindings );
    static void SelectSingleAnimation( ISequencer& iSequencer, UMovieSceneSubSection* iSubSection, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding );
    static void SelectMultiAnimation( ISequencer& iSequencer, UMovieSceneSubSection* iSubSection, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding );

    static AOdysseyAnimationActor* SpawnAnimation( UWorld* iWorld, ACineCameraActor* iCamera, float iFocusDistance, float iSafeMargin, FVector2D iRelativeScaling );
    static AOdysseyAnimationActor* SpawnAndBindAnimation( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iCameraGuid, ACineCameraActor* iCamera, FFrameNumber iFrameNumber, const FAnimationArgs& iAnimationArgs, FGuid* oGuid );

// Inside EposSequenceTools_AnimationCut
public:
    /**
    *  Add a animation cut
    *
    * @param ISequencer iSequencer to add a animation cut.
    */
    static void CreateAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings, const FAnimationCutArgs& iAnimationCutArgs = FAnimationCutArgs() );
    static void CreateAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding, const FAnimationCutArgs& iAnimationCutArgs = FAnimationCutArgs() );

    static bool CanCreateAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding );

private:
    static void CreateAnimationCut( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings, const FAnimationCutArgs& iAnimationCutArgs );

public:
    /**
    *  Go to the previous animation cut
    *
    * @param ISequencer iSequencer to add a animation cut.
    */
    static void GotoPreviousAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool HasPreviousAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /**
    *  Go to the next animation cut
    *
    * @param ISequencer iSequencer to add a animation cut.
    */
    static void GotoNextAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    static bool HasNextAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber );

private:
    static void GotoPreviousAnimationCut( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );
    static void GotoNextAnimationCut( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );

// Inside EposSequenceTools_Opacity
public:
    static bool CanCreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iActorBinding );

    static void CreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iActorBindings, float iOpacity );
    static void CreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iActorBinding, float iOpacity );

    static void DeleteOpacity( ISequencer* iSequencer, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles );

    static void SetOpacity( ISequencer* iSequencer, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles, float iOpacity );

private:
    static void CreateOpacity( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, TArray<FGuid> iActorBindings, float iOpacity );
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
