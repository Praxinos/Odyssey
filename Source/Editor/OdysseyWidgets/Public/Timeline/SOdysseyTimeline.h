// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "SOdysseyTimelineEvents.h"

class SOdysseyScrubWidget;

class ODYSSEYWIDGETS_API SOdysseyTimeline : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOdysseyTimeline)
		: _ScrubPosition(0.0f)
		, _Offset(0.0f)
		, _Zoom(1.0f)
	{}
		SLATE_DEFAULT_SLOT( FArguments, Content )
		SLATE_ARGUMENT(float, ScrubPosition)
		SLATE_ARGUMENT(float, Offset)
		SLATE_ARGUMENT(float, Zoom)
		SLATE_EVENT(FOnScrubStarted, OnScrubStarted)
		SLATE_EVENT(FOnScrubPositionChanged, OnScrubPositionChanged)
		SLATE_EVENT(FOnScrubStopped, OnScrubStopped)
		SLATE_EVENT(FOnOffsetChanged, OnOffsetChanged)
		SLATE_EVENT(FOnZoomChanged, OnZoomChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	// End of SWidget interface

public:
	void OnScrollBarUserScrolled(float ScrollOffset);

public:
	// 1.0 = 100%
	float Zoom() const;
	void Zoom(float iZoom);

	// Offset of the timeline in frames
	float Offset() const;
	void Offset(float iOffset);

	// ScrubPosition
	float ScrubPosition() const;
	void ScrubPosition(float iPosition);

	bool IsScrubbing() const;
	
	// Returns the framesize in pixels according to the zoom
	float FrameSize() const;

	FOptionalSize GetScrollBoxHWidth() const;;

private:
	//Zoom
	float 			mZoom;
	FOnZoomChanged 	mOnZoomChanged;

	//Offset
	float 				mOffset;
	FVector2D 			mOffsetMousePosition;
	bool 				mIsOffsetting;
	FOnOffsetChanged 	mOnOffsetChanged;

	//Scrub
	float 						mScrubPosition;
    bool 						mIsScrubbing;
    FOnScrubStarted 			mOnScrubStarted;
    FOnScrubPositionChanged 	mOnScrubPositionChanged;
    FOnScrubStopped 			mOnScrubStopped;

	TSharedPtr<SWidget> mContent;
	TSharedPtr<SScrollBar> mScrollBarH;
	TSharedPtr<SScrollBox> mScrollBoxH;
};
