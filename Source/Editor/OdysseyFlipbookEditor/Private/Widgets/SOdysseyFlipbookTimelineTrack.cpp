// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyFlipbookTimelineTrack.h"

#include "IAssetTools.h"
#include "Fonts/FontMeasure.h"
#include "SOdysseyTimelineFrameList.h"
#include "SOdysseyTimelineFrame.h"

#include "Widgets/Layout/SScaleBox.h"
#include "PaperImporterSettings.h"
#include "PaperSprite.h"
#include "Framework/Commands/GenericCommands.h"
//#include "FOdysseyFlipbookTimelineFrameDragDropOperation.h"

#define LOCTEXT_NAMESPACE "FlipbookTimelineTrack"

void SOdysseyFlipbookTimelineTrack::Construct( const SOdysseyFlipbookTimelineTrack::FArguments& InArgs )
{
	mFlipbook = InArgs._Flipbook;
    mFrameSize = InArgs._FrameSize;
	//mOnStructureChanged = InArgs._OnStructureChanged;
	mOnFlipbookChanged = InArgs._OnFlipbookChanged;
    mOnKeyframeRemoved = InArgs._OnKeyframeRemoved;

	mFrameWarningBrush = FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameWarning");

	ChildSlot
	[
		//Frames
		SAssignNew(mFrameList, SOdysseyTimelineFrameList)
		.FrameSize(mFrameSize)
		.OnFramesMoved(this, &SOdysseyFlipbookTimelineTrack::OnFramesMoved)
		.OnFramesLengthChanged(this, &SOdysseyFlipbookTimelineTrack::OnFramesLengthChanged)
		.OnFramesEditStart(this, &SOdysseyFlipbookTimelineTrack::OnFramesEditStart)
		.OnFramesEditStop(this, &SOdysseyFlipbookTimelineTrack::OnFramesEditStop)
		.OnFramesEditCancel(this, &SOdysseyFlipbookTimelineTrack::OnFramesEditCancel)
		.OnGenerateFrameContextMenu(this, &SOdysseyFlipbookTimelineTrack::OnGenerateFrameContextMenu)
		/* .OnFrameLengthChangeStart(InArgs._OnFrameLengthChangeStart)
		.OnFrameLengthChanged(InArgs._OnFrameLengthChanged)
		.OnFrameLengthChangeStop(InArgs._OnFrameLengthChangeStop)
		.OnFramesLengthChangeStart(InArgs._OnFramesLengthChangeStart)
		.OnFramesLengthChanged(InArgs._OnFramesLengthChanged)
		.OnFramesLengthChangeStop(InArgs._OnFramesLengthChangeStop)
		.OnDrop(this, &SOdysseyFlipbookTimelineTrack::OnFrameListDrop) */
	];

	Rebuild();
}

void
SOdysseyFlipbookTimelineTrack::Rebuild()
{
	mFrameList->RemoveAllFrames();
	
	UPaperFlipbook* flipbook = mFlipbook.Get();
	if (!flipbook)
		return;

	mAssetThumbnailPool = MakeShareable( new FAssetThumbnailPool(1024, true) );

	for (int32 index = 0; index < flipbook->GetNumKeyFrames(); ++index)
	{
		const FPaperFlipbookKeyFrame& keyframe = flipbook->GetKeyFrameChecked(index);
		int32 frameLength = keyframe.FrameRun;

		UTexture2D* texture = NULL;
		
		const UPaperSprite* sprite = keyframe.Sprite;
		if (sprite)
		{
			texture = sprite->GetSourceTexture();
		}

		AddFrame(texture, frameLength);
	}
}

void
SOdysseyFlipbookTimelineTrack::AddFrame(UTexture2D* iTexture, int32 iFrameLength)
{
	TSharedPtr<SWidget> contentWidget = CreateFrameContent(iTexture);

	/* TSharedPtr<SOdysseyTimelineFrame> frame = SNew(SOdysseyTimelineFrame)
		.FrameSize(mFrameSize)
		.MinLength(1)
		.Length(iFrameLength)
		.OnLengthChanged(this, &SOdysseyFlipbookTimelineTrack::OnFrameLengthChanged)
		.OnGenerateContextMenu(this, &SOdysseyFlipbookTimelineTrack::GenerateFrameContextMenu)
		.OnDrop(this, &SOdysseyFlipbookTimelineTrack::OnFrameDrop)
		.OnAcceptDrop(this, &SOdysseyFlipbookTimelineTrack::OnFrameAcceptDrop)
		.OnDragDetected(this, &SOdysseyFlipbookTimelineTrack::OnFrameDragDetected)
		[
			contentWidget.ToSharedRef()
		]; */

	int32 index = mFrameList->AddFrame();
	mFrameList->SetFrameLength(index, iFrameLength);
	mFrameList->SetFrameContent(index, contentWidget);
}

