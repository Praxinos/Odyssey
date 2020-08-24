// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyTimelineFrameList.h"

#include "DragAndDrop/AssetDragDropOp.h"
#include "Fonts/FontMeasure.h"

#include "SOdysseyTimelineFrame.h"
#include "SOdysseyDropWidget.h"
#include "FOdysseyTimelineFrameDragDropOperation.h"

#define LOCTEXT_NAMESPACE "OdysseyTimeline"

//---
//---
//---

FOdysseyTimelineFrameListFrameMetaData::FOdysseyTimelineFrameListFrameMetaData(int32 iIndex) :
	mIndex(iIndex)
{

}

int32
FOdysseyTimelineFrameListFrameMetaData::Index() const
{
	return mIndex;
}

void
FOdysseyTimelineFrameListFrameMetaData::Index(int32 iIndex)
{
	mIndex = iIndex;
}

//---
//---
//---

void SOdysseyTimelineFrameList::Construct( const SOdysseyTimelineFrameList::FArguments& InArgs )
{
	//Set Attributes
    mFrameSize = InArgs._FrameSize;

	//Set Events
	mOnFramesMoved = InArgs._OnFramesMoved;
	mOnFramesLengthChanged = InArgs._OnFramesLengthChanged;
	mOnFramesEditStart = InArgs._OnFramesEditStart;
	mOnFramesEditStop = InArgs._OnFramesEditStop;
	mOnFramesEditCancel = InArgs._OnFramesEditCancel;
	mOnGenerateFrameContextMenu = InArgs._OnGenerateFrameContextMenu;

	//Set Internals
	mTimingHandleBrush = FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameTimingHandle");
	mLengthHandleBrush = FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameLengthHandle");
	
	mDropPreviewAlignment = kDropPreviewAlignment_None;
	mFrameDragOverIndex = -1;

	//Construct Children
	ChildSlot
	.Padding(0.f, 0.f, 0.f, 0.f)
	[
		SAssignNew(mFramesContainer, SHorizontalBox)
	];
}

FVector2D
SOdysseyTimelineFrameList::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	//Add a half of the size of a handle to be able to see the last one it entirely
	FVector2D size = mFramesContainer->GetDesiredSize();
	size.X += FMath::Max(mLengthHandleBrush->ImageSize.X, mTimingHandleBrush->ImageSize.X) / 2;
	// size.Y += mTimingHandleBrush->ImageSize.Y / 2;
	return size;
}

