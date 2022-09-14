// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

//#include "LevelSequenceEditorBlueprintLibrary.h"
#include "Misc/QualifiedFrameTime.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MovieSceneBindingProxy.h"
#include "MovieSceneSequencePlayer.h"
#include "MovieSceneObjectBindingID.h"

#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"

#include "EposSequenceEditorBlueprintLibrary.generated.h"

class ISequencer;
class UBoardSequence;
class UEposMovieSceneSequence;
class UMovieSceneFolder;
class UMovieSceneSection;
class UMovieSceneSubSection;
class UMovieSceneTrack;
class UShotSequence;

//USTRUCT(BlueprintType)
//struct FSequencerChannelProxy
//{
//    GENERATED_BODY()
//
//    FSequencerChannelProxy()
//        : Section(nullptr)
//    {}
//
//    FSequencerChannelProxy(const FName& InChannelName, UMovieSceneSection* InSection)
//        : ChannelName(InChannelName)
//        , Section(InSection)
//    {}
//
//    UPROPERTY(BlueprintReadWrite, Category=Channel)
//    FName ChannelName;
//
//    UPROPERTY(BlueprintReadWrite, Category=Channel)
//    TObjectPtr<UMovieSceneSection> Section;
//};

UCLASS()
class EPOSSEQUENCEEDITOR_API UBoardSequenceEditorBlueprintLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    /*
     * Get the currently opened root/master board sequence asset
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor|Board")
    static UBoardSequence* GetRootBoardSequence();

    /*
     * Get the currently focused/viewed board sequence asset if there is a hierarchy of sequences.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor|Board")
    static UBoardSequence* GetFocusedBoardSequence();

public:

    /*
     * Insert a new board sequence at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static UMovieSceneSubSection* InsertBoardSequence( int32 StartFrame, int32 EndFrame );

    /*
     * Insert a new shot sequence at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static UMovieSceneSubSection* InsertShotSequence( int32 StartFrame, int32 EndFrame );

    /*
     * Clone a subsection with its sequence
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static UMovieSceneSubSection* CloneSection( UMovieSceneSubSection* SubSection, int32 FrameNumber );

public:

    /*
     * Create a camera in a subsection at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static void CreateCamera( UMovieSceneSubSection* SubSection );

public:

    /*
     * Create a plane in a subsection at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static void CreatePlane( UMovieSceneSubSection* SubSection );

    /**
     * Activate the lighttable on the plane
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static void ActivateLighttable( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Deactivate the lighttable on the plane
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static void DeactivateLighttable( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Get the state of the lighttable for the plane
     * It returns:
     *   1: lighttable on all drawings is on
     *   0: lighttable on all drawings is off
     *   -1: undetermined (mix of on and off, not enough drawings)
     * @return 1 | 0 | -1 -> on | off | undetermined
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static int32 GetLighttableState( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

public:

    /*
     * Create a drawing in a plane at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static void CreateDrawing( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding, int32 Frame );

public:

    /*
     * Rename a plane/camera binding and its actor
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static void RenameBinding( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding, FString NewLabel );

public:

    /*
     * Create a new take in the subsection
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static FBoardSectionTake CreateTake( UMovieSceneSubSection* SubSection );

    /*
     * Switch take to another in the subsection
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static FBoardSectionTake SwitchTake( UMovieSceneSubSection* SubSection, FBoardSectionTake Take );

    /*
     * Get the list of takes in the subsection
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Board" )
    static TArray<FBoardSectionTake> GetTakes( UMovieSceneSubSection* SubSection );

    /*
     * Get the current take in the subsection
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Board" )
    static FBoardSectionTake GetCurrentTake( UMovieSceneSubSection* SubSection );

    /*
     * Check the validity of a take
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Board" )
    static bool IsValid( FBoardSectionTake Take );

    /*
     * Get the name of a take
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Board" )
    static FText GetDisplayName( FBoardSectionTake Take );

    /*
     * Compare 2 takes
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Board", meta = ( DisplayName = "Equal (BoardSectionTake)", CompactNodeTitle = "==" ) )
    static bool EqualEqual_BoardSectionTakeBoardSectionTake( FBoardSectionTake A, FBoardSectionTake B );
};

//---

UCLASS()
class EPOSSEQUENCEEDITOR_API UShotSequenceEditorBlueprintLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    /*
     * Get the currently opened root/master shot sequence asset
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor|Shot")
    static UShotSequence* GetRootShotSequence();

    /*
     * Get the currently focused/viewed shot sequence asset if there is a hierarchy of sequences.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor|Shot")
    static UShotSequence* GetFocusedShotSequence();

    /*
     * Step to next sibling shot.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor|Shot")
    static void StepToNextShot();

    /*
     * Step to previous sibling shot.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor|Shot")
    static void StepToPreviousShot();

public:

    /*
     * Create a camera in a shot at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Shot" )
    static void CreateCamera();

public:

    /*
     * Create a plane in a shot at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Shot" )
    static void CreatePlane();

    /**
     * Activate the lighttable on the plane
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Shot" )
    static void ActivateLighttable( const FMovieSceneBindingProxy& Binding );

    /**
     * Deactivate the lighttable on the plane
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Shot" )
    static void DeactivateLighttable( const FMovieSceneBindingProxy& Binding );

    /**
     * Get the state of the lighttable for the plane
     * It returns:
     *   1: lighttable on all drawings is on
     *   0: lighttable on all drawings is off
     *   -1: undetermined (mix of on and off, not enough drawings)
     * @return 1 | 0 | -1 -> on | off | undetermined
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Shot" )
    static int32 GetLighttableState( const FMovieSceneBindingProxy& Binding );

public:

    /*
     * Create a drawing in a plane at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Shot" )
    static void CreateDrawing( const FMovieSceneBindingProxy& Binding, int32 Frame );

public:

    /*
     * Rename a plane/camera binding and its actor
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Shot" )
    static void RenameBinding( const FMovieSceneBindingProxy& Binding, FString NewLabel );

};

//---

UCLASS()
class EPOSSEQUENCEEDITOR_API UEposSequenceEditorBlueprintLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    /*
     * Open a board sequence asset
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static bool OpenEposSequence( UEposMovieSceneSequence* EposSequence );

    /*
     * Get the currently opened root/master epos sequence asset
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static UEposMovieSceneSequence* GetRootEposSequence();

    /*
     * Get the currently focused/viewed epos sequence asset if there is a hierarchy of sequences.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static UEposMovieSceneSequence* GetFocusedEposSequence();

    /*
     * Focus/view the sequence associated to the given sub sequence section.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void FocusEposSequence(UMovieSceneSubSection* SubSection);

    /*
     * Focus/view the parent sequence, popping out of the current sub sequence section.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void FocusParentSequence();

    /*
     * Get the current sub section hierarchy from the current sequence to the section associated with the focused sequence.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static TArray<UMovieSceneSubSection*> GetSubSequenceHierarchy();

    /*
     * Close
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void CloseEposSequence();

public:

    /*
     * Move and scale a plane from the camera
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor" )
    static void MoveAndScalePlane( APlaneActor* Plane, const ACineCameraActor* Camera, float NewDistance, EScalePlane ScaleType );

    /*
     * Move and scale a plane from the camera
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor" )
    static void SetCameraFocalLengthAndScalePlane( TArray<APlaneActor*> Planes, ACineCameraActor* Camera, float NewFocalLength, EScalePlane ScaleType );

public:

    /**
     * Play the current level sequence
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void Play();

    /**
     * Pause the current level sequence
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void Pause();

public:

    /**
     * Set global playback position for the current level sequence in frames
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SetCurrentTime(int32 NewFrame);

    /**
     * Get the current global playback position in frames
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static int32 GetCurrentTime();

    /**
     * Set local playback position for the current level sequence in frames
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SetCurrentLocalTime(int32 NewFrame);

    /**
     * Get the current local playback position in frames
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static int32 GetCurrentLocalTime();

    /**
     * Play from the current time to the requested time in frames
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void PlayTo(FMovieSceneSequencePlaybackParams PlaybackParams);

public:

    /** Check whether the sequence is actively playing. */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static bool IsPlaying();