void
SOdysseyFlipbookTimelineTrack::SetFrame(int32 iIndex, UTexture2D* iTexture)
{
	/* TSharedPtr<SWidget> contentWidget = SNew(SScaleBox)
		.HAlign(HAlign_Left)
		.Stretch(EStretch::ScaleToFitY)
		[
			CreateFrameContent(iTexture).ToSharedRef()
		]; */

	TSharedPtr<SWidget> contentWidget = CreateFrameContent(iTexture);
	mFrameList->SetFrameContent(iIndex, contentWidget);
	// mFrameList->GetFrame(iIndex)->SetContent(contentWidget);
}

TSharedPtr<SWidget>
SOdysseyFlipbookTimelineTrack::CreateFrameContent(UTexture2D* iTexture)
{
	static const float thumbnailWidth = 40;
	static const float thumbnailHeight = 40;

	TSharedPtr<SWidget> contentWidget = nullptr;
	if (iTexture)
	{
		TSharedPtr<FAssetThumbnail> AssetThumbnail = MakeShareable( new FAssetThumbnail( iTexture, thumbnailWidth, thumbnailHeight, mAssetThumbnailPool ) );
		FAssetThumbnailConfig ThumbnailConfig;
		ThumbnailConfig.bAllowFadeIn = true;
		ThumbnailConfig.bAllowHintText = false;
		ThumbnailConfig.bForceGenericThumbnail = false;
		ThumbnailConfig.bAllowAssetSpecificThumbnailOverlay = false;
		ThumbnailConfig.ThumbnailLabel = EThumbnailLabel::NoLabel;
		ThumbnailConfig.HighlightedText = FText::FromString("");
		ThumbnailConfig.HintColorAndOpacity = FLinearColor( 1.0, 1.0, 1.0, 1.0 );
		ThumbnailConfig.AssetTypeColorOverride = FLinearColor(0.0, 0.0, 0.0, 0.0);
		TSharedPtr<SWidget> thumbnailWidget = AssetThumbnail->MakeThumbnailWidget(ThumbnailConfig);

		contentWidget = SNew(SScaleBox)
			.HAlign(HAlign_Left)
			.Stretch(EStretch::ScaleToFit)
			[
				thumbnailWidget.ToSharedRef()
			];
	}
	else
	{
		contentWidget = SNew(SBox)
		.WidthOverride(mFrameWarningBrush->ImageSize.X)
		.HeightOverride(mFrameWarningBrush->ImageSize.Y)
		[
			SNew(SScaleBox)
			.HAlign(HAlign_Left)
			.Stretch(EStretch::ScaleToFitY)
			[
				SNew(SImage)
				.Image(mFrameWarningBrush)
			]
		];
	}
	return contentWidget;
}

/* void
SOdysseyFlipbookTimelineTrack::OnFrameLengthChanged(TSharedPtr<SOdysseyTimelineFrame> iFrame)
{	
	UPaperFlipbook* flipbook = mFlipbook.Get();
	if (!flipbook)
		return;

	TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> metadata = iFrame->GetMetaData<FOdysseyTimelineFrameListFrameMetaData>();
	if (!metadata)
		return;

	FScopedFlipbookMutator mutator(flipbook);
	FPaperFlipbookKeyFrame& keyframe = mutator.KeyFrames[metadata->Index()];
	keyframe.FrameRun = iFrame->Length();
}

FReply
SOdysseyFlipbookTimelineTrack::GenerateFrameContextMenu(TSharedPtr<SOdysseyTimelineFrame> iFrame)
{
	TSharedPtr<FUICommandList> frameCommandList = MakeShareable(new FUICommandList());
	frameCommandList->MapAction(FGenericCommands::Get().Delete, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::DeleteFrame, iFrame));

	FMenuBuilder MenuBuilder(true, frameCommandList);
	{	
		const FText KeyframeSectionTitle = LOCTEXT("OdysseyTimelineFrameActionsHeader", "Keyframe Actions");
		MenuBuilder.BeginSection("KeyframeActions", KeyframeSectionTitle);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
		MenuBuilder.EndSection();
	}

	TSharedRef<SWidget> MenuContents = MenuBuilder.MakeWidget();
	FWidgetPath WidgetPath = iMouseEvent.GetEventPath() != nullptr ? *iMouseEvent.GetEventPath() : FWidgetPath();
	FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuContents, iMouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

	return FReply::Handled();
} */

