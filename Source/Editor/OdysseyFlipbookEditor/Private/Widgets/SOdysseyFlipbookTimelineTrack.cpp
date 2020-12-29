// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyFlipbookTimelineTrack.h"

#include "IAssetTools.h"
#include "Fonts/FontMeasure.h"
#include "SOdysseyTimelineFrameList.h"
#include "SOdysseyTimelineFrame.h"
#include "OdysseyFlipbookEditorCommands.h"

#include "Widgets/Layout/SScaleBox.h"
#include "PaperImporterSettings.h"
#include "PaperSprite.h"
#include "Framework/Commands/GenericCommands.h"

#define LOCTEXT_NAMESPACE "FlipbookTimelineTrack"

SOdysseyFlipbookTimelineTrack::~SOdysseyFlipbookTimelineTrack()
{
	mFlipbookWrapper->OnSpriteTextureChanged().Remove(mOnSpriteTextureChangedHandle);
}

void SOdysseyFlipbookTimelineTrack::Construct( const SOdysseyFlipbookTimelineTrack::FArguments& InArgs )
{
	mFlipbookWrapper = InArgs._FlipbookWrapper;
    mFrameSize = InArgs._FrameSize;
	mOnFlipbookChanged = InArgs._OnFlipbookChanged;
    mOnKeyframeRemoved = InArgs._OnKeyframeRemoved;
	mOnKeyframeAdded = InArgs._OnKeyframeAdded;
	mOnSpriteCreated = InArgs._OnSpriteCreated;
	mOnTextureCreated = InArgs._OnTextureCreated;

	mOnSpriteTextureChangedHandle = mFlipbookWrapper->OnSpriteTextureChanged().AddRaw(this, &SOdysseyFlipbookTimelineTrack::OnSpriteTextureChanged);

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

	mAssetThumbnailPool = MakeShareable( new FAssetThumbnailPool(1024, true) );

	for (int32 index = 0; index < mFlipbookWrapper->Flipbook()->GetNumKeyFrames(); ++index)
	{
		const FPaperFlipbookKeyFrame& keyframe = mFlipbookWrapper->Flipbook()->GetKeyFrameChecked(index);
		AddFrame(mFlipbookWrapper->GetKeyframeTexture(index), keyframe.FrameRun);
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
		float w = iTexture->Source.GetSizeX();
		float h = iTexture->Source.GetSizeY();
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

const TSharedPtr<FOdysseyFlipbookWrapper>&
SOdysseyFlipbookTimelineTrack::FlipbookWrapper() const
{
	return mFlipbookWrapper;
}

void
SOdysseyFlipbookTimelineTrack::DeleteFrame(int32 iIndex)
{
	//Remove from GUI
	mFrameList->RemoveFrameAt(iIndex);

	//Remove from Data
	FPaperFlipbookKeyFrame keyframe = mFlipbookWrapper->Flipbook()->GetKeyFrameChecked(iIndex);

	mFlipbookWrapper->RemoveKeyFrame(iIndex);
	
	mOnKeyframeRemoved.ExecuteIfBound(keyframe);
	mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::DuplicateFrame(int32 iIndex)
{
	//Remove from Data
	UTexture2D* createdTexture = NULL;
	UPaperSprite* createdSprite = NULL;
	if (!mFlipbookWrapper->DuplicateKeyFrame(iIndex, &createdTexture, &createdSprite))
		return;

	//TODO: Instead of duplicating a keyframe directly, follow this:
	// - Create empty keyframe
	// - Move to the new keyframe
	// - Copy the previous keyframe content (layers and everything)
	//
	// This will ensure that any modifications on the source keyframe will be done before copying it.
	
	FPaperFlipbookKeyFrame keyframe = mFlipbookWrapper->Flipbook()->GetKeyFrameChecked(iIndex + 1);
	InsertFrame(iIndex + 1, createdTexture, keyframe.FrameRun);

	mOnKeyframeAdded.ExecuteIfBound(keyframe);
	mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::ShowKeyFrameSpriteInContentBrowser(int32 iIndex)
{
	mFlipbookWrapper->ShowKeyFrameSpriteInContentBrowser(iIndex);
}

/* void
SOdysseyFlipbookTimelineTrack::EditSpriteForKeyFrame(int32 iIndex)
{
	mFlipbookWrapper->OpenKeyFrameSpriteEditor(iIndex);
} */

void
SOdysseyFlipbookTimelineTrack::OnFramesMoved(TArray<int32> iSrcIndexes, int32 iDstIndex)
{
	//Move in Data
	mFlipbookWrapper->MoveKeyFrames(iSrcIndexes, iDstIndex);

	mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::OnFramesLengthChanged(TArray<int32> iFrameIndexes)
{
	//Change in Data
	{
		FScopedFlipbookMutator mutator(mFlipbookWrapper->Flipbook());
		for( int i = 0; i < iFrameIndexes.Num(); i++)
		{
			mFlipbookWrapper->SetKeyFrameLength(iFrameIndexes[i], mFrameList->GetFrameLength(iFrameIndexes[i]));
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
	const FOdysseyFlipbookEditorCommands& flipbookCommands = FOdysseyFlipbookEditorCommands::Get();
	TSharedPtr<FUICommandList> frameCommandList = MakeShareable(new FUICommandList());
	frameCommandList->MapAction(FGenericCommands::Get().Duplicate, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::DuplicateFrame, iFrameIndex));
	frameCommandList->MapAction(FGenericCommands::Get().Delete, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::DeleteFrame, iFrameIndex));
	frameCommandList->MapAction(flipbookCommands.ShowSpriteInContentBrowser, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::ShowKeyFrameSpriteInContentBrowser, iFrameIndex));
	// frameCommandList->MapAction(flipbookCommands.EditSpriteForKeyFrame, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::EditSpriteForKeyFrame, iFrameIndex));
	frameCommandList->MapAction(flipbookCommands.AddNewKeyFrame, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::AddNewKeyframe, mFlipbookWrapper->Flipbook()->GetNumKeyFrames()), FCanExecuteAction());
	frameCommandList->MapAction(flipbookCommands.AddNewKeyFrameBefore, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::AddNewKeyframe, iFrameIndex), FCanExecuteAction());
	frameCommandList->MapAction(flipbookCommands.AddNewKeyFrameAfter, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::AddNewKeyframe, iFrameIndex + 1), FCanExecuteAction());

	FMenuBuilder MenuBuilder(true, frameCommandList);
	{	
		const FText KeyframeSectionTitle = LOCTEXT("OdysseyTimelineFrameActionsHeader", "Keyframe Actions");
		MenuBuilder.BeginSection("KeyframeActions", KeyframeSectionTitle);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Duplicate);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);

		MenuBuilder.AddMenuSeparator();

		MenuBuilder.AddMenuEntry(flipbookCommands.AddNewKeyFrameBefore);
		MenuBuilder.AddMenuEntry(flipbookCommands.AddNewKeyFrameAfter);

		MenuBuilder.EndSection();

		const FText SpriteSectionTitle = LOCTEXT("OdysseyTimelineSpriteActionsHeader", "Sprite Actions");
		MenuBuilder.BeginSection("SpriteActions", SpriteSectionTitle);
		MenuBuilder.AddMenuEntry(flipbookCommands.ShowSpriteInContentBrowser);
		//MenuBuilder.AddMenuEntry(flipbookCommands.EditSpriteForKeyFrame);
		/* MenuBuilder.AddSubMenu(
			flipbookCommands.PickNewSpriteFrame->GetLabel(),
			flipbookCommands.PickNewSpriteFrame->GetDescription(),
			FNewMenuDelegate::CreateSP(this, &SOdysseyFlipbookTimelineTrack::OpenSpritePickerMenu, iFrameIndex)); */
		MenuBuilder.EndSection();
	}

	TSharedRef<SWidget> MenuContents = MenuBuilder.MakeWidget();
	FWidgetPath WidgetPath = iMouseEvent.GetEventPath() != nullptr ? *iMouseEvent.GetEventPath() : FWidgetPath();
	FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuContents, iMouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

	return FReply::Handled();
}

