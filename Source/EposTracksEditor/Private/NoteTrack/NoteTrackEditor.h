// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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
    FKeyPropertyResult AddNewMasterNote( FFrameNumber KeyTime, FString iText, UMovieSceneNoteTrack* Track, int32 RowIndex );

    /** Delegate for AnimatablePropertyChanged in HandleAssetAdded for attached sounds */
    FKeyPropertyResult AddNewAttachedNote( FFrameNumber KeyTime, FString iText, UMovieSceneNoteTrack* Track, TArray<TWeakObjectPtr<UObject>> ObjectsToAttachTo );

private:
    /** Callback for executing the "Add Note Track" menu entry. */
    void HandleAddNoteTrackMenuEntryExecute();

    /** Callback for executing the "Add Note Track" menu entry on an actor */
    void HandleAddAttachedNoteTrackMenuEntryExecute( FMenuBuilder& MenuBuilder, TArray<FGuid> ObjectBindings );

    /** Note sub menu */
    TSharedRef<SWidget> BuildNoteSubMenu( FOnAssetSelected OnAssetSelected, FOnAssetEnterPressed OnAssetEnterPressed );

    /** Note sub menu */
    TSharedRef<SWidget> BuildNoteSubMenu2( FOnTextCommitted OnTextCommited );

    /** Note text commited */
    void OnNoteTextCommited( const FText& iText, ETextCommit::Type iType, UMovieSceneTrack* Track );

    /** Note asset selected */
    void OnNoteAssetSelected( const FAssetData& AssetData, UMovieSceneTrack* Track );

    /** Note asset enter pressed */
    void OnNoteAssetEnterPressed( const TArray<FAssetData>& AssetData, UMovieSceneTrack* Track );

    /** Attached Note asset selected */
    void OnAttachedNoteAssetSelected( const FAssetData& AssetData, TArray<FGuid> ObjectBindings );

    /** Attached Note asset enter pressed */
    void OnAttachedNoteEnterPressed( const TArray<FAssetData>& AssetData, TArray<FGuid> ObjectBindings );
};

//---

/**
 * Class for audio sections, handles drawing of all waveform previews.
 */
class FNoteSection
    : public ISequencerSection
    , public TSharedFromThis<FNoteSection>
{
public:
    /** Constructor. */
    FNoteSection( UMovieSceneSection& InSection, TWeakPtr<ISequencer> InSequencer );

    /** Virtual destructor. */
    virtual ~FNoteSection();

public:
    // ISequencerSection interface

    virtual UMovieSceneSection* GetSectionObject() override;
    virtual FText GetSectionTitle() const override;
    virtual FText GetSectionToolTip() const override;
    virtual float GetSectionHeight() const override;
    //virtual int32 OnPaintSection( FSequencerSectionPainter& Painter ) const override;
    //virtual void Tick( const FGeometry& AllottedGeometry, const FGeometry& ParentGeometry, const double InCurrentTime, const float InDeltaTime ) override;

private:
    /** The section we are visualizing. */
    UMovieSceneSection& Section;

    TWeakPtr<ISequencer> Sequencer;
};
