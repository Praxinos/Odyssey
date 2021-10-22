// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Widgets/SCompoundWidget.h"

#include "ISequencer.h"

//---

class FCinematicBoardSection;
class UStoryNote;

class EPOSTRACKSEDITOR_API SCinematicBoardSectionNotes
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionNotes )
        {}
        SLATE_ATTRIBUTE( EVisibility, OptionalWidgetsVisibility )
    SLATE_END_ARGS()

    virtual ~SCinematicBoardSectionNotes();

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

    // SWidget overrides
    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

public:
    /** Called when our sequencer has changed moviescene data */
    void RebuildNoteList( EMovieSceneDataChangeType iType );

protected:
    TSharedRef<ITableRow> MakeNoteRow( TWeakObjectPtr<UStoryNote> iItem, const TSharedRef<STableViewBase>& iOwnerTable );

    void RebuildNoteList();

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    TAttribute<EVisibility>             mOptionalWidgetsVisibility;

    TWeakPtr<ISequencer>                mSequencer;

private:
    TArray<TWeakObjectPtr<UStoryNote>> mNotes;
    TSharedPtr<SListView<TWeakObjectPtr<UStoryNote>>> mWidgetNoteList;

    bool mNeedRebuildNoteList { true };

    /** Delegate binding handle for ISequencer::OnMovieSceneDataChanged */
    FDelegateHandle mRebuildNoteListHandle;
};
