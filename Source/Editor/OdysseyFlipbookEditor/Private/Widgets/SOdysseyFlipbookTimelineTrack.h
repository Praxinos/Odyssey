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
		SLATE_ARGUMENT( TSharedPtr<FOdysseyFlipbookWrapper>, FlipbookWrapper )
        SLATE_ATTRIBUTE( float, FrameSize )
		SLATE_EVENT( FOnFlipbookChanged, OnFlipbookChanged)
        SLATE_EVENT( FOnKeyframeRemoved, OnKeyframeRemoved)
		SLATE_EVENT( FOnKeyframeAdded, OnKeyframeAdded)
		SLATE_EVENT( FOnSpriteCreated, OnSpriteCreated)
		SLATE_EVENT( FOnTextureCreated, OnTextureCreated)
	SLATE_END_ARGS()

	
	~SOdysseyFlipbookTimelineTrack();

	// Construct the widget
	void Construct( const FArguments& InArgs );
	void BindCommands(const TSharedRef<FUICommandList>& iCommandList);

public:
	//Accessors
	const TSharedPtr<FOdysseyFlipbookWrapper>& FlipbookWrapper() const;
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
	void ShowKeyFrameSpriteInContentBrowser(int32 iIndex);
	// void EditSpriteForKeyFrame(int32 iIndex);
	void AddNewKeyframe(int32 iIndex);

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
	
	void OpenSpritePickerMenu(FMenuBuilder& MenuBuilder, int32 iIndex);
	void CloseMenu();
	void OnAssetSelected(const FAssetData& AssetData, int32 iFrameIndex);
    void OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture);

private:
	TSharedPtr<FOdysseyFlipbookWrapper> mFlipbookWrapper;
    TAttribute<float> 	mFrameSize;
	TSharedPtr<SOdysseyTimelineFrameList> mFrameList;
	TSharedPtr<FAssetThumbnailPool> mAssetThumbnailPool;
	TSharedPtr<FUICommandList> mFrameCommandList; //List of commands we can use on a frame
	/* FOnStructureChanged mOnStructureChanged; */
	FOnFlipbookChanged mOnFlipbookChanged;
    FOnKeyframeRemoved mOnKeyframeRemoved;
	FOnKeyframeRemoved mOnKeyframeAdded;
	FOnSpriteCreated mOnSpriteCreated;
	FOnTextureCreated mOnTextureCreated;

	const FSlateBrush* mFrameWarningBrush;
	FDelegateHandle mOnSpriteTextureChangedHandle;
};

