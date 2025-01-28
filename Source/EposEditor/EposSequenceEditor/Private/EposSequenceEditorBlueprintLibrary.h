// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor|Board")
    static UBoardSequence* GetRootBoardSequence();

    /**
     * Get the currently focused/viewed board sequence asset if there is a hierarchy of sequences.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor|Board")
    static UBoardSequence* GetFocusedBoardSequence();

public:

    /**
     * Create a new root board sequence
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static UBoardSequence* CreateRootBoardSequence( const FString& BoardPath, const FString& BoardName );

    /**
     * Insert a new board sequence at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static UMovieSceneSubSection* InsertBoardSequence( int32 StartFrame, int32 EndFrame );

    /**
     * Insert a new shot sequence at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static UMovieSceneSubSection* InsertShotSequence( int32 StartFrame, int32 EndFrame );

    /**
     * Clone a subsection with its sequence
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static UMovieSceneSubSection* CloneSection( UMovieSceneSubSection* SubSection, int32 FrameNumber );

    /**
     * Import an image sequence
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static UBoardSequence* ImportImageSequence( const FString& BoardPath, const FString& BoardName, const FImportImageSequenceOptions& Options );

public:

    /**
     * Create a camera in a subsection at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static void CreateCamera( UMovieSceneSubSection* SubSection );

public:

    /**
     * Create a plane in a subsection at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static void CreatePlane( UMovieSceneSubSection* SubSection );

    /**
     * Collapse plane's keys area
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static void CollapsePlane( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Expand plane's keys area
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static void ExpandPlane( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Get plane's keys area state
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Board" )
    static bool IsPlaneCollapsed( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Get plane's keys area state
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Board" )
    static bool IsPlaneExpanded( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Activate the lighttable on the plane
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board|Lighttable" )
    static void ActivateLighttable( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Deactivate the lighttable on the plane
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board|Lighttable" )
    static void DeactivateLighttable( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

    /**
     * Get the state of the lighttable for the plane
     * It returns:
     *   1: lighttable on all drawings is on
     *   0: lighttable on all drawings is off
     *   -1: undetermined (mix of on and off, not enough drawings)
     * @return 1 | 0 | -1 -> on | off | undetermined
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Board|Lighttable" )
    static int32 GetLighttableState( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding );

public:

    /**
     * Create a drawing in a plane at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static void CreateDrawing( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding, int32 Frame );

public:

    /**
     * Rename a plane/camera binding and its actor
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board" )
    static void RenameBinding( UMovieSceneSubSection* SubSection, const FMovieSceneBindingProxy& Binding, FString NewLabel );

public:

    /**
     * Create a new take in the subsection
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board|Take" )
    static FBoardSectionTake CreateTake( UMovieSceneSubSection* SubSection );

    /**
     * Switch take to another in the subsection
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Board|Take" )
    static FBoardSectionTake SwitchTake( UMovieSceneSubSection* SubSection, FBoardSectionTake Take );

    /**
     * Get the list of takes in the subsection
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Board|Take" )
    static TArray<FBoardSectionTake> GetTakes( UMovieSceneSubSection* SubSection );

    /**
     * Get the current take in the subsection
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Board|Take" )
    static FBoardSectionTake GetCurrentTake( UMovieSceneSubSection* SubSection );

    /**
     * Check the validity of a take
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Board|Take" )
    static bool IsValid( FBoardSectionTake Take );

    /**
     * Get the name of a take
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Board|Take" )
    static FText GetDisplayName( FBoardSectionTake Take );

    /**
     * Compare 2 takes
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Board|Take", meta = ( DisplayName = "Equal (BoardSectionTake)", CompactNodeTitle = "==" ) )
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
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor|Shot")
    static UShotSequence* GetRootShotSequence();

    /**
     * Get the currently focused/viewed shot sequence asset if there is a hierarchy of sequences.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor|Shot")
    static UShotSequence* GetFocusedShotSequence();

    /**
     * Step to next sibling shot.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor|Shot")
    static void StepToNextShot();

    /**
     * Step to previous sibling shot.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor|Shot")
    static void StepToPreviousShot();

public:

    /**
     * Create a camera in a shot at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Shot" )
    static void CreateCamera();

public:

    /**
     * Create a plane in a shot at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Shot" )
    static void CreatePlane();

    /**
     * Activate the lighttable on the plane
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Shot|Lighttable" )
    static void ActivateLighttable( const FMovieSceneBindingProxy& Binding );

    /**
     * Deactivate the lighttable on the plane
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Shot|Lighttable" )
    static void DeactivateLighttable( const FMovieSceneBindingProxy& Binding );

    /**
     * Get the state of the lighttable for the plane
     * It returns:
     *   1: lighttable on all drawings is on
     *   0: lighttable on all drawings is off
     *   -1: undetermined (mix of on and off, not enough drawings)
     * @return 1 | 0 | -1 -> on | off | undetermined
     */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor|Shot|Lighttable" )
    static int32 GetLighttableState( const FMovieSceneBindingProxy& Binding );

public:

    /**
     * Create a drawing in a plane at the given frame
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor|Shot" )
    static void CreateDrawing( const FMovieSceneBindingProxy& Binding, int32 Frame );

public:

    /**
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
    //PATCH: used for UEposSequenceEditorBlueprintLibrary::LocateBoundObjects_PATCHBefore543() until the UMovieSceneSequenceExtensions::LocateBoundObjects() is patch in >=5.4.3, then remove it and also update the .cs file
    UE_DEPRECATED( 5.4, "Use UMovieSceneSequenceExtensions::LocateBoundObjects once fixed in 5.4.3" )
    UFUNCTION(BlueprintCallable, Category="Sequencer|Sequence", meta=(ScriptMethod))
    //UFUNCTION(BlueprintCallable, Category="Sequencer|Sequence", meta=(ScriptMethod, DeprecatedFunction, DeprecationMessage="Use UMovieSceneSequenceExtensions::LocateBoundObjects once fixed in 5.4.3"))
    static TArray<UObject*> LocateBoundObjects_PATCHBefore543(UMovieSceneSequence* Sequence, const FMovieSceneBindingProxy& InBinding, UObject* Context);

public:

    /**
     * Open a board sequence asset
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static bool OpenEposSequence( UEposMovieSceneSequence* EposSequence );

    /**
     * Get the currently opened root/master epos sequence asset
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static UEposMovieSceneSequence* GetRootEposSequence();

    /**
     * Get the currently focused/viewed epos sequence asset if there is a hierarchy of sequences.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static UEposMovieSceneSequence* GetFocusedEposSequence();

    /**
     * Focus/view the sequence associated to the given sub sequence section.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void FocusEposSequence(UMovieSceneSubSection* SubSection);

    /**
     * Focus/view the parent sequence, popping out of the current sub sequence section.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void FocusParentSequence();

    /**
     * Get the current sub section hierarchy from the current sequence to the section associated with the focused sequence.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static TArray<UMovieSceneSubSection*> GetSubSequenceHierarchy();

    /**
     * Close
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void CloseEposSequence();

public:

    /**
     * Move and scale a plane from the camera
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor" )
    static void MoveAndScalePlane( APlaneActor* Plane, const ACineCameraActor* Camera, float NewDistance, EScalePlane ScaleType );

    /**
     * Move and scale a plane from the camera
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor" )
    static void SetCameraFocalLengthAndScalePlane( TArray<APlaneActor*> Planes, ACineCameraActor* Camera, float NewFocalLength, EScalePlane ScaleType );

public:

    /**
     * Export in pdf
     *
     * @param Options The export options
     * @return The export state
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor" )
    static bool ExportAsPDF( const FExportPDFOptions& Options );

    /**
     * Export in image sequence
     *
     * @param Options The export options
     * @return The export state
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor" )
    static bool ExportAsImageSequence( const FExportImageSequenceOptions& Options );

public:

    /**
     * Get Settings
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor" )
    static UEposSequenceEditorSettings* GetEposSequenceEditorDefaultSettings();

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

    UE_DEPRECATED( 5.4, "Use SetCurrentTime that takes a FMovieSceneSequencePlaybackParams" )
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor", meta = ( DeprecatedFunction, DeprecationMessage = "Use SetCurrentTime that takes a FMovieSceneSequencePlaybackParams" ) )
    static void SetCurrentTime(int32 NewFrame);

    UE_DEPRECATED( 5.4, "Use GetCurrentTime that returns a FMovieSceneSequencePlaybackParams" )
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor", meta = ( DeprecatedFunction, DeprecationMessage = "Use GetCurrentTime that returns a FMovieSceneSequencePlaybackParams" ) )
    static int32 GetCurrentTime();

    UE_DEPRECATED( 5.4, "Use SetCurrentLocalTime that takes a FMovieSceneSequencePlaybackParams" )
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor", meta = ( DeprecatedFunction, DeprecationMessage = "Use SetCurrentLocalTime that takes a FMovieSceneSequencePlaybackParams" ) )
    static void SetCurrentLocalTime(int32 NewFrame);

    UE_DEPRECATED( 5.4, "Use GetCurrentLocalTime that takes a FMovieSceneSequencePlaybackParams" )
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor", meta = ( DeprecatedFunction, DeprecationMessage = "Use GetCurrentLocalTime that returns a FMovieSceneSequencePlaybackParams" ) )
    static int32 GetCurrentLocalTime();

    /**
     * Set global playhead position for the current epos sequence. If the requested time is the same as the current time, an evaluation will be forced.
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor", DisplayName = "Set Current Time" )
    static void SetGlobalPosition( FMovieSceneSequencePlaybackParams PlaybackParams, EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate );

    /**
     * Get the current global playhead position
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor", DisplayName = "Get Current Time" )
    static FMovieSceneSequencePlaybackParams GetGlobalPosition( EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate );

    /**
     * Set local playhead position for the current epos sequence. If the requested time is the same as the current time, an evaluation will be forced.
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor", DisplayName = "Set Current Local Time" )
    static void SetLocalPosition( FMovieSceneSequencePlaybackParams PlaybackParams, EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate );

    /**
     * Get the current local playhead position
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor", DisplayName = "Get Current Local Time" )
    static FMovieSceneSequencePlaybackParams GetLocalPosition( EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate );

    /**
     * Set playback speed of the current epos sequence
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor" )
    static void SetPlaybackSpeed( float NewPlaybackSpeed );

    /**
     * Get playback speed of the current epos sequence
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor" )
    static float GetPlaybackSpeed();

    /**
     * Set loop mode (note this is a per user preference)
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor" )
    static void SetLoopMode( ESequencerLoopMode NewLoopMode );

    /**
     * Get loop mode (note this is a per user preference)
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor" )
    static ESequencerLoopMode GetLoopMode();

    /**
     * Play from the current time to the requested time in frames
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void PlayTo(FMovieSceneSequencePlaybackParams PlaybackParams, EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate);

public:

    /** Return the playback start position */
    UFUNCTION( BlueprintCallable, Category = "Level Sequence Editor", DisplayName = "Get Playback Start Time" )
    static FMovieSceneSequencePlaybackParams GetPlaybackStartPosition( EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate );

    /** Return end of the playback range in the Sequencer UI, which accounts for the exclusive upper bound */
    UFUNCTION( BlueprintCallable, Category = "Level Sequence Editor", DisplayName = "Get Playback End Time" )
    static FMovieSceneSequencePlaybackParams GetPlaybackEndPosition( EMovieSceneTimeUnit TimeUnit = EMovieSceneTimeUnit::DisplayRate );

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

    /** Gets the currently selected channels. */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static TArray<FSequencerChannelProxy> GetSelectedChannels();

    /** Gets the channel with selected keys. */
    UFUNCTION( BlueprintPure, Category = "Level Sequence Editor" )
    static TArray<FSequencerChannelProxy> GetChannelsWithSelectedKeys();

    /** Gets the selected key indices with this channel */
    UFUNCTION( BlueprintPure, Category = "Level Sequence Editor" )
    static TArray<int32> GetSelectedKeys( const FSequencerChannelProxy& ChannelProxy );

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

    /** Select channels */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SelectChannels(const TArray<FSequencerChannelProxy>& Channels);

    /** Select keys from indices */
    UFUNCTION( BlueprintCallable, Category = "Level Sequence Editor" )
    static void SelectKeys( const FSequencerChannelProxy& Channel, const TArray<int32>& Indices );

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

    /** Refresh Sequencer UI on next tick */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void RefreshCurrentEposSequence();

    /** Force sequencer evaluation and UI update immediately */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor" )
    static void ForceUpdate();

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

    /** Check whether the lock for the viewport on the board track to the inner camera cuts is enabled. */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static bool IsBoardInnerCameraCutLockedToViewport();

    /** Sets the lock for the viewport on the board track to the inner camera cuts. */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SetLockBoardInnerCameraCutToViewport(bool bLock);