public:

    /** Gets the currently selected tracks. */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static TArray<UMovieSceneTrack*> GetSelectedTracks();

    /** Gets the currently selected sections. */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static TArray<UMovieSceneSection*> GetSelectedSections();

    ///** Gets the currently selected channels. */
    //UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    //static TArray<FSequencerChannelProxy> GetSelectedChannels();

    /** Gets the currently selected folders. */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static TArray<UMovieSceneFolder*> GetSelectedFolders();

    /** Gets the currently selected object bindings */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static TArray<FMovieSceneBindingProxy> GetSelectedBindings();

    /** Select tracks */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SelectTracks(const TArray<UMovieSceneTrack*>& Tracks);

    /** Select sections */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SelectSections(const TArray<UMovieSceneSection*>& Sections);

    ///** Select channels */
    //UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    //static void SelectChannels(const TArray<FSequencerChannelProxy>& Channels);

    /** Select folders */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SelectFolders(const TArray<UMovieSceneFolder*>& Folders);

    /** Select bindings */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SelectBindings(const TArray<FMovieSceneBindingProxy>& ObjectBindings);

    /** Empties the current selection. */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void EmptySelection();

    /** Set the selection range start frame. */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SetSelectionRangeStart(int32 NewFrame);

    /** Set the selection range end frame. */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SetSelectionRangeEnd(int32 NewFrame);

    /** Get the selection range start frame. */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static int32 GetSelectionRangeStart();

    /** Get the selection range end frame. */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static int32 GetSelectionRangeEnd();

