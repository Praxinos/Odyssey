// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "SOdysseyTimelineFrameListEvents.h"

class SOdysseyTimelineFrame;

class ODYSSEYWIDGETS_API FOdysseyTimelineFrameListFrameMetaData : public ISlateMetaData
{
public:
	SLATE_METADATA_TYPE(FOdysseyTimelineFrameListFrameMetaData, ISlateMetaData)

public:
	FOdysseyTimelineFrameListFrameMetaData(int32 iIndex);

public:
	int32 Index() const;
	void Index(int32 iIndex);
	
public:
	int32 mIndex;
};

class ODYSSEYWIDGETS_API SOdysseyTimelineFrameList : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SOdysseyTimelineFrameList)
        {}
        SLATE_ATTRIBUTE( float, FrameSize )
		SLATE_EVENT( FOnFramesMoved, OnFramesMoved)
		SLATE_EVENT( FOnFramesLengthChanged, OnFramesLengthChanged)
		SLATE_EVENT( FOnFramesEditStart, OnFramesEditStart)
		SLATE_EVENT( FOnFramesEditStop, OnFramesEditStop)
		SLATE_EVENT( FOnFramesEditCancel, OnFramesEditCancel)
		SLATE_EVENT( FOnGenerateFrameContextMenu, OnGenerateFrameContextMenu)

        /* SLATE_EVENT( FOnFrameLengthChangeStart, OnFrameLengthChangeStart) //A specific frame timing changes
		SLATE_EVENT( FOnFrameLengthChanged, OnFrameLengthChanged)
		SLATE_EVENT( FOnFrameLengthChangeStop, OnFrameLengthChangeStop)
        SLATE_EVENT( FOnFramesLengthChangeStart, OnFramesLengthChangeStart) //One or more frames timing changes
		SLATE_EVENT( FOnFramesLengthChanged, OnFramesLengthChanged)
		SLATE_EVENT( FOnFramesLengthChangeStop, OnFramesLengthChangeStop)
		SLATE_EVENT( FOnFrameGenerateContextMenu, OnFrameGenerateContextMenu)
		SLATE_EVENT( FOnDropOnFrame, OnDropOnFrame )
		SLATE_EVENT( FOnAcceptDropOnFrame, OnAcceptDropOnFrame )
		// SLATE_EVENT( FOnTimelineFrameDragDetected, OnFrameDragDetected )
		SLATE_EVENT( FOnAcceptDrop, OnAcceptDropOnFrameList)
		SLATE_EVENT( FOnDrop, OnDropOnFrameList) */
	SLATE_END_ARGS()

	// Construct the widget
	void Construct( const FArguments& InArgs );

	//SCompoundWidget overrides
	virtual void OnDragLeave(const FDragDropEvent& DragDropEvent);
	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
	virtual FReply OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent);
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const;

public:
	//Getters
	float GetFrameSize() const;
	int32 GetFrameCount() const;
	int32 GetFrameLength(int32 iIndex) const;
	TSharedPtr<SWidget> GetFrameContent(int32 iIndex) const;
	EVisibility GetFrameVisibility(int32 iIndex) const;
	EVisibility GetTimingHandleVisibility() const;
	EVisibility GetLengthHandleVisibility() const;
	FMargin GetTimingHandlePadding() const;
	FMargin GetLengthHandlePadding() const;
	

	// TSharedPtr<SOdysseyTimelineFrame>& GetFrame(int32 iIndex);
	// TSharedPtr<FOdysseyTimelineFrame> GetFrameAt(int32 iIndex) const;

public:
	//Setters / Modifiers
	void SetFrameLength(int32 iIndex, int32 iLength);
	void SetFrameContent(int32 iIndex, TSharedPtr<SWidget> iContent);
	void SetFrameVisibility(int32 iIndex, EVisibility iVisibility);

	int32 AddFrame();
	void InsertFrame(int32 iIndex);
	void MoveFrame(int32 iSrcIndex, int32 iDestIndex);
	void RemoveFrameAt(int32 iIndex);
	void RemoveAllFrames();

	// void InsertFrameAt(TSharedPtr<SOdysseyTimelineFrame>& iFrame, int32 iINdex);
	//void MoveFrame(int32 iSrcIndex, int32 iDstIndex);

private:
	// Private Event Handlers
	void OnLengthHandleDragStarted(TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData);
	void OnLengthHandleDragged(int32 iOffset, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData);
	void OnLengthHandleDragStopped(TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData);
	void OnTimingHandleDragStarted(TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData);
	void OnTimingHandleDragged(int32 iOffset, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData);
	void OnTimingHandleDragStopped(TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData);
	FReply OnFrameDrop( const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData);
	void OnFrameDragEnter( const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData);
	FReply OnFrameDragOver( const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData);
	void OnFrameDragLeave( const FDragDropEvent& DragDropEvent, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData);
	FReply OnFrameDragDetected( const FGeometry& iGeometry, const FPointerEvent& iMouseEvent, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData);
	FReply OnGenerateFrameContextMenu( const FGeometry& iGeometry, const FPointerEvent& iMouseEvent, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData);

private:
	// Private Utilities 
	TSharedPtr<SWidget> CreateFrameControlWidget(TSharedPtr<SOdysseyTimelineFrame>& iFrame);

	EVisibility DropPreviewLeftVisibility(TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData) const;
	EVisibility DropPreviewRightVisibility(TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData) const;

private:
	enum eDropPreviewAlignment
	{
		kDropPreviewAlignment_None = 0,
		kDropPreviewAlignment_Left,
		kDropPreviewAlignment_Right
	};

private:
	//Attributes
    TAttribute<float> mFrameSize;

	//Widgets
	TSharedPtr<SHorizontalBox> mFramesContainer;

	//Events
	FOnFramesMoved mOnFramesMoved;
	FOnFramesLengthChanged mOnFramesLengthChanged;
	FOnFramesEditStart mOnFramesEditStart;
	FOnFramesEditStop mOnFramesEditStop;
	FOnFramesEditCancel mOnFramesEditCancel;
	FOnGenerateFrameContextMenu mOnGenerateFrameContextMenu;
	
	//Internal
	TArray<TSharedPtr<SOdysseyTimelineFrame>> mFrames;
	int32 mFrameDragOverIndex;
	int32 mLengthOnDragStart;
	int32 mNextLengthOnDragStart;

	const FSlateBrush* mTimingHandleBrush;
	const FSlateBrush* mLengthHandleBrush;

	eDropPreviewAlignment mDropPreviewAlignment;
};

