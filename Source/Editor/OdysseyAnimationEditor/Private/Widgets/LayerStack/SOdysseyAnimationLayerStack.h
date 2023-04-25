// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SOdysseyLayerStackTreeView.h"

/**
 * Implements the Animation Layer stack widget
 */

class UOdysseyAnimation;
class UOdysseyAnimationPlayer;
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerStack
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerStack)
        {}
        SLATE_ARGUMENT( UOdysseyAnimation*, Animation )
        SLATE_ARGUMENT( UOdysseyAnimationPlayer*, Player )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyAnimationLayerStack();
    SOdysseyAnimationLayerStack();
    
    void Construct(const FArguments& InArgs);

    UOdysseyAnimation* GetAnimation() const;
    UOdysseyAnimationPlayer* GetPlayer() const;
    TSharedPtr<SOdysseyLayerStackTreeView> GetTreeView() const;

    void TimelineZoomIn();
    void TimelineZoomOut();

	void SetTimelineZoom(float iZoom);
	void SetTimelineOffset(float iOffset);

	static float GetTimelineBaseFrameSize();
	float GetTimelineFrameWidth() const;
	float GetTimelineZoom() const;
	float GetTimelineOffset() const;

public:
    //Events
    FSimpleMulticastDelegate& OnTimelineOffsetChanged();

private:
    TSharedRef<ITableRow> OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable);
    void OnTimelineScrollBarScrolled(float iOffset);

private:
    UOdysseyAnimation* mAnimation;
    UOdysseyAnimationPlayer* mPlayer;
    TSharedPtr<SOdysseyLayerStackTreeView> mTreeView;
	float mTimelineZoom;
	float mTimelineOffset;
	TSharedPtr<SScrollBar> mTimelineScrollBar;
    FSimpleMulticastDelegate mOnTimelineOffsetChanged;
};