public:

    /** Refresh Sequencer UI. */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void RefreshCurrentEposSequence();

    /** Get the object bound to the given binding ID with the current Epos Sequence Editor */
    UFUNCTION(BlueprintPure, Category="Epos Sequence Editor")
    static TArray<UObject*> GetBoundObjects(FMovieSceneObjectBindingID ObjectBinding);

    /** Check whether the current level sequence and its descendants are locked for editing. */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static bool IsEposSequenceLocked();

    /** Sets the lock for the current level sequence and its descendants for editing. */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SetLockEposSequence(bool bLock);

public:

    /** Check whether the lock for the viewport to the camera cuts is enabled. */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static bool IsCameraCutLockedToViewport();

    /** Sets the lock for the viewport to the camera cuts. */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SetLockCameraCutToViewport(bool bLock);

public:

    /** Gets whether the specified track filter is on/off */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static bool IsTrackFilterEnabled(const FText& TrackFilterName);

    /** Sets the specified track filter to be on or off */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SetTrackFilterEnabled(const FText& TrackFilterName, bool bEnabled);

    /** Gets all the available track filter names */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static TArray<FText> GetTrackFilterNames();

public:

    /** Get if a custom color for specified channel idendified by it's class and identifier exists */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static bool HasCustomColorForChannel(UClass* Class, const FString& Identifier);

    /** Get custom color for specified channel idendified by it's class and identifier,if none exists will return white*/
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static FLinearColor GetCustomColorForChannel(UClass* Class, const FString& Identifier);

    /** Set Custom Color for specified channel idendified by it's class and identifier. This will be stored in editor user preferences.*/
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SetCustomColorForChannel(UClass* Class, const FString& Identifier, const FLinearColor& NewColor);

    /** Set Custom Color for specified channels idendified by it's class and identifiers. This will be stored in editor user preferences.*/
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SetCustomColorForChannels(UClass* Class, const TArray<FString>& Identifiers, const TArray<FLinearColor>& NewColors);

    /** Set Random Colors for specified channels idendified by it's class and identifiers. This will be stored in editor user preferences.*/
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SetRandomColorForChannels(UClass* Class, const TArray<FString>& Identifiers);

    /** Delete for specified channel idendified by it's class and identifier.*/
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void DeleteColorForChannels(UClass* Class, FString& Identifier);

public:

    /*
     * Callbacks
     */

public:

    /**
     * Internal function to assign a sequencer singleton.
     * NOTE: Only to be called by EposSequenceEditor::Construct.
     */
    static void SetSequencer(TSharedRef<ISequencer> InSequencer);
};
