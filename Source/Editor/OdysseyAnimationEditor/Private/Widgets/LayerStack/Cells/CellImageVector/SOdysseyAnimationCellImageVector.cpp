// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Cells/CellImageVector/SOdysseyAnimationCellImageVector.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimation.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#define THUMBNAIL_SIZE 32

void
SOdysseyAnimationCellImageVector::Construct(const FArguments& iArgs, UOdysseyAnimationCellImageVector* iCell)
{
    mShowContent = iArgs._ShowContent;

    if (!iCell)
        return;

    mCell = iCell;
    mCell->OnThumbnailDirtied().AddSP(this, &SOdysseyAnimationCellImageVector::OnThumbnailDirtied);
    mCell->OnThumbnailChanged().AddSP(this, &SOdysseyAnimationCellImageVector::OnThumbnailChanged);

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
        .Visibility(this, &SOdysseyAnimationCellImageVector::GetContentVisibility)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            thumbnailWidget
        ]
    ];
}

void
SOdysseyAnimationCellImageVector::OnThumbnailDirtied()
{
    mAssetThumbnail->RefreshThumbnail();
}

void
SOdysseyAnimationCellImageVector::OnThumbnailChanged()
{
    mAssetThumbnail->RefreshThumbnail();
}

EVisibility
SOdysseyAnimationCellImageVector::GetContentVisibility() const
{
    return mShowContent.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}