void
SOdysseyFlipbookTimelineTrack::OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture)
{
	UTexture2D* texture = iSprite->GetSourceTexture();

	UPaperFlipbook* flipbook = mFlipbookWrapper->Flipbook();
	for (int i = 0; i < flipbook->GetNumKeyFrames(); i++)
	{
		UPaperSprite* sprite = mFlipbookWrapper->GetKeyframeSprite(i);
		if (sprite == iSprite)
		{
			SetFrame(i, texture);
		}
	}	
}

void
SOdysseyFlipbookTimelineTrack::OpenSpritePickerMenu(FMenuBuilder& MenuBuilder, int32 iIndex)
{
	UPaperSprite* sprite = mFlipbookWrapper->GetKeyframeSprite(iIndex);
	if (!sprite)
		return;
	
	FAssetData CurrentAssetData(sprite);

	const bool bAllowClear = true;
	TArray<const UClass*> AllowedClasses;
	AllowedClasses.Add(UPaperSprite::StaticClass());

	TSharedRef<SWidget> AssetPickerWidget = PropertyCustomizationHelpers::MakeAssetPickerWithMenu(CurrentAssetData,
		bAllowClear,
		AllowedClasses,
		PropertyCustomizationHelpers::GetNewAssetFactoriesForClasses(AllowedClasses),
		FOnShouldFilterAsset(),
		FOnAssetSelected::CreateSP(this, &SOdysseyFlipbookTimelineTrack::OnAssetSelected, iIndex),
		FSimpleDelegate::CreateSP(this, &SOdysseyFlipbookTimelineTrack::CloseMenu));

	MenuBuilder.AddWidget(AssetPickerWidget, FText::GetEmpty(), /*bNoIndent=*/ true);
}

void
SOdysseyFlipbookTimelineTrack::CloseMenu()
{
	FSlateApplication::Get().DismissAllMenus();
}

void
SOdysseyFlipbookTimelineTrack::OnAssetSelected(const FAssetData& AssetData, int32 iFrameIndex)
{
	UPaperSprite* sprite = Cast<UPaperSprite>(AssetData.GetAsset());
	mFlipbookWrapper->SetKeyframeSprite(iFrameIndex, sprite);

	//TODO: Call an equivalent to SpriteCreated or TextureCreated
}

void
SOdysseyFlipbookTimelineTrack::AddNewKeyframe(int32 iIndex)
{
	UPaperSprite* sprite = NULL;
	UTexture2D* texture = NULL;
	if (!mFlipbookWrapper->CreateKeyFrame(iIndex, &texture, &sprite))
		return;

	InsertFrame(iIndex, texture, mFlipbookWrapper->Flipbook()->GetKeyFrameChecked(iIndex).FrameRun);
    mOnSpriteCreated.ExecuteIfBound(sprite);
    mOnTextureCreated.ExecuteIfBound(texture);
}

void
SOdysseyFlipbookTimelineTrack::BindCommands(const TSharedRef<FUICommandList>& iCommandList)
{
	
}

#undef LOCTEXT_NAMESPACE