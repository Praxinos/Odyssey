// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

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

class AActor;
struct FAssetData;
class FMenuBuilder;
class FTrackEditorThumbnailPool;
class UMovieSceneBoardSection;
class UMovieSceneBoardTrack;
class UMovieSceneSubSection;

/**
 * Tools for boards.
 */
class FBoardTrackEditor
    : public FMovieSceneTrackEditor
{
public:

    /**
     * Constructor
     *
     * @param InSequencer The sequencer instance to be used by this tool.
     */
    FBoardTrackEditor( TSharedRef<ISequencer> InSequencer );

    /** Virtual destructor. */
    virtual ~FBoardTrackEditor()
    {
    }

    /**
     * Creates an instance of this class.  Called by a sequencer .
     *
     * @param OwningSequencer The sequencer instance to be used by this tool.
     * @return The new instance of this class.
     */
    static TSharedRef<ISequencerTrackEditor> CreateTrackEditor( TSharedRef<ISequencer> OwningSequencer );

    TWeakObjectPtr<AActor> GetBoardCamera() const
    {
        return BoardCamera;
    }

public:

    // ISequencerTrackEditor interface
    virtual void OnInitialize() override;
    virtual void OnRelease() override;
    virtual void BuildAddTrackMenu( FMenuBuilder& MenuBuilder ) override;
    virtual TSharedPtr<SWidget> BuildOutlinerEditWidget( const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params ) override;
    virtual TSharedRef<ISequencerSection> MakeSectionInterface( UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding ) override;
    virtual bool HandleAssetAdded( UObject* Asset, const FGuid& TargetObjectGuid ) override;
    virtual bool SupportsSequence( UMovieSceneSequence* InSequence ) const override;
    virtual bool SupportsType( TSubclassOf<UMovieSceneTrack> Type ) const override;
    virtual void Tick( float DeltaTime ) override;
    //virtual void BuildTrackContextMenu( FMenuBuilder& MenuBuilder, UMovieSceneTrack* Track ) override;
    virtual const FSlateBrush* GetIconBrush() const override;
    virtual bool OnAllowDrop( const FDragDropEvent& DragDropEvent, UMovieSceneTrack* Track, int32 RowIndex, const FGuid& TargetObjectGuid ) override;
    virtual FReply OnDrop( const FDragDropEvent& DragDropEvent, UMovieSceneTrack* Track, int32 RowIndex, const FGuid& TargetObjectGuid ) override;

    /*
     * Insert board.
     */
    void InsertBoard();

    /*
     * Insert filler.
     */
    void InsertFiller();

    /*
     * Duplicate board.
     *
     * @param Section The section to duplicate
     */
    void DuplicateBoard( UMovieSceneBoardSection* Section );

    /*
     * Render board.
     *
     * @param Section The section to render
     */
    void RenderBoard( UMovieSceneBoardSection* Section );

    /*
     * Rename board.
     *
     * @param Section The section to rename.
     */
    void RenameBoard( UMovieSceneBoardSection* Section );

    /*
     * New take.
     *
     * @param Section The section to create a new take of.
     */
    //void NewTake( UMovieSceneBoardSection* Section );

    /*
    * Switch take for the selected sections
    *
    * @param TakeObject The take object to switch to.
    */
    //void SwitchTake( UObject* TakeObject );

private:

    /*
     * Create board
     *
     * @param NewBoardName The new board name.
     * @param NewBoardStartTime The time to start the new board at.
     * @param BoardToDuplicate The board to duplicate.
     * @return The new board.
     */
    UMovieSceneSubSection* CreateBoardInternal( FString& NewBoardName, FFrameNumber NewBoardStartTime, UMovieSceneBoardSection* BoardToDuplicate = nullptr );


private:

    /** Callback for determining whether the "Add Board" menu entry can execute. */
    bool HandleAddBoardTrackMenuEntryCanExecute() const;

    /** Callback for executing the "Add Board Track" menu entry. */
    void HandleAddBoardTrackMenuEntryExecute();

    /** Callback for generating the menu of the "Add Board" combo button. */
    TSharedRef<SWidget> HandleAddBoardComboButtonGetMenuContent();

    /** Callback for executing a menu entry in the "Add Board" combo button. */
    void HandleAddBoardComboButtonMenuEntryExecute( const FAssetData& AssetData );

    /** Callback for executing a menu entry in the "Add Board" combo button when enter pressed. */
    void HandleAddBoardComboButtonMenuEntryEnterPressed( const TArray<FAssetData>& AssetData );

    /** Find or create a cinematic board track in the currently focused movie scene. */
    UMovieSceneBoardTrack* FindOrCreateBoardTrack();

    /** Delegate for AnimatablePropertyChanged in AddKey */
    FKeyPropertyResult AddKeyInternal( FFrameNumber KeyTime, UMovieSceneSequence* InMovieSceneSequence, int32 RowIndex );

    /** Delegate for boards button lock state */
    ECheckBoxState AreBoardsLocked() const;

    /** Delegate for locked boards button */
    void OnLockBoardsClicked( ECheckBoxState CheckBoxState );

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
    bool CanAddSubSequence( const UMovieSceneSequence& Sequence ) const;

    /** Called when our sequencer wants to switch cameras */
    void OnUpdateCameraCut( UObject* CameraObject, bool bJumpCut );

    /** Callback for AnimatablePropertyChanged in HandleAssetAdded. */
    FKeyPropertyResult HandleSequenceAdded( FFrameNumber KeyTime, UMovieSceneSequence* Sequence, int32 RowIndex );

private:

    /** The Thumbnail pool which draws all the viewport thumbnails for the board track. */
    TSharedPtr<FTrackEditorThumbnailPool> ThumbnailPool;

    /** The camera actor for the current cut. */
    TWeakObjectPtr<AActor> BoardCamera;

    /** Delegate binding handle for ISequencer::OnCameraCut */
    FDelegateHandle OnCameraCutHandle;
};
