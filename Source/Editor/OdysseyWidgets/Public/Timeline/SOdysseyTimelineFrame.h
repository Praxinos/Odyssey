// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "SOdysseyTimelineFrameEvents.h"

class ODYSSEYWIDGETS_API SOdysseyTimelineFrame : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOdysseyTimelineFrame) :
		_FrameSize(64),
		_Length(0),
		_MinLength(-1),
		_MaxLength(-1)
		{}
		SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_ATTRIBUTE(float, FrameSize)
		SLATE_ARGUMENT(int32, Length)
		SLATE_ARGUMENT(int32, MinLength)
		SLATE_ARGUMENT(int32, MaxLength)
		SLATE_EVENT(FOnGenerateContextMenu, OnGenerateContextMenu)
		SLATE_EVENT(FOnDrop, OnDrop)
		SLATE_EVENT(FOnDragEnter, OnDragEnter)
		SLATE_EVENT(FOnDrop, OnDragOver)
		SLATE_EVENT(FOnDragLeave, OnDragLeave)
		SLATE_EVENT(FOnDragDetected, OnDragDetected)
	SLATE_END_ARGS()

	// Construct the widget
	void Construct(const FArguments& InArgs);
	virtual FReply OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent);
	virtual FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent);
	virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
	virtual void OnDragLeave(const FDragDropEvent& DragDropEvent);
	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
	virtual FReply OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent);
	virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent);

public:
	float FrameSize() const;

	int32 Length() const;
	void Length(int32 iLength);
	
	int32 MinLength() const;
	void MinLength(int32 iLength);
	
	int32 MaxLength() const;
	void MaxLength(int32 iLength);

	FOptionalSize GetWidthInPixels() const;

	void Content(TSharedPtr<SWidget> iWidget);
	TSharedPtr<SWidget> Content() const;

private:
	TSharedPtr<SBorder> mContentContainer;
	TAttribute<float> 	mFrameSize;
	int32 mLength;
	int32 mMinLength;
	int32 mMaxLength;

	FOnGenerateContextMenu mOnGenerateContextMenu;
	FOnDrop mOnDrop;
	FOnDragEnter mOnDragEnter;
	FOnDrop mOnDragOver;
	FOnDragLeave mOnDragLeave;
	FOnDragDetected mOnDragDetected;
};
