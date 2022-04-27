// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SCompoundWidget.h"

#include "ISequencer.h"

//---

class FCinematicBoardSection;

class EPOSTRACKSEDITOR_API SCinematicBoardSectionPlanes
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionPlanes )
        {}
        SLATE_ATTRIBUTE( EVisibility, OptionalWidgetsVisibility )
    SLATE_END_ARGS()

    virtual ~SCinematicBoardSectionPlanes();

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

    // SWidget overrides
    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

public:
    /** Called when our sequencer has changed moviescene data */
    void RebuildPlaneList( EMovieSceneDataChangeType iType );

protected:
    TSharedRef<ITableRow> MakePlaneRow( TSharedRef<FMovieScenePossessable> iItem, const TSharedRef<STableViewBase>& iOwnerTable );

    TSharedRef<SWidget> MakeCreatePlaneMenu();
    void CreatePlane( TSharedRef<FString> iPlaneName );

    void RebuildPlaneList();

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    TAttribute<EVisibility>             mOptionalWidgetsVisibility;

    TWeakPtr<ISequencer>                mSequencer;

private:
    TArray<TSharedRef<FMovieScenePossessable>> mPossessables;
    TSharedPtr<SListView<TSharedRef<FMovieScenePossessable>>> mWidgetPlaneList;

    bool mNeedRebuildPlaneList { true };

    /** Delegate binding handle for ISequencer::OnMovieSceneDataChanged */
    FDelegateHandle mRebuildPlaneListHandle;
};
