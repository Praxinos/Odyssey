// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyFlipbookTimelineTrack.h"

#include "IAssetTools.h"
#include "Fonts/FontMeasure.h"
#include "SOdysseyFlipbookTimelineFrameList.h"
#include "SOdysseyFlipbookTimelineFrame.h"
#include "OdysseyPainterEditorFlipbookCommands.h"

#include "Widgets/Layout/SScaleBox.h"
#include "PaperImporterSettings.h"
#include "PaperSprite.h"
#include "Framework/Commands/GenericCommands.h"
#include "PropertyCustomizationHelpers.h"
#include "OdysseyPainterEditorFlipbookListener.h"
#include "OdysseyPainterEditorFlipbookUtils.h"
#include "OdysseyStyle.h"

#define LOCTEXT_NAMESPACE "FlipbookEditor"

SOdysseyFlipbookTimelineTrack::~SOdysseyFlipbookTimelineTrack()
{
}

void SOdysseyFlipbookTimelineTrack::Construct( const SOdysseyFlipbookTimelineTrack::FArguments& InArgs )
{
    mFlipbook = InArgs._Flipbook;
    mFrameSize = InArgs._FrameSize;
    mOnFlipbookChanged = InArgs._OnFlipbookChanged;
    mOnKeyframeRemoved = InArgs._OnKeyframeRemoved;
    mOnKeyframeAdded = InArgs._OnKeyframeAdded;
    mOnSpriteCreated = InArgs._OnSpriteCreated;
    mOnTextureCreated = InArgs._OnTextureCreated;

    mListener = MakeUnique<FOdysseyPainterEditorFlipbookListener>(mFlipbook);
    mListener->OnSpriteTextureChanged().AddRaw(this, &SOdysseyFlipbookTimelineTrack::OnSpriteTextureChanged);

    mFrameWarningBrush = FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameWarning");

    ChildSlot
    [
        //Frames
        SAssignNew(mFrameList, SOdysseyFlipbookTimelineFrameList)
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
    if (!mFlipbook)
        return;

    mFrameList->RemoveAllFrames();
    mAssetThumbnailPool = MakeShareable( new FAssetThumbnailPool(1024, true) );

    for (int32 index = 0; index < mFlipbook->GetNumKeyFrames(); ++index)
    {
        const FPaperFlipbookKeyFrame& keyframe = mFlipbook->GetKeyFrameChecked(index);
        AddFrame(OdysseyPainterEditorFlipbookUtils::GetKeyframeTexture(mFlipbook, index), keyframe.FrameRun);
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
        ThumbnailConfig.AllowAssetSpecificThumbnailOverlay = false;
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

void
SOdysseyFlipbookTimelineTrack::DeleteFrame(int32 iIndex)
{
    //Remove from GUI
    mFrameList->RemoveFrameAt(iIndex);

    //Remove from Data
    FPaperFlipbookKeyFrame keyframe = mFlipbook->GetKeyFrameChecked(iIndex);

    OdysseyPainterEditorFlipbookUtils::RemoveKeyFrame(mFlipbook, iIndex);

    mOnKeyframeRemoved.ExecuteIfBound(keyframe);
    mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::DuplicateFrame(int32 iIndex)
{
    //Remove from Data
    UTexture2D* createdTexture = NULL;
    UPaperSprite* createdSprite = NULL;
    if (!OdysseyPainterEditorFlipbookUtils::DuplicateKeyFrame(mFlipbook, iIndex, &createdTexture, &createdSprite))
        return;

    FPaperFlipbookKeyFrame keyframe = mFlipbook->GetKeyFrameChecked(iIndex + 1);
    InsertFrame(iIndex + 1, createdTexture, keyframe.FrameRun);

    mOnKeyframeAdded.ExecuteIfBound(keyframe);
    mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::ShowKeyFrameSpriteInContentBrowser(int32 iIndex)
{
    OdysseyPainterEditorFlipbookUtils::ShowKeyFrameSpriteInContentBrowser(mFlipbook, iIndex);
}

void
SOdysseyFlipbookTimelineTrack::OnFramesMoved(TArray<int32> iSrcIndexes, int32 iDstIndex)
{
    //Move in Data
    OdysseyPainterEditorFlipbookUtils::MoveKeyFrames(mFlipbook, iSrcIndexes, iDstIndex);

    mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::OnFramesLengthChanged(TArray<int32> iFrameIndexes)
{
    //Change in Data
    {
        FScopedFlipbookMutator mutator(mFlipbook);
        for( int i = 0; i < iFrameIndexes.Num(); i++)
        {
            OdysseyPainterEditorFlipbookUtils::SetKeyFrameLength(mFlipbook, iFrameIndexes[i], mFrameList->GetFrameLength(iFrameIndexes[i]));
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
    const FOdysseyPainterEditorFlipbookCommands& flipbookCommands = FOdysseyPainterEditorFlipbookCommands::Get();
    TSharedPtr<FUICommandList> frameCommandList = MakeShareable(new FUICommandList());
    frameCommandList->MapAction(FGenericCommands::Get().Duplicate, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::DuplicateFrame, iFrameIndex));
    frameCommandList->MapAction(FGenericCommands::Get().Delete, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::DeleteFrame, iFrameIndex));
    frameCommandList->MapAction(flipbookCommands.ShowSpriteInContentBrowser, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::ShowKeyFrameSpriteInContentBrowser, iFrameIndex));
    // frameCommandList->MapAction(flipbookCommands.EditSpriteForKeyFrame, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::EditSpriteForKeyFrame, iFrameIndex));
    frameCommandList->MapAction(flipbookCommands.AddNewKeyFrame, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::AddNewKeyframe, mFlipbook->GetNumKeyFrames()), FCanExecuteAction());
    frameCommandList->MapAction(flipbookCommands.AddNewKeyFrameBefore, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::AddNewKeyframe, iFrameIndex), FCanExecuteAction());
    frameCommandList->MapAction(flipbookCommands.AddNewKeyFrameAfter, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::AddNewKeyframe, iFrameIndex + 1), FCanExecuteAction());

    FMenuBuilder MenuBuilder(true, frameCommandList);
    {
        const FText KeyframeSectionTitle = LOCTEXT("timeline.context-menu.keyframe-section.name", "Keyframe Actions");
        MenuBuilder.BeginSection("KeyframeActions", KeyframeSectionTitle);
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().Duplicate);
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);

        MenuBuilder.AddMenuSeparator();

        MenuBuilder.AddMenuEntry(flipbookCommands.AddNewKeyFrameBefore);
        MenuBuilder.AddMenuEntry(flipbookCommands.AddNewKeyFrameAfter);

        MenuBuilder.EndSection();

        const FText SpriteSectionTitle = LOCTEXT("timeline.context-menu.sprite-section.name", "Sprite Actions");
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
    for (int i = 0; i < mFlipbook->GetNumKeyFrames(); i++)
    {
        UPaperSprite* sprite = OdysseyPainterEditorFlipbookUtils::GetKeyframeSprite(mFlipbook, i);
        if (sprite == iSprite)
        {
            SetFrame(i, texture);
        }
    }
}

void
SOdysseyFlipbookTimelineTrack::OpenSpritePickerMenu(FMenuBuilder& MenuBuilder, int32 iIndex)
{
    UPaperSprite* sprite = OdysseyPainterEditorFlipbookUtils::GetKeyframeSprite(mFlipbook, iIndex);
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
    OdysseyPainterEditorFlipbookUtils::SetKeyframeSprite(mFlipbook, iFrameIndex, sprite);
}

void
SOdysseyFlipbookTimelineTrack::AddNewKeyframe(int32 iIndex)
{
    UPaperSprite* sprite = NULL;
    UTexture2D* texture = NULL;
    FOdysseyTextureConfiguration textureConfiguration = mTextureConfiguration.Get();
    if (!OdysseyPainterEditorFlipbookUtils::CreateKeyFrame(mFlipbook, iIndex, &texture, &sprite, textureConfiguration))
        return;

    InsertFrame(iIndex, texture, mFlipbook->GetKeyFrameChecked(iIndex).FrameRun);
    mOnSpriteCreated.ExecuteIfBound(sprite);
    mOnTextureCreated.ExecuteIfBound(texture, textureConfiguration);
}

#undef LOCTEXT_NAMESPACE