float
SOdysseyFlipbookTimelineTrack::FrameSize() const
{
	return mFrameSize.Get();
}

UPaperFlipbook*
SOdysseyFlipbookTimelineTrack::Flipbook() const
{
	return mFlipbook.Get();
}

void
SOdysseyFlipbookTimelineTrack::DeleteFrame(int32 iIndex)
{
	UPaperFlipbook* flipbook = mFlipbook.Get();
	if (!flipbook)
		return;

	//Remove from GUI
	mFrameList->RemoveFrameAt(iIndex);

	FPaperFlipbookKeyFrame keyframe = flipbook->GetKeyFrameChecked(iIndex);

	//Remove from Data
	{
		FScopedFlipbookMutator mutator(flipbook);
		mutator.KeyFrames.RemoveAt(iIndex);
	}
	
	mOnKeyframeRemoved.ExecuteIfBound(keyframe);
	mOnFlipbookChanged.ExecuteIfBound();
}

/* FReply
SOdysseyFlipbookTimelineTrack::OnFrameListDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
	TSharedPtr<FDragDropOperation> operation = iDragDropEvent.GetOperation();
	if (!operation.IsValid())
	{
	}
	else if (operation->IsOfType<FAssetDragDropOp>())
	{
		const auto& AssetDragDropOp = StaticCastSharedPtr<FAssetDragDropOp>(operation);
		//@TODO: Handle asset inserts

		// 			OnAssetsDropped(*AssetDragDropOp);
		// 			bWasDropHandled = true;
	}
	else if (operation->IsOfType<FOdysseyFlipbookTimelineFrameDragDropOperation>())
	{
		const auto& FrameDragDropOp = StaticCastSharedPtr<FOdysseyFlipbookTimelineFrameDragDropOperation>(operation);
		
		UPaperFlipbook* flipbook = mFlipbook.Get();
		if (!flipbook)
			return FReply::Handled();

		TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> srcMetadata = FrameDragDropOp->Frame()->GetMetaData<FOdysseyTimelineFrameListFrameMetaData>();
		if (!srcMetadata)
			return FReply::Handled();

		int32 srcIndex = srcMetadata->Index();
		int32 destIndex = mFrameList->FrameCount();// flipbook->GetNumKeyFrames();

		//Move in GUI
		mFrameList->InsertFrameAt(FrameDragDropOp->Frame(), destIndex); //Move to the end

		//Move in Data
		{
			FScopedFlipbookMutator mutator(flipbook);
			FPaperFlipbookKeyFrame keyframe = mutator.KeyFrames[srcIndex];
			
			mutator.KeyFrames.RemoveAt(srcIndex);
			mutator.KeyFrames.Add(keyframe);
		}

		mOnStructureChanged.ExecuteIfBound();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

bool
SOdysseyFlipbookTimelineTrack::OnFrameAcceptDrop(TSharedPtr<SOdysseyTimelineFrame> iFrame, const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
	TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
	return Operation.IsValid() && Operation->IsOfType<FOdysseyFlipbookTimelineFrameDragDropOperation>();
}

FReply
SOdysseyFlipbookTimelineTrack::OnFrameDragDetected(TSharedPtr<SOdysseyTimelineFrame> iFrame, const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
	if (iMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		TSharedRef<FOdysseyFlipbookTimelineFrameDragDropOperation> operation = MakeShareable(new FOdysseyFlipbookTimelineFrameDragDropOperation(iFrame, mFrameList));
		return FReply::Handled().BeginDragDrop(operation);
	}

	return FReply::Unhandled();
}

FReply
SOdysseyFlipbookTimelineTrack::OnFrameDrop(TSharedPtr<SOdysseyTimelineFrame> iFrame, const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent, int32 iIndexOffset)
{
	TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
	if (!Operation.IsValid())
	{
	}
	else if (Operation->IsOfType<FAssetDragDropOp>())
	{
		const auto& AssetDragDropOp = StaticCastSharedPtr<FAssetDragDropOp>(Operation);
		//@TODO: Handle asset inserts

		// 			OnAssetsDropped(*AssetDragDropOp);
		// 			bWasDropHandled = true;
	}
	else if (Operation->IsOfType<FOdysseyFlipbookTimelineFrameDragDropOperation>())
	{
		const auto& FrameDragDropOp = StaticCastSharedPtr<FOdysseyFlipbookTimelineFrameDragDropOperation>(Operation);
		
		UPaperFlipbook* flipbook = mFlipbook.Get();
		if (!flipbook)
			return FReply::Handled();

		TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> srcMetadata = FrameDragDropOp->Frame()->GetMetaData<FOdysseyTimelineFrameListFrameMetaData>();
		if (!srcMetadata)
			return FReply::Handled();

		TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> destMetadata = iFrame->GetMetaData<FOdysseyTimelineFrameListFrameMetaData>();
		if (!destMetadata)
			return FReply::Handled();

		int32 srcIndex = srcMetadata->Index();
		int32 destIndex = destMetadata->Index() + iIndexOffset;

		//Move in GUI
		mFrameList->InsertFrameAt(FrameDragDropOp->Frame(), destIndex);

		//Move in Data
		{
			FScopedFlipbookMutator mutator(flipbook);
			FPaperFlipbookKeyFrame keyframe = mutator.KeyFrames[srcIndex];
			
			mutator.KeyFrames.RemoveAt(srcIndex);
			
			if (destIndex > srcIndex )
				destIndex--;
			
			mutator.KeyFrames.Insert(keyframe, destIndex);
		}

		mOnStructureChanged.ExecuteIfBound();
		return FReply::Handled();
	}
	return FReply::Unhandled();
} */