FReply
SOdysseyTimelineFrameList::OnDragOver(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
	/*
	mFrameDragOverIndex = mFrames.Num() - 1;
	mDropPreviewAlignment = kDropPreviewAlignment_Right;
	return FReply::Unhandled(); */

	TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
	if (!Operation.IsValid())
	{
		mFrameDragOverIndex = -1;
		mDropPreviewAlignment = kDropPreviewAlignment_None;
		return FReply::Unhandled();
	}

	if (Operation->IsOfType<FOdysseyTimelineFrameDragDropOperation>())
	{
		//If there's no frame or only one frame (which is the one we are dragging), we can still drop but there's no preview
		if (mFrames.Num() <= 1)
		{
			mFrameDragOverIndex = -1;
			mDropPreviewAlignment = kDropPreviewAlignment_None;
			return FReply::Handled();
		}

		const auto& frameDragDropOp = StaticCastSharedPtr<FOdysseyTimelineFrameDragDropOperation>(Operation);

		TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> metadata = frameDragDropOp->Frame()->GetMetaData<FOdysseyTimelineFrameListFrameMetaData>();
		if (!metadata)
			return FReply::Handled();

		mFrameDragOverIndex = (metadata->Index() == mFrames.Num() - 1) ? mFrames.Num() - 2 : mFrames.Num() - 1;
		mDropPreviewAlignment = kDropPreviewAlignment_Right;
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void
SOdysseyTimelineFrameList::OnDragLeave(const FDragDropEvent& iDragDropEvent)
{
	mFrameDragOverIndex = -1;
	mDropPreviewAlignment = kDropPreviewAlignment_None;
}

FReply
SOdysseyTimelineFrameList::OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
	if (mFrameDragOverIndex <= 0)
		return FReply::Unhandled();

	TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> metadata = mFrames[mFrameDragOverIndex]->GetMetaData<FOdysseyTimelineFrameListFrameMetaData>();
	return OnFrameDrop(mFrames[mFrameDragOverIndex]->GetCachedGeometry(), iDragDropEvent, metadata);
}

//---
//---
//---


float
SOdysseyTimelineFrameList::GetFrameSize() const
{
	return mFrameSize.Get();
}

int32
SOdysseyTimelineFrameList::GetFrameCount() const
{
	return mFrames.Num();
}

int32
SOdysseyTimelineFrameList::GetFrameLength(int32 iIndex) const
{
	return mFrames[iIndex]->Length();
}

TSharedPtr<SWidget>
SOdysseyTimelineFrameList::GetFrameContent(int32 iIndex) const
{

	return mFrames[iIndex]->Content();
}

EVisibility
SOdysseyTimelineFrameList::GetFrameVisibility(int32 iIndex) const
{
	return mFramesContainer->GetChildren()->GetChildAt(iIndex)->GetVisibility();
	//return mFrames[iIndex]->GetVisibility();
}

//---
//---
//---

void
SOdysseyTimelineFrameList::SetFrameLength(int32 iIndex, int32 iLength)
{
	return mFrames[iIndex]->Length(iLength);
}

void
SOdysseyTimelineFrameList::SetFrameContent(int32 iIndex, TSharedPtr<SWidget> iContent)
{
	return mFrames[iIndex]->Content(iContent);
}

void
SOdysseyTimelineFrameList::SetFrameVisibility(int32 iIndex, EVisibility iVisibility)
{
	mFramesContainer->GetChildren()->GetChildAt(iIndex)->SetVisibility(iVisibility);
	//return mFrames[iIndex]->SetVisibility(iVisibility);
}

//---
//---
//---

int32
SOdysseyTimelineFrameList::AddFrame()
{	
	TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> metadata = MakeShareable(new FOdysseyTimelineFrameListFrameMetaData(mFrames.Num()));

	TSharedPtr<SOdysseyTimelineFrame> frame = SNew(SOdysseyTimelineFrame)
		.FrameSize(mFrameSize)
		.MinLength(1)
		.Length(1)
		.OnDragDetected(this, &SOdysseyTimelineFrameList::OnFrameDragDetected, metadata)
		.OnGenerateContextMenu(this, &SOdysseyTimelineFrameList::OnGenerateFrameContextMenu, metadata);

	frame->AddMetadata(metadata.ToSharedRef());

	mFrames.Add(frame);
	mFramesContainer->AddSlot()
	.Padding(0.f, 0.f, 0.f, 0.f)
	.AutoWidth()
	[
		CreateFrameControlWidget(frame).ToSharedRef()
	];

	return mFrames.Num() - 1;
}

//---
//---
//---

void
SOdysseyTimelineFrameList::InsertFrame(int32 iIndex)
{	
	TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> metadata = MakeShareable(new FOdysseyTimelineFrameListFrameMetaData(iIndex));

	TSharedPtr<SOdysseyTimelineFrame> frame = SNew(SOdysseyTimelineFrame)
		.FrameSize(mFrameSize)
		.MinLength(1)
		.Length(1)
		.OnDragDetected(this, &SOdysseyTimelineFrameList::OnFrameDragDetected, metadata)
		.OnGenerateContextMenu(this, &SOdysseyTimelineFrameList::OnGenerateFrameContextMenu, metadata);

	frame->AddMetadata(metadata.ToSharedRef());

	mFrames.Insert(frame, iIndex);
	mFramesContainer->InsertSlot(iIndex)
	.Padding(0.f, 0.f, 0.f, 0.f)
	.AutoWidth()
	[
		CreateFrameControlWidget(frame).ToSharedRef()
	];

	for (int i = iIndex; i < mFrames.Num(); i++)
	{
		metadata = mFrames[i]->GetMetaData<FOdysseyTimelineFrameListFrameMetaData>();
		metadata->Index(i);
	}
}


void
SOdysseyTimelineFrameList::MoveFrame(int32 iSrcIndex, int32 iDestIndex)
{
	int32 fixedDestIndex = (iSrcIndex < iDestIndex) ? iDestIndex - 1 : iDestIndex;

	TSharedPtr<SOdysseyTimelineFrame> frame = mFrames[iSrcIndex];

	//Remove frame
	mFramesContainer->RemoveSlot(mFramesContainer->GetChildren()->GetChildAt(iSrcIndex)); //Remove he last slot to avoid regenerating framedata indexes
	mFrames.RemoveAt(iSrcIndex);

	//Insert Frame
	mFrames.Insert(frame, fixedDestIndex);
	mFramesContainer->InsertSlot(fixedDestIndex)
	.Padding(0.f, 0.f, 0.f, 0.f)
	.AutoWidth()
	[
		CreateFrameControlWidget(frame).ToSharedRef()
	];

	for (int i = FMath::Min(iSrcIndex, fixedDestIndex); i < mFrames.Num(); i++)
	{
		TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> metadata = mFrames[i]->GetMetaData<FOdysseyTimelineFrameListFrameMetaData>();
		metadata->Index(i);
	}
}

void
SOdysseyTimelineFrameList::RemoveFrameAt(int32 iIndex)
{
	mFramesContainer->RemoveSlot(mFramesContainer->GetChildren()->GetChildAt(iIndex)); //Remove he last slot to avoid regenerating framedata indexes
	mFrames.RemoveAt(iIndex);

	for(int i = iIndex; i < mFrames.Num(); i++)
	{
		TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> metadata = mFrames[i]->GetMetaData<FOdysseyTimelineFrameListFrameMetaData>();
		metadata->Index(i);
	}
}

void
SOdysseyTimelineFrameList::RemoveAllFrames()
{
	mFramesContainer->ClearChildren();
	mFrames.Empty();
}

//---
//---
//---

void
SOdysseyTimelineFrameList::OnLengthHandleDragStarted(TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData)
{
	mLengthOnDragStart = mFrames[iMetaData->Index()]->Length();
	mOnFramesEditStart.ExecuteIfBound();
}

void
SOdysseyTimelineFrameList::OnLengthHandleDragged(int32 iOffset, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData)
{
	mFrames[iMetaData->Index()]->Length(mLengthOnDragStart + iOffset);
	TArray<int32> indexes;
	indexes.Add(iMetaData->Index());
	mOnFramesLengthChanged.ExecuteIfBound(indexes);
}

void
SOdysseyTimelineFrameList::OnLengthHandleDragStopped(TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData)
{
	mOnFramesEditStop.ExecuteIfBound();
}

void
SOdysseyTimelineFrameList::OnTimingHandleDragStarted(TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData)
{
	mLengthOnDragStart = mFrames[iMetaData->Index()]->Length();
	
	mOnFramesEditStart.ExecuteIfBound();

	if (iMetaData->Index() + 1 >= mFrames.Num())
	{
		return;
	}
	TSharedPtr<SOdysseyTimelineFrame>& nextFrame = mFrames[iMetaData->Index() + 1];
	mNextLengthOnDragStart = nextFrame->Length();
}

void
SOdysseyTimelineFrameList::OnTimingHandleDragged(int32 iOffset, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData)
{
	mFrames[iMetaData->Index()]->Length(mLengthOnDragStart + iOffset);
	
	TArray<int32> indexes;
	indexes.Add(iMetaData->Index());
	
	if (iMetaData->Index() + 1 < mFrames.Num())
	{
		TSharedPtr<SOdysseyTimelineFrame>& nextFrame = mFrames[iMetaData->Index() + 1];
		
		int32 offset = mFrames[iMetaData->Index()]->Length() - mLengthOnDragStart;
		nextFrame->Length(mNextLengthOnDragStart - offset);

		indexes.Add(iMetaData->Index() + 1);
	}
	
	mOnFramesLengthChanged.ExecuteIfBound(indexes);
}

void
SOdysseyTimelineFrameList::OnTimingHandleDragStopped(TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData)
{
	mOnFramesEditStop.ExecuteIfBound();
}



FReply
SOdysseyTimelineFrameList::OnFrameDrop( const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData)
{
	mFrameDragOverIndex = -1;
	mDropPreviewAlignment = kDropPreviewAlignment_None;

	TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
	if (!Operation.IsValid())
		return FReply::Unhandled();

	FVector2D screenPosition = iDragDropEvent.GetScreenSpacePosition();
	FVector2D widgetPosition = iGeometry.AbsoluteToLocal(screenPosition);
	float normalizedX = widgetPosition.X / iGeometry.GetLocalSize().X;

	int32 indexOffset = normalizedX > 0.5f ? 1 : 0;

	if (Operation->IsOfType<FAssetDragDropOp>())
	{
		const auto& AssetDragDropOp = StaticCastSharedPtr<FAssetDragDropOp>(Operation);
		//TODO: Here is the place were to manage drop of external assets
	}
	else if (Operation->IsOfType<FOdysseyTimelineFrameDragDropOperation>())
	{
		const auto& frameDragDropOp = StaticCastSharedPtr<FOdysseyTimelineFrameDragDropOperation>(Operation);

		TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> srcMetadata = frameDragDropOp->Frame()->GetMetaData<FOdysseyTimelineFrameListFrameMetaData>();
		if (!srcMetadata)
			return FReply::Handled();

		TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> destMetadata = iMetaData;

		int32 srcIndex = srcMetadata->Index();
		int32 destIndex = destMetadata->Index() + indexOffset;

		//Move in GUI
		SetFrameVisibility(srcIndex, EVisibility::Visible);
		MoveFrame(srcIndex, destIndex);

		TArray<int32> indexes;
		indexes.Add(srcIndex);
		mOnFramesMoved.ExecuteIfBound(indexes, destIndex);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void
SOdysseyTimelineFrameList::OnFrameDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData)
{
}

FReply
SOdysseyTimelineFrameList::OnFrameDragOver( const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData)
{
	TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
	if (!Operation.IsValid())
	{
		mFrameDragOverIndex = -1;
		mDropPreviewAlignment = kDropPreviewAlignment_None;
		return FReply::Unhandled();
	}

	if (Operation->IsOfType<FOdysseyTimelineFrameDragDropOperation>())
	{
		mFrameDragOverIndex = iMetaData->Index();

		FVector2D screenPosition = iDragDropEvent.GetScreenSpacePosition();
		FVector2D widgetPosition = iGeometry.AbsoluteToLocal(screenPosition);
		float normalizedX = widgetPosition.X / iGeometry.GetLocalSize().X;

		if (normalizedX > 0.5f)
		{
			mDropPreviewAlignment = kDropPreviewAlignment_Right;
		}
		else
		{
			mDropPreviewAlignment = kDropPreviewAlignment_Left;
		}
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void
SOdysseyTimelineFrameList::OnFrameDragLeave(const FDragDropEvent& DragDropEvent, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData)
{
	mDropPreviewAlignment = kDropPreviewAlignment_None;
	mFrameDragOverIndex = -1;
}

FReply
SOdysseyTimelineFrameList::OnFrameDragDetected( const FGeometry& iGeometry, const FPointerEvent& iMouseEvent, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData)
{
	if (iMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		TSharedRef<FOdysseyTimelineFrameDragDropOperation> operation = MakeShareable(new FOdysseyTimelineFrameDragDropOperation(mFrames[iMetaData->Index()], SharedThis(this)));
		return FReply::Handled().BeginDragDrop(operation);
	}

	return FReply::Unhandled();
}

FReply
SOdysseyTimelineFrameList::OnGenerateFrameContextMenu( const FGeometry& iGeometry, const FPointerEvent& iMouseEvent, TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetadata )
{
	if (!mOnGenerateFrameContextMenu.IsBound())
		return FReply::Unhandled();
	
	return mOnGenerateFrameContextMenu.Execute(iGeometry, iMouseEvent, iMetadata->Index());
}


//---
//---
//---

TSharedPtr<SWidget>
SOdysseyTimelineFrameList::CreateFrameControlWidget(TSharedPtr<SOdysseyTimelineFrame>& iFrame)
{
	static const float dropPreviewWidth = 20.0f;
	static const FLinearColor dropPreviewColor(0.2f, 0.2f, 1.0f, 0.3f);

	TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> metaData = iFrame->GetMetaData<FOdysseyTimelineFrameListFrameMetaData>();

	return SNew(SOdysseyDropWidget)
	.OnDrop(this, &SOdysseyTimelineFrameList::OnFrameDrop, metaData)
	.OnDragEnter(this, &SOdysseyTimelineFrameList::OnFrameDragEnter, metaData)
	.OnDragOver(this, &SOdysseyTimelineFrameList::OnFrameDragOver, metaData)
	.OnDragLeave(this, &SOdysseyTimelineFrameList::OnFrameDragLeave, metaData)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.Padding(0.f, 0.f, 0.f, 0.f)
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			.Padding(0.f, mTimingHandleBrush->ImageSize.Y/2, 0.f, 0.f)
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				[
					iFrame.ToSharedRef()
				]
				+ SOverlay::Slot()
				//.Padding(0.0f, 0.0f, -mLengthHandleBrush->ImageSize.X/2, 0.f)
				.Padding(TAttribute<FMargin>(this, &SOdysseyTimelineFrameList::GetTimingHandlePadding))
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.Visibility(this, &SOdysseyTimelineFrameList::GetLengthHandleVisibility)
					// .Padding(FFlipbookUIConstants::FramePadding)
					.WidthOverride(mLengthHandleBrush->ImageSize.X)
					.HeightOverride(mLengthHandleBrush->ImageSize.Y)
					[
						SNew(SOdysseyTimelineFrameHandle)
						.FrameSize(mFrameSize)
						.OnDragStarted(this, &SOdysseyTimelineFrameList::OnLengthHandleDragStarted, metaData)
						.OnDragged(this, &SOdysseyTimelineFrameList::OnLengthHandleDragged, metaData)
						.OnDragStopped(this, &SOdysseyTimelineFrameList::OnLengthHandleDragStopped, metaData)
						[
							SNew(SImage)
							.Image(mLengthHandleBrush)
						]
					]
				]
			]
			
			+SOverlay::Slot()
			//.Padding(0.0f, 0.0f, -mTimingHandleBrush->ImageSize.X/2, 0.f)
			.Padding(TAttribute<FMargin>(this, &SOdysseyTimelineFrameList::GetTimingHandlePadding))
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Top)
			[
				SNew(SBox)
				.Visibility(this, &SOdysseyTimelineFrameList::GetLengthHandleVisibility)
				.WidthOverride(mTimingHandleBrush->ImageSize.X)
				.HeightOverride(mTimingHandleBrush->ImageSize.Y)
				[
					SNew(SOdysseyTimelineFrameHandle)
					.FrameSize(mFrameSize)
					.OnDragStarted(this, &SOdysseyTimelineFrameList::OnTimingHandleDragStarted, metaData)
					.OnDragged(this, &SOdysseyTimelineFrameList::OnTimingHandleDragged, metaData)
					.OnDragStopped(this, &SOdysseyTimelineFrameList::OnTimingHandleDragStopped, metaData)
					[
						SNew(SImage)
						.Image(mTimingHandleBrush)
					]
				]
			]
		]
		
		+ SOverlay::Slot()
		.Padding(-dropPreviewWidth/2, 0.0f, 0.f, 0.f)
		.HAlign(HAlign_Left)
		[
			SNew(SBox)
			.WidthOverride(dropPreviewWidth)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Visibility(this, &SOdysseyTimelineFrameList::DropPreviewLeftVisibility, metaData)
			[
				SNew(SColorBlock)
				.Color(dropPreviewColor)
			]
		]
		+ SOverlay::Slot()
		.Padding(0.0f, 0.f, -dropPreviewWidth/2, 0.f)
		.HAlign(HAlign_Right)
		[
			SNew(SBox)
			.WidthOverride(dropPreviewWidth)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Visibility(this, &SOdysseyTimelineFrameList::DropPreviewRightVisibility, metaData)
			[
				SNew(SColorBlock)
				.Color(dropPreviewColor)
			]
		]
	];
}

