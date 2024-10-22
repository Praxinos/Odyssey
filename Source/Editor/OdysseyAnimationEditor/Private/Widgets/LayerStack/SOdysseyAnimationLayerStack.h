// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SOdysseyLayerStackTreeView.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineTools.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineOutOfPegsKey.h"

/**
 * Implements the Animation Layer stack widget
 */

class SOdysseyLayerStackTreeView;
class SOdysseyAnimationTimelineTreeView;
class UOdysseyAnimationLayerStack;
class FOdysseyAnimationTimelineTool;
class FOdysseyAnimationEditorTimelinePosition;
class FOdysseyAnimationEditorTimelineCellSelection;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerStack
    : public SCompoundWidget
{
    SLATE_DECLARE_WIDGET(SOdysseyAnimationLayerStack, SCompoundWidget)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerStack)
		: _Animation(nullptr)
		, _PlayerControlsVisibility(EVisibility::Visible)
		, _ScrollbarVisibility(EVisibility::Visible)
		, _PlaybackFramesPerSecond(24.0f)
        {}
        SLATE_ATTRIBUTE( UOdysseyAnimation*, Animation )
		SLATE_ATTRIBUTE( UOdysseyAnimationPlayer*, Player )
		SLATE_ATTRIBUTE( EVisibility, PlayerControlsVisibility)
		SLATE_ATTRIBUTE( EVisibility, ScrollbarVisibility)
		SLATE_ATTRIBUTE( float, PlaybackFramesPerSecond )
		SLATE_ATTRIBUTE( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
		SLATE_ATTRIBUTE( TSharedPtr<FOdysseyAnimationEditorTimelineCellSelection>, TimelineCellSelection )
		SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnActivateOutOfPegs, OnActivateOutOfPegs)
		SLATE_EVENT(FSimpleDelegate, OnInactivateOutOfPegs)
		SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnIsOutOfPegsChecked, OnIsOutOfPegsChecked)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyAnimationLayerStack();
    SOdysseyAnimationLayerStack();
    
    void Construct(const FArguments& InArgs);
    TSharedPtr<SOdysseyLayerStackTreeView> GetTreeView() const;

private:
    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

private:
    TSharedRef<ITableRow> OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable);
    void OnTimelineScrollBarHScrolled(float iOffset);
	void OnTreeViewScrolled(double iOffset);
	void OnTimelineTreeViewScrolled(double iOffset);
    void RebuildWidgets();

    void OnLayerAdded(UOdysseyLayer* iLayer);
    float PlaybackFramesPerSecond() const;

	EOdysseyTimelineTool GetCurrentTool() const;
    void OnToolChecked(EOdysseyTimelineTool iTool, ECheckBoxState iState);

	int GetCurrentFrame() const;

private:
    TSlateAttribute<UOdysseyAnimation*> mAnimation;
	TAttribute<UOdysseyAnimationPlayer*> mPlayer;
	TAttribute<EVisibility> mPlayerControlsVisibility;
	TAttribute<EVisibility> mScrollbarVisibility;
	TAttribute<float> mPlaybackFramesPerSecond;
	TAttribute<TSharedPtr<FOdysseyAnimationEditorTimelinePosition>> mTimelinePosition;
	TAttribute<TSharedPtr<FOdysseyAnimationEditorTimelineCellSelection>> mTimelineCellSelection;
	SOdysseyAnimationTimelineOutOfPegsKey::FOnActivateOutOfPegs mOnActivateOutOfPegs;
	FSimpleDelegate mOnInactivateOutOfPegs;
	SOdysseyAnimationTimelineOutOfPegsKey::FOnIsOutOfPegsChecked mOnIsOutOfPegsChecked;

    TSharedPtr<SOdysseyLayerStackTreeView> mTreeView;
	TSharedPtr<SOdysseyAnimationTimelineTreeView> mTimelineTreeView;
	TSharedPtr<SScrollBar> mTimelineScrollBarH;
	TSharedPtr<SScrollBar> mTimelineScrollBarV;
	TSharedPtr<SSplitter> mSplitter;
};
