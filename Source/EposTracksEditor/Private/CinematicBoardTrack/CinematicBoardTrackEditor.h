// IDDN.FR.001.220036.001.S.P.2021.000.00000
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
#include "MovieSceneTrackEditor.h"

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
    : public FMovieSceneTrackEditor
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
    virtual void BuildAddTrackMenu( FMenuBuilder& ioMenuBuilder ) override;
    virtual TSharedPtr<SWidget> BuildOutlinerEditWidget( const FGuid& iObjectBinding, UMovieSceneTrack* iTrack, const FBuildEditWidgetParams& iParams ) override;
    virtual TSharedRef<ISequencerSection> MakeSectionInterface( UMovieSceneSection& ioSectionObject, UMovieSceneTrack& ioTrack, FGuid iObjectBinding ) override;
    virtual bool HandleAssetAdded( UObject* iAsset, const FGuid& iTargetObjectGuid ) override;
    virtual bool SupportsSequence( UMovieSceneSequence* iSequence ) const override;
    virtual bool SupportsType( TSubclassOf<UMovieSceneTrack> iType ) const override;
    virtual void Tick( float iDeltaTime ) override;
    virtual void BuildTrackContextMenu( FMenuBuilder& ioMenuBuilder, UMovieSceneTrack* iTrack ) override;
    virtual const FSlateBrush* GetIconBrush() const override;
    virtual bool OnAllowDrop( const FDragDropEvent& iDragDropEvent, FSequencerDragDropParams& DragDropParams ) override;
    virtual FReply OnDrop( const FDragDropEvent& iDragDropEvent, const FSequencerDragDropParams& DragDropParams ) override;

    /** Insert board. */
    void InsertBoard();

    /** Insert shot. */
    void InsertShot();

    /*
     * Duplicate board.
     *
     * @param Section The section to duplicate
     */
    //void DuplicateBoard( UMovieSceneCinematicBoardSection* iSection );

    /*
     * Duplicate board.
     *
     * @param Section The section to duplicate
     */
    //void CloneSection( UMovieSceneCinematicBoardSection* iSection );

    /*
     * Render board.
     *
     * @param Section The section to render
     */
    //void RenderBoards( const TArray<UMovieSceneCinematicBoardSection*>& Sections );

    /*
     * Rename board.
     *
     * @param Section The section to rename.
     */
    void RenameBoard( UMovieSceneCinematicBoardSection* iSection );

    /*
     * New take.
     *
     * @param Section The section to create a new take of.
     */
    //void NewTake( UMovieSceneCinematicBoardSection* iSection );

    /*
    * Switch take for the selected sections
    *
    * @param TakeObject The take object to switch to.
    */
    //void SwitchTake( UObject* iTakeObject );


private:

    /** Callback for determining whether the "Add Board" menu entry can execute. */
    bool HandleAddCinematicBoardTrackMenuEntryCanExecute() const;

    /** Callback for executing the "Add Board Track" menu entry. */
    void HandleAddCinematicBoardTrackMenuEntryExecute();

    /** Callback for generating the menu of the "Add Board" combo button. */
    TSharedRef<SWidget> HandleAddBoardComboButtonGetMenuContent();

    void SetArrangeSections( EArrangeSections iArrangeSections );
    bool IsArrangeSections( EArrangeSections iArrangeSections );

    /** Delegate for AnimatablePropertyChanged in AddKey */
    FKeyPropertyResult AddKeyInternal( FFrameNumber iKeyTime, UMovieSceneSequence* iMovieSceneSequence, int32 iRowIndex, TOptional<FFrameNumber> iDroppedFrame );

    /** Delegate for boards button lock state */
    ECheckBoxState AreBoardsLocked() const;

    /** Delegate for locked boards button */
    void OnLockBoardsClicked( ECheckBoxState iCheckBoxState );

    /** Delegate for boards button lock tooltip */
    FText GetLockBoardsToolTip() const;

    /**
     * Check whether the given sequence can be added as a sub-sequence.
     *
     * The purpose of this method is to disallow circular references
     * between sub-sequences in the focused movie scene.
     *
     * @param Sequence The sequence to check.
     * @return true if the sequence can be added as a sub-sequence, false otherwise.
     */
    bool CanAddSubSequence( const UMovieSceneSequence& iSequence ) const;

    /** Called when our sequencer wants to switch cameras */
    void OnUpdateCameraCut( UObject* iCameraObject, bool iJumpCut );

    /** Callback for AnimatablePropertyChanged in HandleAssetAdded. */
    FKeyPropertyResult HandleSequenceAdded( FFrameNumber iKeyTime, UMovieSceneSequence* iSequence, int32 iRowIndex );

private:

    /** The Thumbnail pool which draws all the viewport thumbnails for the board track. */
    TSharedPtr<FTrackEditorThumbnailPool> mThumbnailPool;

    /** The camera actor for the current cut. */
    TWeakObjectPtr<AActor> mBoardCamera;

    /** Delegate binding handle for ISequencer::OnCameraCut */
    FDelegateHandle mOnCameraCutHandle;
};