EVisibility
SOdysseyTimelineFrameList::DropPreviewLeftVisibility(TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData) const
{
	if (mFrameDragOverIndex != iMetaData->Index() || mDropPreviewAlignment != kDropPreviewAlignment_Left)
		return EVisibility::Collapsed;

	return EVisibility::HitTestInvisible;
}

EVisibility
SOdysseyTimelineFrameList::DropPreviewRightVisibility(TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> iMetaData) const
{
	if (mFrameDragOverIndex != iMetaData->Index() || mDropPreviewAlignment != kDropPreviewAlignment_Right)
		return EVisibility::Collapsed;

	return EVisibility::HitTestInvisible;
}

EVisibility
SOdysseyTimelineFrameList::GetTimingHandleVisibility() const
{
	return (GetFrameSize() < mTimingHandleBrush->ImageSize.X / 2) ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility
SOdysseyTimelineFrameList::GetLengthHandleVisibility() const
{
	return (GetFrameSize() < mLengthHandleBrush->ImageSize.X / 2) ? EVisibility::Collapsed : EVisibility::Visible;
}


FMargin
SOdysseyTimelineFrameList::GetTimingHandlePadding() const
{
	return (GetFrameSize() < mTimingHandleBrush->ImageSize.X / 2) ?
		FMargin(0.0f, 0.0f, 0.f, 0.f) :
		FMargin(0.0f, 0.0f, -mTimingHandleBrush->ImageSize.X / 2, 0.f);
}

FMargin
SOdysseyTimelineFrameList::GetLengthHandlePadding() const
{
	return (GetFrameSize() < mTimingHandleBrush->ImageSize.X / 2) ?
		FMargin(0.0f, 0.0f, 0.f, 0.f) :
		FMargin(0.0f, 0.0f, -mTimingHandleBrush->ImageSize.X / 2, 0.f);
}

#undef LOCTEXT_NAMESPACE