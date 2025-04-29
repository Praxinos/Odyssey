// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/Cells/CellImageRaster/SOdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimation.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#define THUMBNAIL_SIZE 32

void
SOdysseyAnimationCellImageRaster::Construct(const FArguments& iArgs, UOdysseyAnimationCellImageRaster* iCell)
{
    mShowContent = iArgs._ShowContent;

    if (!iCell)
        return;

    mCell = iCell;
    mCell->OnThumbnailDirtied().AddSP(this, &SOdysseyAnimationCellImageRaster::OnThumbnailDirtied);
    mCell->OnThumbnailChanged().AddSP(this, &SOdysseyAnimationCellImageRaster::OnThumbnailChanged);

    UOdysseyAnimation* animation = mCell->GetAnimation();
    float ratio = (float)animation->GetWidth() / (float)animation->GetHeight();

    mAssetThumbnail = MakeShareable(new FAssetThumbnail(mCell, (int)(ratio * THUMBNAIL_SIZE), THUMBNAIL_SIZE, UThumbnailManager::Get().GetSharedThumbnailPool()));
    FAssetThumbnailConfig thumbnailConfig;
    thumbnailConfig.bAllowFadeIn = false;
    thumbnailConfig.bAllowHintText = false;
    thumbnailConfig.bAllowRealTimeOnHovered = false;
    thumbnailConfig.bForceGenericThumbnail = false;
    thumbnailConfig.bAllowAssetSpecificThumbnailOverlay = false;
    thumbnailConfig.ThumbnailLabel = EThumbnailLabel::NoLabel;
    thumbnailConfig.HighlightedText = FText::FromString("");
    thumbnailConfig.HintColorAndOpacity = FLinearColor( 1.0, 1.0, 1.0, 1.0 );
    thumbnailConfig.AssetTypeColorOverride = FLinearColor(1.0, 1.0, 1.0, 1.0);
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
SOdysseyAnimationCellImageRaster::OnThumbnailDirtied()
{
    mAssetThumbnail->RefreshThumbnail();
}

void
SOdysseyAnimationCellImageRaster::OnThumbnailChanged()
{
    mAssetThumbnail->RefreshThumbnail();
}

EVisibility
SOdysseyAnimationCellImageRaster::GetContentVisibility() const
{
    return mShowContent.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}
