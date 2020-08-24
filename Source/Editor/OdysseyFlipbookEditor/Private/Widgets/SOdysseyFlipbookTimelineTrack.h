// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "SOdysseyFlipbookTimelineTrackEvents.h"
#include "PaperFlipbook.h"

class SOdysseyTimelineFrameList;
class SOdysseyTimelineFrame;

class SOdysseyFlipbookTimelineTrack : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOdysseyFlipbookTimelineTrack)
        {}
		SLATE_ATTRIBUTE( UPaperFlipbook*, Flipbook )
        SLATE_ATTRIBUTE( float, FrameSize )
        /* SLATE_EVENT( FOnFrameLengthChangeStart, OnFrameLengthChangeStart) //A specific frame timing changes
		SLATE_EVENT( FOnFrameLengthChanged, OnFrameLengthChanged)
		SLATE_EVENT( FOnFrameLengthChangeStop, OnFrameLengthChangeStop)
        SLATE_EVENT( FOnFramesLengthChangeStart, OnFramesLengthChangeStart) //One or more frames timing changes
		SLATE_EVENT( FOnFramesLengthChanged, OnFramesLengthChanged)
		SLATE_EVENT( FOnFramesLengthChangeStop, OnFramesLengthChangeStop)
		SLATE_EVENT( FOnStructureChanged, OnStructureChanged) */
		SLATE_EVENT( FOnFlipbookChanged, OnFlipbookChanged)
        SLATE_EVENT( FOnKeyframeRemoved, OnKeyframeRemoved)
		SLATE_EVENT( FOnKeyframeAdded, OnKeyframeAdded)
	SLATE_END_ARGS()

	// Construct the widget
	void Construct( const FArguments& InArgs );

public:
	//Accessors
	UPaperFlipbook* Flipbook() const;
	float FrameSize() const;

public:
	//Modifiers

	//Adds a new frame containing iTexture for iFrameLength time
	void AddFrame(UTexture2D* iTexture, int32 iFrameLength);

	//Inserts a new frame containing iTexture for iFrameLength time
	void InsertFrame(int32 iIndex, UTexture2D* iTexture, int32 iFrameLength);

	//Sets an existing frame to contain iTexture for iFrameLength time
	void SetFrame(int32 iIndex, UTexture2D* iTexture);

private:
	void Rebuild();
	TSharedPtr<SWidget> CreateFrameContent(UTexture2D* iTexture);
	void DuplicateFrame(int32 iIndex);
	void DeleteFrame(int32 iIndex);

private:
	//Events
	/* void OnFrameLengthChanged(TSharedPtr<SOdysseyTimelineFrame> iFrame);
	FReply OnFrameListDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent);
	FReply OnFrameDrop(TSharedPtr<SOdysseyTimelineFrame> iFrame, const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent, int32 iIndexOffset);
	FReply OnFrameDragDetected(TSharedPtr<SOdysseyTimelineFrame> iFrame, const FGeometry& iGeometry, const FPointerEvent& iMouseEvent);
	bool OnFrameAcceptDrop(TSharedPtr<SOdysseyTimelineFrame> iFrame, const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent);
	TSharedRef<SWidget> GenerateFrameContextMenu(TSharedPtr<SOdysseyTimelineFrame> iFrame); */

	
	void OnFramesMoved(TArray<int32> iSrcIndexes, int32 iDstIndexes);
	void OnFramesLengthChanged(TArray<int32> iFrameIndexes);
	void OnFramesEditStart();
	void OnFramesEditStop();
	void OnFramesEditCancel();
	FReply OnGenerateFrameContextMenu(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent, int32 iFrameIndex);

private:
	TAttribute<UPaperFlipbook*> mFlipbook;
    TAttribute<float> 	mFrameSize;
	TSharedPtr<SOdysseyTimelineFrameList> mFrameList;
	TSharedPtr<FAssetThumbnailPool> mAssetThumbnailPool;
	TSharedPtr<FUICommandList> mFrameCommandList; //List of commands we can use on a frame
	/* FOnStructureChanged mOnStructureChanged; */
	FOnFlipbookChanged mOnFlipbookChanged;
    FOnKeyframeRemoved mOnKeyframeRemoved;
	FOnKeyframeRemoved mOnKeyframeAdded;

	const FSlateBrush* mFrameWarningBrush;
};

