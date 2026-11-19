// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

//#include "LevelSequenceEditorBlueprintLibrary.h"
#include "Misc/QualifiedFrameTime.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MovieSceneBindingProxy.h"
#include "MovieSceneSequencePlayer.h"
#include "MovieSceneTimeUnit.h"
#include "MovieSceneObjectBindingID.h"
#include "SequencerSettings.h"
#include "SequencerCurveEditorObject.h"

#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "Export/ImageSequence/ExportImageSequenceSettings.h"
#include "Export/PDF/ExportPDFSettings.h"
#include "Tools/EposSequenceTools.h"
#include "Import/ImportImageSequenceSettings.h"

#include "EposSequenceEditorBlueprintLibrary.generated.h"

class ISequencer;
class UBoardSequence;
class UEposMovieSceneSequence;
class UEposSequenceEditorSettings;
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

    /**
     * Get the currently opened root/master board sequence asset
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board")
    static UBoardSequence* GetRootBoardSequence();

    /**
     * Get the currently focused/viewed board sequence asset if there is a hierarchy of sequences.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board")
    static UBoardSequence* GetFocusedBoardSequence();

public:

    /**
     * Create a new root board sequence
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board" )
    static UBoardSequence* CreateRootBoardSequence( const FString& BoardPath, const FString& BoardName );

    /**
     * Insert a new board sequence at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board" )
    static UMovieSceneSubSection* InsertBoardSequence( int32 StartFrame, int32 EndFrame );

    /**
     * Insert a new shot sequence at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board" )
    static UMovieSceneSubSection* InsertShotSequence( int32 StartFrame, int32 EndFrame );

    /**
     * Clone a subsection with its sequence
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board" )
    static UMovieSceneSubSection* CloneSection( UMovieSceneSubSection* SubSection, int32 FrameNumber );

    /**
     * Import an image sequence
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board" )
    static UBoardSequence* ImportImageSequence( const FString& BoardPath, const FString& BoardName, const FImportImageSequenceOptions& Options );

public:

    /**
     * Create a camera in a subsection at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board" )
    static void CreateCameraWithAnimation( UMovieSceneSubSection* SubSection, bool iSpawnable = true );

public:

    /**
     * Create a animation in a subsection at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board" )
    static void CreateAnimation( UMovieSceneSubSection* SubSection, bool iSpawnable = true );

    /**
     * Collapse animation's keys area
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board" )
    static void CollapseAnimation( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Expand animation's keys area
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board" )
    static void ExpandAnimation( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Get animation's keys area state
     */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Storyboard Editor|Board" )
    static bool IsAnimationCollapsed( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Get animation's keys area state
     */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Storyboard Editor|Board" )
    static bool IsAnimationExpanded( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Activate the lighttable on the plane
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board|Lighttable" )
    static void ActivateLighttable( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Deactivate the lighttable on the plane
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board|Lighttable" )
    static void DeactivateLighttable( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Get the state of the lighttable for the plane
     * It returns:
     *   1: lighttable on all animation layers is on
     *   0: lighttable on all animation layers is off
     *   -1: undetermined (mix of on and off, not enough animation layers)
     * @return 1 | 0 | -1 -> on | off | undetermined
     */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Storyboard Editor|Board|Lighttable" )
    static int32 GetLighttableState( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

public:

    /**
     * Create an animationcut in a animation at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board" )
    static void CreateAnimationCut( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding, int32 Frame );

public:

    /**
     * Rename a animation/camera binding and its actor
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board" )
    static void RenameBinding( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding, FString NewLabel );

public:

    /**
     * Create a new take in the subsection
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board|Take" )
    static FBoardSectionTake CreateTake( UMovieSceneSubSection* SubSection );

    /**
     * Switch take to another in the subsection
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Board|Take" )
    static FBoardSectionTake SwitchTake( UMovieSceneSubSection* SubSection, FBoardSectionTake Take );

    /**
     * Get the list of takes in the subsection
     */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Storyboard Editor|Board|Take" )
    static TArray<FBoardSectionTake> GetTakes( UMovieSceneSubSection* SubSection );

    /**
     * Get the current take in the subsection
     */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Storyboard Editor|Board|Take" )
    static FBoardSectionTake GetCurrentTake( UMovieSceneSubSection* SubSection );

    /**
     * Check the validity of a take
     */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Storyboard Editor|Board|Take" )
    static bool IsValid( FBoardSectionTake Take );

    /**
     * Get the name of a take
     */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Storyboard Editor|Board|Take" )
    static FText GetDisplayName( FBoardSectionTake Take );

    /**
     * Compare 2 takes
     */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Storyboard Editor|Board|Take", meta = ( DisplayName = "Equal (BoardSectionTake)", CompactNodeTitle = "==" ) )
    static bool EqualEqual_BoardSectionTakeBoardSectionTake( FBoardSectionTake A, FBoardSectionTake B );
};

//---

UCLASS()
class EPOSSEQUENCEEDITOR_API UShotSequenceEditorBlueprintLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    /**
     * Get the currently opened root/master shot sequence asset
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor|Shot")
    static UShotSequence* GetRootShotSequence();

    /**
     * Get the currently focused/viewed shot sequence asset if there is a hierarchy of sequences.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor|Shot")
    static UShotSequence* GetFocusedShotSequence();

    /**
     * Step to next sibling shot.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor|Shot")
    static void StepToNextShot();

    /**
     * Step to previous sibling shot.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor|Shot")
    static void StepToPreviousShot();

public:

    /**
     * Create a camera in a shot at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Shot" )
    static void CreateCameraWithAnimation( bool iSpawnable = true );

public:

    /**
     * Create a animation in a shot at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Shot" )
    static void CreateAnimation( bool iSpawnable = true );

    /**
     * Activate the lighttable on the plane
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Shot|Lighttable" )
    static void ActivateLighttable( const FMovieSceneBindingProxy& Binding );

    /**
     * Deactivate the lighttable on the plane
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Shot|Lighttable" )
    static void DeactivateLighttable( const FMovieSceneBindingProxy& Binding );

    /**
     * Get the state of the lighttable for the plane
     * It returns:
     *   1: lighttable on all animation layers is on
     *   0: lighttable on all animation layers is off
     *   -1: undetermined (mix of on and off, not enough animation layers)
     * @return 1 | 0 | -1 -> on | off | undetermined
     */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Storyboard Editor|Shot|Lighttable" )
    static int32 GetLighttableState( const FMovieSceneBindingProxy& Binding );

public:

    /**
     * Create an naimationcut in a animation at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Shot" )
    static void CreateAnimationCut( const FMovieSceneBindingProxy& Binding, int32 Frame );

public:

    /**
     * Rename a animation/camera binding and its actor
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Shot" )
    static void RenameBinding( const FMovieSceneBindingProxy& Binding, FString NewLabel );

};

//---

UCLASS()
class EPOSSEQUENCEEDITOR_API UEposSequenceEditorBlueprintLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    //PATCH: used for UEposSequenceEditorBlueprintLibrary::LocateBoundObjects_PATCHBefore543() until the UMovieSceneSequenceExtensions::LocateBoundObjects() is patch in >=5.4.3, then remove it and also update the .cs file
    UE_DEPRECATED( 5.4, "Use UMovieSceneSequenceExtensions::LocateBoundObjects once fixed in 5.4.3" )
    /**
     * Find the objects bound to a binding in a sequence.
     *
     * Temporary replacement for LocateBoundObjects, to be removed once
     * the engine version is fixed.
     */
    UFUNCTION(BlueprintCallable, Category="Sequencer|Sequence", meta=(ScriptMethod))
    //UFUNCTION(BlueprintCallable, Category="Sequencer|Sequence", meta=(ScriptMethod, DeprecatedFunction, DeprecationMessage="Use UMovieSceneSequenceExtensions::LocateBoundObjects once fixed in 5.4.3"))
    static TArray<UObject*> LocateBoundObjects_PATCHBefore543(UMovieSceneSequence* Sequence, const FMovieSceneBindingProxy& InBinding, UObject* Context);

public:

    /**
     * Open a board sequence asset
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static bool OpenEposSequence( UEposMovieSceneSequence* EposSequence );

    /**
     * Get the currently opened root/master epos sequence asset
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static UEposMovieSceneSequence* GetRootEposSequence();

    /**
     * Get the focused epos sequence, which may be a subsequence of the opened root sequence.
     * Returns nullptr when no Sequencer is open or the focused sequence is not an epos sequence.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static UEposMovieSceneSequence* GetFocusedEposSequence();

    /**
     * Focus/view the sequence associated to the given sub sequence section.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void FocusEposSequence(UMovieSceneSubSection* SubSection);

    /**
     * Focus/view the parent sequence, popping out of the current sub sequence section.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void FocusParentSequence();

    /**
     * Get the current sub section hierarchy from the current sequence to the section associated with the focused sequence.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static TArray<UMovieSceneSubSection*> GetSubSequenceHierarchy();

    /**
     * Close
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void CloseEposSequence();

public:

    /**
     * Move and scale an animation from the camera
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static void MoveAndScaleActor( AActor* Actor, const ACineCameraActor* Camera, float NewDistance, EScaleActor ScaleType );

    /**
     * Modify camera focal length and scale an animation accordingly
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static void SetCameraFocalLengthAndScaleActor( TArray<AActor*> Actors, ACineCameraActor* Camera, float NewFocalLength, EScaleActor ScaleType );

    /**
     * Fit the actor to 100% of the camera view (if possible)
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static void FitActorToCameraView( AActor* Actor, const ACineCameraActor* Camera );

public:

    /**
     * Export in pdf
     *
     * @param Options The export options
     * @return The export state
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static bool ExportAsPDF( const FExportPDFOptions& Options );

    /**
     * Export in image sequence
     *
     * @param Options The export options
     * @return The export state
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static bool ExportAsImageSequence( const FExportImageSequenceOptions& Options );

public:

    /**
     * Get Settings
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static UEposSequenceEditorSettings* GetEposSequenceEditorDefaultSettings();

public:

    /**
     * Play the current level sequence
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void Play();

    /**
     * Pause the current level sequence
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void Pause();

public:

    UE_DEPRECATED( 5.4, "Use SetCurrentTime that takes a FMovieSceneSequencePlaybackParams" )
    /**
     * Set the playhead to a frame of the current sequence.
     *
     * Deprecated: use Set Current Time with playback parameters.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor", meta = ( DeprecatedFunction, DeprecationMessage = "Use SetCurrentTime that takes a FMovieSceneSequencePlaybackParams" ) )
    static void SetCurrentTime(int32 NewFrame);

    UE_DEPRECATED( 5.4, "Use GetCurrentTime that returns a FMovieSceneSequencePlaybackParams" )
    /**
     * Get the playhead frame of the current sequence.
     *
     * Deprecated: use Get Current Time with playback parameters.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor", meta = ( DeprecatedFunction, DeprecationMessage = "Use GetCurrentTime that returns a FMovieSceneSequencePlaybackParams" ) )
    static int32 GetCurrentTime();

    UE_DEPRECATED( 5.4, "Use SetCurrentLocalTime that takes a FMovieSceneSequencePlaybackParams" )
    /**
     * Set the playhead to a local frame of the current sequence.
     *
     * Deprecated: use Set Current Local Time with playback parameters.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor", meta = ( DeprecatedFunction, DeprecationMessage = "Use SetCurrentLocalTime that takes a FMovieSceneSequencePlaybackParams" ) )
    static void SetCurrentLocalTime(int32 NewFrame);

    UE_DEPRECATED( 5.4, "Use GetCurrentLocalTime that takes a FMovieSceneSequencePlaybackParams" )
    /**
     * Get the playhead local frame of the current sequence.
     *
     * Deprecated: use Get Current Local Time with playback parameters.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor", meta = ( DeprecatedFunction, DeprecationMessage = "Use GetCurrentLocalTime that returns a FMovieSceneSequencePlaybackParams" ) )
    static int32 GetCurrentLocalTime();

    /**
     * Set global playhead position for the current epos sequence. If the requested time is the same as the current time, an evaluation will be forced.
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor", DisplayName = "Set Current Time" )
    static void SetGlobalPosition( FMovieSceneSequencePlaybackParams PlaybackParams, EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate );

    /**
     * Get the current global playhead position
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor", DisplayName = "Get Current Time" )
    static FMovieSceneSequencePlaybackParams GetGlobalPosition( EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate );

    /**
     * Set local playhead position for the current epos sequence. If the requested time is the same as the current time, an evaluation will be forced.
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor", DisplayName = "Set Current Local Time" )
    static void SetLocalPosition( FMovieSceneSequencePlaybackParams PlaybackParams, EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate );

    /**
     * Get the current local playhead position
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor", DisplayName = "Get Current Local Time" )
    static FMovieSceneSequencePlaybackParams GetLocalPosition( EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate );

    /**
     * Set playback speed of the current epos sequence. Negative speeds play in reverse; zero and non-finite
     * speeds raise an error and leave the speed unchanged.
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static void SetPlaybackSpeed( float NewPlaybackSpeed );

    /**
     * Get playback speed of the current epos sequence
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static float GetPlaybackSpeed();

    /**
     * Set loop mode (note this is a per user preference)
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static void SetLoopMode( ESequencerLoopMode NewLoopMode );

    /**
     * Get loop mode (note this is a per user preference)
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static ESequencerLoopMode GetLoopMode();

    /**
     * Play from the current time to the requested time in frames
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void PlayTo(FMovieSceneSequencePlaybackParams PlaybackParams, EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate);

public:

    /** Return the playback start position */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor", DisplayName = "Get Playback Start Time" )
    static FMovieSceneSequencePlaybackParams GetPlaybackStartPosition( EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate );

    /** Return end of the playback range in the Sequencer UI, which accounts for the exclusive upper bound */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor", DisplayName = "Get Playback End Time" )
    static FMovieSceneSequencePlaybackParams GetPlaybackEndPosition( EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate );

    /** Check whether the sequence is actively playing. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static bool IsPlaying();

public:

    /** Gets the currently selected tracks, or an empty array when no Sequencer is open. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static TArray<UMovieSceneTrack*> GetSelectedTracks();

    /** Gets the currently selected sections, or an empty array when no Sequencer is open. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static TArray<UMovieSceneSection*> GetSelectedSections();

    /** Gets the currently selected channels. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static TArray<FSequencerChannelProxy> GetSelectedChannels();

    /** Gets the channel with selected keys. */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Storyboard Editor" )
    static TArray<FSequencerChannelProxy> GetChannelsWithSelectedKeys();

    /** Gets the selected key indices with this channel */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Storyboard Editor" )
    static TArray<int32> GetSelectedKeys( const FSequencerChannelProxy& ChannelProxy );

    /** Gets selected folders, including parent folders of selected tracks or bindings. Returns an empty array when no Sequencer is open. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static TArray<UMovieSceneFolder*> GetSelectedFolders();

    /** Gets only folders explicitly selected in the outliner. Returns an empty array when no Sequencer is open. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static TArray<UMovieSceneFolder*> GetDirectlySelectedFolders();

    /** Gets the currently selected object bindings for the focused sequence. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static TArray<FMovieSceneBindingProxy> GetSelectedBindings();

    /** Gets the currently selected marked frames, as indices into the focused movie scene's marked frames array. */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Storyboard Editor" )
    static TArray<int32> GetSelectedMarkedFrames();

    /** Adds tracks from the focused sequence to the selection without clearing it. Raises an error when no Sequencer is open, or without selecting anything when any track is invalid or foreign. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void SelectTracks(const TArray<UMovieSceneTrack*>& Tracks);

    /** Adds sections from the focused sequence to the selection without clearing it. Raises an error when no Sequencer is open, or without selecting anything when any section is invalid or foreign. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void SelectSections(const TArray<UMovieSceneSection*>& Sections);

    /** Adds channels to the selection without clearing it. A channel name matches a channel of the section or the label of the outliner node that contains it. Raises an error when no Sequencer is open, or without selecting anything when any section is foreign or any name matches nothing. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void SelectChannels(const TArray<FSequencerChannelProxy>& Channels);

    /** Select keys from indices */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static void SelectKeys( const FSequencerChannelProxy& Channel, const TArray<int32>& Indices );

    /** Adds folders from the focused sequence to the selection without clearing it. Raises an error when no Sequencer is open, or without selecting anything when any folder is invalid or foreign. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void SelectFolders(const TArray<UMovieSceneFolder*>& Folders);

    /** Adds bindings of the focused sequence to the selection without clearing it. Raises an error when no Sequencer is open, or without selecting anything when any binding is invalid or foreign. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void SelectBindings(const TArray<FMovieSceneBindingProxy>& ObjectBindings);

    /** Deselect bindings */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void DeselectBindings(const TArray<FMovieSceneBindingProxy>& ObjectBindings);

    /** Select marked frames by index into the focused movie scene's marked frames array. Adds to the current marked frame selection. */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static void SelectMarkedFrames( const TArray<int32>& MarkedFrameIndices );

    /** Deselect marked frames by index into the focused movie scene's marked frames array. */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static void DeselectMarkedFrames( const TArray<int32>& MarkedFrameIndices );

    /** Empties the current selection. Does nothing when no Sequencer is open. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void EmptySelection();

    /** Empties the marked frame selection, leaving all other selection intact. */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static void EmptyMarkedFrameSelection();

    /** Empties the key selection, leaving all other selection intact. */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static void EmptyKeySelection();

    /** Set the selection range start frame. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void SetSelectionRangeStart(int32 NewFrame);

    /** Set the selection range end frame. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void SetSelectionRangeEnd(int32 NewFrame);

    /** Get the selection range start frame. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static int32 GetSelectionRangeStart();

    /** Get the selection range end frame. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static int32 GetSelectionRangeEnd();

    ///** Whether the focused sequence has a selection range. When it does not, the selection range start and end are both 0. */
    //UFUNCTION( meta = ( PythonCallable, AICallable, AIAccessMode = "Read" ) )
    //static bool HasSelectionRange();

    ///** Clear the focused sequence's selection range as an undoable step, as Clear Selection Range in the time slider does, including on a read-only sequence. Raises when no Sequencer is open. */
    //UFUNCTION( meta = ( PythonCallable, AICallable, AIAccessMode = "Write" ) )
    //static void ClearSelectionRange();

public:

    /** Refresh Sequencer UI on next tick */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void RefreshCurrentEposSequence();

    /** Force sequencer evaluation and UI update immediately */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static void ForceUpdate();

    /**
     * Restore all pre-animated state captured by the current Sequencer, undoing
     * transient evaluation side-effects so the affected objects are no longer
     * referenced by the evaluation. Editor sequences default to KeepState, so
     * closing them leaves things like AnimMixer AnimNext injections active;
     * calling this first releases those references (useful before deleting a
     * sequence, e.g. in automated tests, so ForceDeleteObjects can unload it).
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static void RestorePreAnimatedState();

    /** Get the object bound to the given binding ID with the current Epos Sequence Editor */
    UFUNCTION(BlueprintPure, Category="Odyssey|Storyboard Editor")
    static TArray<UObject*> GetBoundObjects(FMovieSceneObjectBindingID ObjectBinding);

    /** Check whether the current level sequence and its descendants are locked for editing. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static bool IsEposSequenceLocked();

    /** Sets the lock for the current level sequence and its descendants for editing. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void SetLockEposSequence(bool bLock);

public:

    /** Check whether the lock for the viewport to the camera cuts is enabled. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static bool IsCameraCutLockedToViewport();

    /** Sets the lock for the viewport to the camera cuts. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void SetLockCameraCutToViewport(bool bLock);

    /** Check whether the lock for the viewport on the board track to the inner camera cuts is enabled. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static bool IsBoardInnerCameraCutLockedToViewport();

    /** Sets the lock for the viewport on the board track to the inner camera cuts. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void SetLockBoardInnerCameraCutToViewport(bool bLock);

public:

    UE_DEPRECATED( 5.5, "Use IsTrackFilterActive" )
    /**
     * Get whether the specified track filter is on or off.
     *
     * Deprecated: use Is Track Filter Active.
     */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static bool IsTrackFilterEnabled(const FText& TrackFilterName);

    /** Checks active state by the filter's localized display name. Unknown names and a closed Sequencer return false. */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Storyboard Editor" )
    static bool IsTrackFilterActive( const FText& TrackFilterName );

    UE_DEPRECATED( 5.5, "Use SetTrackFilterActive" )
    /**
     * Set the specified track filter on or off.
     *
     * Deprecated: use Set Track Filter Active.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    static void SetTrackFilterEnabled(const FText& TrackFilterName, bool bEnabled);

    /** Sets the specified track filter to be on or off by display name, ignoring case. Raises an error when no Sequencer is open or no filter has that name. */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor" )
    static void SetTrackFilterActive( const FText& TrackFilterName, bool bActive );

    /** Gets available localized filter display names, or an empty array when no Sequencer is open. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Storyboard Editor")
    static TArray<FText> GetTrackFilterNames();

    ///** Gets the filter bar's search text. Raises an error when no Sequencer is open. */
    //UFUNCTION( meta = ( PythonCallable, AICallable, AIAccessMode = "Read" ) )
    //static FString GetTrackFilterText();

    ///** Sets the filter bar's search text, as typing in the search box does. An empty string clears it. Raises an error when no Sequencer is open. */
    //UFUNCTION( meta = ( PythonCallable, AICallable, AIAccessMode = "Write" ) )
    //static void SetTrackFilterText( const FString& FilterText );

public:
    UE_DEPRECATED( 5.4, "Use USequencerCurveEditorObject::HasCustomColorForChannel" )
    /**
     * Check if a channel has a custom color.
     *
     * The channel is identified by its class and identifier.
     *
     * Deprecated: use the Sequencer Curve Editor equivalent.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor",
    meta = (DeprecatedFunction, DeprecationMessage = "Use USequencerCurveEditorObject::HasCustomColorForChannel"))
    static bool HasCustomColorForChannel(UClass* Class, const FString& Identifier);

    UE_DEPRECATED( 5.4, "Use USequencerCurveEditorObject::GetCustomColorForChannel" )
    /**
     * Get the custom color of a channel.
     *
     * The channel is identified by its class and identifier. Return
     * white if none exists.
     *
     * Deprecated: use the Sequencer Curve Editor equivalent.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor",
    meta = (DeprecatedFunction, DeprecationMessage = "Use USequencerCurveEditorObject::HasCustomColorForChannel"))
    static FLinearColor GetCustomColorForChannel(UClass* Class, const FString& Identifier);

    UE_DEPRECATED( 5.4, "Use USequencerCurveEditorObject::SetCustomColorForChannel" )
    /**
     * Set a custom color for a channel.
     *
     * The channel is identified by its class and identifier. The color
     * is stored in the editor user preferences.
     *
     * Deprecated: use the Sequencer Curve Editor equivalent.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor",
    meta = (DeprecatedFunction, DeprecationMessage = "Use USequencerCurveEditorObject::SetCustomColorForChannel"))
    static void SetCustomColorForChannel(UClass* Class, const FString& Identifier, const FLinearColor& NewColor);

    UE_DEPRECATED( 5.4, "Use USequencerCurveEditorObject::DeleteColorForChannels" )
    /**
     * Set custom colors for several channels.
     *
     * The channels are identified by their class and identifiers. The
     * colors are stored in the editor user preferences.
     *
     * Deprecated: use the Sequencer Curve Editor equivalent.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor",
    meta = (DeprecatedFunction, DeprecationMessage = "Use USequencerCurveEditorObject::DeleteColorForChannels"))
    static void SetCustomColorForChannels(UClass* Class, const TArray<FString>& Identifiers, const TArray<FLinearColor>& NewColors);

    UE_DEPRECATED( 5.4, "Use USequencerCurveEditorObject::SetRandomColorForChannels" )
    /**
     * Set random colors for several channels.
     *
     * The channels are identified by their class and identifiers. The
     * colors are stored in the editor user preferences.
     *
     * Deprecated: use the Sequencer Curve Editor equivalent.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor",
    meta = (DeprecatedFunction, DeprecationMessage = "Use USequencerCurveEditorObject::SetRandomColorForChannels"))
    static void SetRandomColorForChannels(UClass* Class, const TArray<FString>& Identifiers);

    UE_DEPRECATED( 5.4, "Use USequencerCurveEditorObject::DeleteColorForChannels" )
    /**
     * Delete the custom color of a channel.
     *
     * The channel is identified by its class and identifier.
     *
     * Deprecated: use the Sequencer Curve Editor equivalent.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor",
    meta = (DeprecatedFunction, DeprecationMessage = "Use USequencerCurveEditorObject::DeleteColorForChannels"))
    static void DeleteColorForChannels(UClass* Class, const FString& Identifier);

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

    /**
     * Return the editor's currently-tracked sequencer, or nullptr if no
     * epos sequence is open. Counterpart to SetSequencer. Used by
     * scripting modules that need to drive the active ISequencer (e.g.
     * to evaluate the sequence for bone matching, root-motion baking).
     *
     * Not a UFUNCTION because ISequencer is not a Blueprint-exposed type;
     * intended for native C++ scripting callers.
     */
    static TSharedPtr<ISequencer> GetCurrentSequencer();
};
