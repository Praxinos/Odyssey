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
	mOnFlipbookChanged = InArgs._OnFlipbookChanged;
    mOnKeyframeRemoved = InArgs._OnKeyframeRemoved;
	mOnKeyframeAdded = InArgs._OnKeyframeAdded;

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

	FOdysseyFlipbookUtils flipbookUtils(flipbook);
	for (int32 index = 0; index < flipbook->GetNumKeyFrames(); ++index)
	{
		const FPaperFlipbookKeyFrame& keyframe = flipbook->GetKeyFrameChecked(index);
		AddFrame(flipbookUtils.GetKeyframeTexture(index), keyframe.FrameRun);
	}
}

void
SOdysseyFlipbookTimelineTrack::AddFrame(UTexture2D* iTexture, int32 iFrameLength)
{
	TSharedPtr<SWidget> contentWidget = CreateFrameContent(iTexture);

	int32 index = mFrameList->AddFrame();
	mFrameList->SetFrameLength(index, iFrameLength);
	mFrameList->SetFrameContent(index, contentWidget);
}

void
SOdysseyFlipbookTimelineTrack::InsertFrame(int32 iIndex, UTexture2D* iTexture, int32 iFrameLength)
{
	TSharedPtr<SWidget> contentWidget = CreateFrameContent(iTexture);

	mFrameList->InsertFrame(iIndex);
	mFrameList->SetFrameLength(iIndex, iFrameLength);
	mFrameList->SetFrameContent(iIndex, contentWidget);
}

void
SOdysseyFlipbookTimelineTrack::SetFrame(int32 iIndex, UTexture2D* iTexture)
{
	TSharedPtr<SWidget> contentWidget = CreateFrameContent(iTexture);
	mFrameList->SetFrameContent(iIndex, contentWidget);
}

TSharedPtr<SWidget>
SOdysseyFlipbookTimelineTrack::CreateFrameContent(UTexture2D* iTexture)
{
	TSharedPtr<SWidget> contentWidget = nullptr;
	if (iTexture)
	{
		float w = iTexture->GetSizeX();
		float h = iTexture->GetSizeY();
		if (w > h)
		{
			float ratio = 64.f / w;
			w = 64.f;
			h *= ratio;
		}
		else
		{
			float ratio = 64.f / h;
			h = 64.f;
			w *= ratio;
		}

		TSharedPtr<FAssetThumbnail> AssetThumbnail = MakeShareable( new FAssetThumbnail( iTexture, w, h, mAssetThumbnailPool ) );
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
			.VAlign(VAlign_Center)
			.Stretch(EStretch::ScaleToFitY)
			.StretchDirection(EStretchDirection::DownOnly)
			[
				SNew(SBox)
				.WidthOverride(w)
				.HeightOverride(h)
				[
					thumbnailWidget.ToSharedRef()
				]
			];
	}
	else
	{
		contentWidget = SNew(SScaleBox)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Stretch(EStretch::ScaleToFitY)
			.StretchDirection(EStretchDirection::DownOnly)
			[
				SNew(SBox)
				.WidthOverride(mFrameWarningBrush->ImageSize.X)
				.HeightOverride(mFrameWarningBrush->ImageSize.Y)
				[
					SNew(SImage)
					.Image(mFrameWarningBrush)
				]
			];
	}
	return contentWidget;
}


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

	//Remove from Data
	FPaperFlipbookKeyFrame keyframe = flipbook->GetKeyFrameChecked(iIndex);

	FOdysseyFlipbookUtils flipbookUtils(flipbook);
	flipbookUtils.RemoveKeyFrame(iIndex);
	
	mOnKeyframeRemoved.ExecuteIfBound(keyframe);
	mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::DuplicateFrame(int32 iIndex)
{
	UPaperFlipbook* flipbook = mFlipbook.Get();
	if (!flipbook)
		return;


	//Remove from Data
	FOdysseyFlipbookUtils flipbookUtils(flipbook);
	
	UTexture2D* createdTexture = NULL;
	UPaperSprite* createdSprite = NULL;
	if (!flipbookUtils.DuplicateKeyFrame(iIndex, &createdTexture, &createdSprite))
		return;
	
	FPaperFlipbookKeyFrame keyframe = flipbook->GetKeyFrameChecked(iIndex + 1);
	InsertFrame(iIndex + 1, createdTexture, keyframe.FrameRun);

	mOnKeyframeAdded.ExecuteIfBound(keyframe);
	mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::OnFramesMoved(TArray<int32> iSrcIndexes, int32 iDstIndex)
{
	//Move in Data
	FOdysseyFlipbookUtils flipbookUtils(mFlipbook.Get());
	flipbookUtils.MoveKeyFrames(iSrcIndexes, iDstIndex);

	mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::OnFramesLengthChanged(TArray<int32> iFrameIndexes)
{
	//Change in Data
	{
		FScopedFlipbookMutator mutator(mFlipbook.Get());
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
	frameCommandList->MapAction(FGenericCommands::Get().Duplicate, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::DuplicateFrame, iFrameIndex));
	frameCommandList->MapAction(FGenericCommands::Get().Delete, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::DeleteFrame, iFrameIndex));

	FMenuBuilder MenuBuilder(true, frameCommandList);
	{	
		const FText KeyframeSectionTitle = LOCTEXT("OdysseyTimelineFrameActionsHeader", "Keyframe Actions");
		MenuBuilder.BeginSection("KeyframeActions", KeyframeSectionTitle);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Duplicate);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
		MenuBuilder.EndSection();
	}

	TSharedRef<SWidget> MenuContents = MenuBuilder.MakeWidget();
	FWidgetPath WidgetPath = iMouseEvent.GetEventPath() != nullptr ? *iMouseEvent.GetEventPath() : FWidgetPath();
	FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuContents, iMouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

	return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE