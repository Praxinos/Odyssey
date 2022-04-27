// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "Templates/SubclassOf.h"
#include "ISequencer.h"
#include "MovieSceneTrack.h"
#include "MovieSceneTrackEditor.h"
#include "ISequencerSection.h"
#include "ISequencerTrackEditor.h"
#include "IContentBrowserSingleton.h"

class UStoryNote;
class UMovieSceneNoteTrack;

/**
* A track editor for note.
*/
class FNoteTrackEditor
    : public FMovieSceneTrackEditor
{
public:
    /**
     * Creates and initializes a new instance.
     *
     * @param InSequencer The sequencer instance to be used by this tool.
     */
    FNoteTrackEditor(TSharedRef<ISequencer> InSequencer);

    /** Virtual destructor. */
    virtual ~FNoteTrackEditor();

public:
    /**
     * Factory function to create an instance of this class (called by a sequencer).
     *
     * @param InSequencer The sequencer instance to be used by this tool.
     * @return The new instance of this class.
     */
    static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> InSequencer);

public:
    // ISequencerTrackEditor interface

    virtual void OnInitialize() override;
    virtual void OnRelease() override;
    virtual void BuildAddTrackMenu( FMenuBuilder& MenuBuilder ) override;
    virtual void BuildObjectBindingTrackMenu( FMenuBuilder& MenuBuilder, const TArray<FGuid>& ObjectBindings, const UClass* ObjectClass ) override;
    virtual TSharedPtr<SWidget> BuildOutlinerEditWidget( const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params ) override;
    virtual bool HandleAssetAdded( UObject* Asset, const FGuid& TargetObjectGuid ) override;
    virtual TSharedRef<ISequencerSection> MakeSectionInterface( UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding ) override;
    virtual bool SupportsType( TSubclassOf<UMovieSceneTrack> Type ) const override;
    virtual bool SupportsSequence( UMovieSceneSequence* InSequence ) const override;
    virtual void BuildTrackContextMenu( FMenuBuilder& MenuBuilder, UMovieSceneTrack* Track ) override;
    virtual const FSlateBrush* GetIconBrush() const override;
    virtual bool IsResizable( UMovieSceneTrack* InTrack ) const override;
    virtual void Resize( float NewSize, UMovieSceneTrack* InTrack ) override;
    virtual bool OnAllowDrop( const FDragDropEvent& DragDropEvent, FSequencerDragDropParams& DragDropParams ) override;
    virtual FReply OnDrop( const FDragDropEvent& DragDropEvent, const FSequencerDragDropParams& DragDropParams ) override;

protected:
    /** Delegate for AnimatablePropertyChanged in HandleAssetAdded for master sounds */
    FKeyPropertyResult AddNewMasterNote( FFrameNumber KeyTime, UStoryNote* iNote, UMovieSceneNoteTrack* Track, int32 RowIndex );

    /** Delegate for AnimatablePropertyChanged in HandleAssetAdded for attached sounds */
    FKeyPropertyResult AddNewAttachedNote( FFrameNumber KeyTime, UStoryNote* iNote, UMovieSceneNoteTrack* Track, TArray<TWeakObjectPtr<UObject>> ObjectsToAttachTo );

    TRange<FFrameNumber> GetReferenceRange( FFrameNumber iFrame );

private:
    /** Callback for executing the "Add Note Track" menu entry. */
    void HandleAddNoteTrackMenuEntryExecute();

    /** Callback for executing the "Add Note Track" menu entry on an actor */
    void HandleAddAttachedNoteTrackMenuEntryExecute( FMenuBuilder& MenuBuilder, TArray<FGuid> ObjectBindings );

    /** Note sub menu */
    TSharedRef<SWidget> BuildNoteSubMenu( FOnAssetSelected OnAssetSelected, FOnAssetEnterPressed OnAssetEnterPressed, FOnTextCommitted OnTextCommited );

    /** Note text commited */
    void OnNoteTextCommited( const FText& iText, ETextCommit::Type iType, UMovieSceneTrack* Track );

    /** Note text commited */
    void OnAttachedNoteTextCommited( const FText& iText, ETextCommit::Type iType, TArray<FGuid> ObjectBindings );

    /** Note asset selected */
    void OnNoteAssetSelected( const FAssetData& AssetData, UMovieSceneTrack* Track );

    /** Note asset enter pressed */
    void OnNoteAssetEnterPressed( const TArray<FAssetData>& AssetData, UMovieSceneTrack* Track );

    /** Attached Note asset selected */
    void OnAttachedNoteAssetSelected( const FAssetData& AssetData, TArray<FGuid> ObjectBindings );

    /** Attached Note asset enter pressed */
    void OnAttachedNoteEnterPressed( const TArray<FAssetData>& AssetData, TArray<FGuid> ObjectBindings );
};
