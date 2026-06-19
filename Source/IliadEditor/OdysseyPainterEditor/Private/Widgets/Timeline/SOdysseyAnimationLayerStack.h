// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Widgets/SOdysseyLayerStackTreeView.h"
#include "TimelineTools/OdysseyAnimationTimelineTools.h"
#include "SOdysseyAnimationTimelineOutOfPegsKey.h"

/**
 * Implements the Animation Layer stack widget
 */

class SOdysseyLayerStackTreeView;
class SOdysseyAnimationTimelineTreeView;
class UOdysseyAnimationLayerStack;
class FOdysseyAnimationTimelineTool;
class FOdysseyPainterEditorAnimationTimelinePosition;
class UOdysseyAnimation;
class UOdysseyAnimationPlayer;
class FOdysseyPainterEditor;

class SOdysseyAnimationLayerStack
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
        SLATE_ATTRIBUTE( TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition )
        SLATE_ATTRIBUTE( FInt32Range, CustomValidRange)
        SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnActivateOutOfPegs, OnActivateOutOfPegs)
        SLATE_EVENT(FSimpleDelegate, OnInactivateOutOfPegs)
        SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnIsOutOfPegsChecked, OnIsOutOfPegsChecked)
        SLATE_EVENT(FSimpleDelegate, OnScrubStart)
        SLATE_EVENT(FSimpleDelegate, OnScrubEnd)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyAnimationLayerStack();
    SOdysseyAnimationLayerStack();

    void Construct(const FArguments& InArgs);
    TSharedPtr<SOdysseyLayerStackTreeView> GetTreeView() const;
    TSharedPtr<SOdysseyAnimationTimelineTreeView> GetTimelineTreeView() const;

private:
    virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent) override;
    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

private:
    void OnTimelineScrollBarHScrolled(float iOffset);
    void OnTreeViewScrolled(double iOffset);
    void OnTimelineTreeViewScrolled(double iOffset);
    void OnTransactCurrentFrame(TOptional<int> iFrame);
    void OnCurrentFrameChanged(int iFrame);
    void OnCurrentFrameCommited(int iFrame);
    void RebuildWidgets();

    void OnLayerAdded(UOdysseyLayer* iLayer);
    float PlaybackFramesPerSecond() const;

    int GetCurrentFrame() const;

private:
    TSlateAttribute<UOdysseyAnimation*> mAnimation;
    TAttribute<UOdysseyAnimationPlayer*> mPlayer;
    TAttribute<EVisibility> mPlayerControlsVisibility;
    TAttribute<EVisibility> mScrollbarVisibility;
    TAttribute<float> mPlaybackFramesPerSecond;
    TAttribute<TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>> mTimelinePosition;
    TAttribute<FInt32Range> mCustomValidRange;
    SOdysseyAnimationTimelineOutOfPegsKey::FOnActivateOutOfPegs mOnActivateOutOfPegs;
    FSimpleDelegate mOnInactivateOutOfPegs;
    SOdysseyAnimationTimelineOutOfPegsKey::FOnIsOutOfPegsChecked mOnIsOutOfPegsChecked;

    TSharedPtr<SOdysseyLayerStackTreeView> mTreeView;
    TSharedPtr<SOdysseyAnimationTimelineTreeView> mTimelineTreeView;
    TSharedPtr<SScrollBar> mTimelineScrollBarH;
    TSharedPtr<SScrollBar> mTimelineScrollBarV;
    TSharedPtr<SSplitter> mSplitter;
    FSimpleDelegate mOnScrubStart;
    FSimpleDelegate mOnScrubEnd;
};
