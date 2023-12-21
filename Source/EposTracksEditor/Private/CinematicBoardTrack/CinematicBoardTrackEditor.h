// IDDN.FR.001.220036.002.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "Templates/SubclassOf.h"
#include "Widgets/SWidget.h"
#include "ISequencer.h"
#include "ISequencerSection.h"
#include "MovieSceneTrack.h"
#include "ISequencerTrackEditor.h"
#include "TrackEditors/SubTrackEditor.h"

#include "ArrangeSectionsType.h"

class AActor;
struct FAssetData;
class FMenuBuilder;
class FTrackEditorThumbnailPool;
class UMovieSceneCinematicBoardSection;
class UMovieSceneCinematicBoardTrack;
class UMovieSceneSubSection;

/**
 * Tools for boards.
 */
class FCinematicBoardTrackEditor
    : public FSubTrackEditor
{
public:

    /**
     * Constructor
     *
     * @param InSequencer The sequencer instance to be used by this tool.
     */
    FCinematicBoardTrackEditor( TSharedRef<ISequencer> iSequencer );

    /** Virtual destructor. */
    virtual ~FCinematicBoardTrackEditor()
    {
    }

    /**
     * Creates an instance of this class.  Called by a sequencer .
     *
     * @param OwningSequencer The sequencer instance to be used by this tool.
     * @return The new instance of this class.
     */
    static TSharedRef<ISequencerTrackEditor> CreateTrackEditor( TSharedRef<ISequencer> iOwningSequencer );

    TWeakObjectPtr<AActor> GetBoardCamera() const;

public:

    // ISequencerTrackEditor interface
    virtual void OnInitialize() override;
    virtual void OnRelease() override;
    virtual void BindCommands( TSharedRef<FUICommandList> SequencerCommandBindings ) override;
    virtual void BuildAddTrackMenu( FMenuBuilder& ioMenuBuilder ) override;
    virtual TSharedPtr<SWidget> BuildOutlinerEditWidget( const FGuid& iObjectBinding, UMovieSceneTrack* iTrack, const FBuildEditWidgetParams& iParams ) override;
    virtual TSharedRef<ISequencerSection> MakeSectionInterface( UMovieSceneSection& ioSectionObject, UMovieSceneTrack& ioTrack, FGuid iObjectBinding ) override;
    //virtual bool HandleAssetAdded( UObject* iAsset, const FGuid& iTargetObjectGuid ) override;
    virtual bool SupportsSequence( UMovieSceneSequence* iSequence ) const override;
    //virtual bool SupportsType( TSubclassOf<UMovieSceneTrack> iType ) const override;
    virtual void Tick( float iDeltaTime ) override;
    virtual void BuildTrackContextMenu( FMenuBuilder& ioMenuBuilder, UMovieSceneTrack* iTrack ) override;
    virtual const FSlateBrush* GetIconBrush() const override;
    //virtual bool OnAllowDrop( const FDragDropEvent& iDragDropEvent, FSequencerDragDropParams& DragDropParams ) override;
    virtual FReply OnDrop( const FDragDropEvent& iDragDropEvent, const FSequencerDragDropParams& DragDropParams ) override;

public:

    /** Insert sequence into this track */
    virtual void InsertSection( UMovieSceneTrack* Track )
    {
        checkNoEntry();
    }

    /** Duplicate the section into this track */
    virtual void DuplicateSection( UMovieSceneSubSection* Section )
    {
        checkNoEntry();
    }

    /** Create a new take of the given section */
    virtual void CreateNewTake( UMovieSceneSubSection* Section )
    {
        checkNoEntry();
    }

    /** Switch the selected section's take sequence */
    virtual void ChangeTake( UMovieSceneSequence* Sequence )
    {
        checkNoEntry();
    }

    /** Generate a menu for takes for this section */
    virtual void AddTakesMenu( UMovieSceneSubSection* Section, FMenuBuilder& MenuBuilder )
    {
        checkNoEntry();
    }

    /**
     * Check whether the given sequence can be added as a sub-sequence.
     *
     * The purpose of this method is to disallow circular references
     * between sub-sequences in the focused movie scene.
     *
     * @param Sequence The sequence to check.
     * @return true if the sequence can be added as a sub-sequence, false otherwise.
     */
     // No more needed, defined & used in FSubTrackEditor
     // in addition it's not virtual ...
     //bool CanAddSubSequence( const UMovieSceneSequence& iSequence ) const;

    //---

    /** Insert board. */
    void InsertBoard();

    /** Insert shot. */
    void InsertShot();

public:

    // FSubTrackEditor interface
    virtual FText GetSubTrackName() const override;
    virtual FText GetSubTrackToolTip() const override;
    virtual FName GetSubTrackBrushName() const override;
    virtual FString GetSubSectionDisplayName( const UMovieSceneSubSection* Section ) const override;
    virtual FString GetDefaultSubsequenceName() const override;
    virtual FString GetDefaultSubsequenceDirectory() const override;
    virtual TSubclassOf<UMovieSceneSubTrack> GetSubTrackClass() const;

protected:

    /** Get the list of supported sequence class paths */
    virtual void GetSupportedSequenceClassPaths( TArray<FTopLevelAssetPath>& OutClassPaths ) const;

    /** Callback for executing the "Add Subsequence" menu entry. */
    virtual void HandleAddSubTrackMenuEntryExecute();

    /** Callback for determining whether the "Add Subsequence" menu entry can execute. */
    virtual bool HandleAddSubTrackMenuEntryCanExecute() const;

    /** Callback for determining whether the "Add Board" menu entry is visible. */
    bool HandleAddCinematicBoardTrackMenuEntryIsVisible();

    /** Whether to handle this asset being dropped onto the sequence as opposed to a specific track. */
    virtual bool CanHandleAssetAdded( UMovieSceneSequence* Sequence ) const;

    /** Find or create a sub track. If the given track is a subtrack, it will be returned. */
    UMovieSceneSubTrack* FindOrCreateSubTrack( UMovieScene* MovieScene, UMovieSceneTrack* Track ) const;

    /** Callback for generating the menu of the "Add Sequence" combo button. */
    TSharedRef<SWidget> HandleAddSubSequenceComboButtonGetMenuContent( UMovieSceneTrack* InTrack );

private:

    /** Delegate for AnimatablePropertyChanged in AddKey */
    // add:
    // - == TEXT( "BoardSequence" ) / TEXT( "ShotSequence" )
    // - BoardSequenceTools::UpdateViewRange( GetSequencer().Get(), newSection->GetTrueRange() );
    //FKeyPropertyResult AddKeyInternal( FFrameNumber iKeyTime, UMovieSceneSequence* iMovieSceneSequence, UMovieSceneTrack* iTrack, int32 iRowIndex, TOptional<FFrameNumber> iDroppedFrame );

    /** Callback for AnimatablePropertyChanged in HandleAssetAdded. */
    // add:
    // - BoardSequenceTools::UpdateViewRange( GetSequencer().Get(), newSection->GetTrueRange() );
    //FKeyPropertyResult HandleSequenceAdded( FFrameNumber iKeyTime, UMovieSceneSequence* iSequence, UMovieSceneTrack* iTrack, int32 iRowIndex );

private:

    void SetArrangeSections( EArrangeSections iArrangeSections );
    bool IsArrangeSections( EArrangeSections iArrangeSections );

    /** Delegate for boards button lock state */
    ECheckBoxState AreBoardsLocked() const;

    /** Delegate for locked boards button */
    void OnLockBoardsClicked( ECheckBoxState iCheckBoxState );

    /** Delegate for boards button lock tooltip */
    FText GetLockBoardsToolTip() const;

    /** Called when our sequencer wants to switch cameras */
    void OnUpdateCameraCut( UObject* iCameraObject, bool iJumpCut );

private:

    /** The Thumbnail pool which draws all the viewport thumbnails for the board track. */
    TSharedPtr<FTrackEditorThumbnailPool> mThumbnailPool;

    /** The camera actor for the current cut. */
    TWeakObjectPtr<AActor> mBoardCamera;

    /** Delegate binding handle for ISequencer::OnCameraCut */
    FDelegateHandle mOnCameraCutHandle;
};