public:

    /** Gets whether the specified track filter is on/off */
    UE_DEPRECATED( 5.5, "Use IsTrackFilterActive" )
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static bool IsTrackFilterEnabled(const FText& TrackFilterName);

    /** Gets whether the specified track filter is on/off */
    UFUNCTION( BlueprintPure, Category = "Epos Sequence Editor" )
    static bool IsTrackFilterActive( const FText& TrackFilterName );

    /** Sets the specified track filter to be on or off */
    UE_DEPRECATED( 5.5, "Use SetTrackFilterActive" )
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    static void SetTrackFilterEnabled(const FText& TrackFilterName, bool bEnabled);

    /** Sets the specified track filter to be on or off */
    UFUNCTION( BlueprintCallable, Category = "Epos Sequence Editor" )
    static void SetTrackFilterActive( const FText& TrackFilterName, bool bActive );

    /** Gets all the available track filter names */
    UFUNCTION(BlueprintPure, Category = "Epos Sequence Editor")
    static TArray<FText> GetTrackFilterNames();

public:
    /** Get if a custom color for specified channel idendified by it's class and identifier exists */
    UE_DEPRECATED( 5.4, "Use USequencerCurveEditorObject::HasCustomColorForChannel" )
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor",
    meta = (DeprecatedFunction, DeprecationMessage = "Use USequencerCurveEditorObject::HasCustomColorForChannel"))
    static bool HasCustomColorForChannel(UClass* Class, const FString& Identifier);

    /** Get custom color for specified channel idendified by it's class and identifier,if none exists will return white*/
    UE_DEPRECATED( 5.4, "Use USequencerCurveEditorObject::GetCustomColorForChannel" )
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor",
    meta = (DeprecatedFunction, DeprecationMessage = "Use USequencerCurveEditorObject::HasCustomColorForChannel"))
    static FLinearColor GetCustomColorForChannel(UClass* Class, const FString& Identifier);

    /** Set Custom Color for specified channel idendified by it's class and identifier. This will be stored in editor user preferences.*/
    UE_DEPRECATED( 5.4, "Use USequencerCurveEditorObject::SetCustomColorForChannel" )
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor",
    meta = (DeprecatedFunction, DeprecationMessage = "Use USequencerCurveEditorObject::SetCustomColorForChannel"))
    static void SetCustomColorForChannel(UClass* Class, const FString& Identifier, const FLinearColor& NewColor);

    /** Set Custom Color for specified channels idendified by it's class and identifiers. This will be stored in editor user preferences.*/
    UE_DEPRECATED( 5.4, "Use USequencerCurveEditorObject::DeleteColorForChannels" )
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor",
    meta = (DeprecatedFunction, DeprecationMessage = "Use USequencerCurveEditorObject::DeleteColorForChannels"))
    static void SetCustomColorForChannels(UClass* Class, const TArray<FString>& Identifiers, const TArray<FLinearColor>& NewColors);

    /** Set Random Colors for specified channels idendified by it's class and identifiers. This will be stored in editor user preferences.*/
    UE_DEPRECATED( 5.4, "Use USequencerCurveEditorObject::SetRandomColorForChannels" )
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor",
    meta = (DeprecatedFunction, DeprecationMessage = "Use USequencerCurveEditorObject::SetRandomColorForChannels"))
    static void SetRandomColorForChannels(UClass* Class, const TArray<FString>& Identifiers);

    /** Delete for specified channel idendified by it's class and identifier.*/
    UE_DEPRECATED( 5.4, "Use USequencerCurveEditorObject::DeleteColorForChannels" )
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor",
    meta = (DeprecatedFunction, DeprecationMessage = "Use USequencerCurveEditorObject::DeleteColorForChannels"))
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
