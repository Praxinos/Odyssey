// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "SOdysseyTimelineFrameHandleEvents.h"

class ODYSSEYWIDGETS_API SOdysseyTimelineFrameHandle : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOdysseyTimelineFrameHandle)
	{}
		SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_ATTRIBUTE(float, FrameSize)
		SLATE_EVENT(FOnDragStarted, OnDragStarted)
		SLATE_EVENT(FOnDragged, OnDragged)
		SLATE_EVENT(FOnDragStopped, OnDragStopped)
	SLATE_END_ARGS()

	// Construct the widget
	void Construct(const FArguments& InArgs);

	//SWidget overrides
	virtual FReply OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;

public:
	float FrameSize() const;

private:
	TAttribute<float> 	mFrameSize;
	bool mIsDragging;
	FVector2D mDragScreenSpacePosition;

	FOnDragStarted mOnDragStarted;
	FOnDragged mOnDragged;
	FOnDragStopped mOnDragStopped;
};
