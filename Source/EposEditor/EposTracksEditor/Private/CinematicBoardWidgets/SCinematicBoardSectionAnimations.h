// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Widgets/SCompoundWidget.h"

#include "ISequencer.h"

//---

class FCinematicBoardSection;

class EPOSTRACKSEDITOR_API SCinematicBoardSectionAnimations
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionAnimations )
        {}
        SLATE_ATTRIBUTE( EVisibility, OptionalWidgetsVisibility )
    SLATE_END_ARGS()

    virtual ~SCinematicBoardSectionAnimations();

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

    // SWidget overrides
    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

public:
    /** Called when our sequencer has changed moviescene data */
    void RebuildAnimationList( EMovieSceneDataChangeType iType );

protected:
    TSharedRef<ITableRow> MakeAnimationRow( TSharedRef<FMovieScenePossessable> iItem, const TSharedRef<STableViewBase>& iOwnerTable );

    TSharedRef<SWidget> MakeCreateAnimationMenu();
    void CreateAnimation( TSharedRef<FString> iAnimationName );

    void RebuildAnimationList();

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    TAttribute<EVisibility>             mOptionalWidgetsVisibility;

    TWeakPtr<ISequencer>                mSequencer;

private:
    TArray<TSharedRef<FMovieScenePossessable>> mPossessables;
    TSharedPtr<SListView<TSharedRef<FMovieScenePossessable>>> mWidgetAnimationList;

    bool mNeedRebuildAnimationList = true;

    /** Delegate binding handle for ISequencer::OnMovieSceneDataChanged */
    FDelegateHandle mRebuildAnimationListHandle;
};