void
SOdysseyFlipbookTimelineTrack::OnFramesMoved(TArray<int32> iSrcIndexes, int32 iDstIndex)
{
	//Move in Data
	{
		FScopedFlipbookMutator mutator(mFlipbook.Get());

		//store keyframes in the given order
		int32 fixedDestIndex = iDstIndex;
		TArray<FPaperFlipbookKeyFrame> keyframes;
		for( int i = 0; i < iSrcIndexes.Num(); i++)
		{
			keyframes.Add(mutator.KeyFrames[iSrcIndexes[i]]);
			if (iSrcIndexes[i] < iDstIndex)
			{
				fixedDestIndex--;
			}
		}

		//remove given keyframes
		iSrcIndexes.Sort();
		for( int i = iSrcIndexes.Num() - 1; i >= 0; i--)
		{
			mutator.KeyFrames.RemoveAt(iSrcIndexes[i]);
		}
			
		//insert keyframes at their new place
		mutator.KeyFrames.Insert(keyframes, fixedDestIndex);
	}

	mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::OnFramesLengthChanged(TArray<int32> iFrameIndexes)
{
	//Change in Data
	{
		FScopedFlipbookMutator mutator(mFlipbook.Get());

		//store keyframes in the given order
		for( int i = 0; i < iFrameIndexes.Num(); i++)
		{
			mutator.KeyFrames[iFrameIndexes[i]].FrameRun = mFrameList->GetFrameLength(iFrameIndexes[i]);
		}
	}

	mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::OnFramesEditStart()
{
	//Nothing to do here
}

void
SOdysseyFlipbookTimelineTrack::OnFramesEditStop()
{
	mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::OnFramesEditCancel()
{
	//Nothing to do here
}

FReply
SOdysseyFlipbookTimelineTrack::OnGenerateFrameContextMenu(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent, int32 iFrameIndex)
{
	TSharedPtr<FUICommandList> frameCommandList = MakeShareable(new FUICommandList());
	frameCommandList->MapAction(FGenericCommands::Get().Delete, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::DeleteFrame, iFrameIndex));

	FMenuBuilder MenuBuilder(true, frameCommandList);
	{	
		const FText KeyframeSectionTitle = LOCTEXT("OdysseyTimelineFrameActionsHeader", "Keyframe Actions");
		MenuBuilder.BeginSection("KeyframeActions", KeyframeSectionTitle);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
		MenuBuilder.EndSection();
	}

	TSharedRef<SWidget> MenuContents = MenuBuilder.MakeWidget();
	FWidgetPath WidgetPath = iMouseEvent.GetEventPath() != nullptr ? *iMouseEvent.GetEventPath() : FWidgetPath();
	FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuContents, iMouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

	return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE