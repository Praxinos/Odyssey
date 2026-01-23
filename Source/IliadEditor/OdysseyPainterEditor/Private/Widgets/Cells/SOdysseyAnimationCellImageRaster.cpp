// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimation.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#define THUMBNAIL_SIZE 32

SOdysseyAnimationCellImageRaster::~SOdysseyAnimationCellImageRaster()
{
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
}

void
SOdysseyAnimationCellImageRaster::Construct(const FArguments& iArgs, UOdysseyAnimationCellImageRaster* iCell)
{
    mShowContent = iArgs._ShowContent;

    if (!iCell)
        return;

    mCell = iCell;
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddSP(this, &SOdysseyAnimationCellImageRaster::OnRenderingChanged);

    UOdysseyAnimation* animation = mCell->GetAnimation();
    float ratio = (float)animation->GetWidth() / (float)animation->GetHeight();

    mAssetThumbnail = MakeShareable(new FAssetThumbnail(mCell, (int)(ratio * THUMBNAIL_SIZE), THUMBNAIL_SIZE, UThumbnailManager::Get().GetSharedThumbnailPool()));
    FAssetThumbnailConfig thumbnailConfig;
    thumbnailConfig.bAllowFadeIn = true;
    thumbnailConfig.bAllowHintText = false;
    thumbnailConfig.bAllowRealTimeOnHovered = false;
    thumbnailConfig.bForceGenericThumbnail = false;
    thumbnailConfig.AllowAssetSpecificThumbnailOverlay = false;
    thumbnailConfig.ThumbnailLabel = EThumbnailLabel::NoLabel;
    thumbnailConfig.HighlightedText = FText::FromString("");
    thumbnailConfig.HintColorAndOpacity = FLinearColor( 0.f, 0.f, 0.f, 0.f);
    thumbnailConfig.AssetTypeColorOverride = FLinearColor(0.f, 0.f, 0.f, 0.f);
    TSharedRef<SWidget> thumbnailWidget = mAssetThumbnail->MakeThumbnailWidget(thumbnailConfig);

    ChildSlot
    .VAlign(VAlign_Center)
    .HAlign(HAlign_Left)
    [
        SNew(SHorizontalBox)
        .Clipping(EWidgetClipping::ClipToBoundsAlways)
        .Visibility(this, &SOdysseyAnimationCellImageRaster::GetContentVisibility)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            thumbnailWidget
        ]
    ];
}

void
SOdysseyAnimationCellImageRaster::OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent)
{
    if (!mCell)
        return;

    if (iEvent.IsInteractive())
        return;

    TArray<FGuid> composition = mCell->GetRenderingComposition(EOdysseyRenderingType::Render, 0);
    if (composition.Contains(iEvent.GetId()))
        mAssetThumbnail->RefreshThumbnail();
}

EVisibility
SOdysseyAnimationCellImageRaster::GetContentVisibility() const
{
    return mShowContent.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}
