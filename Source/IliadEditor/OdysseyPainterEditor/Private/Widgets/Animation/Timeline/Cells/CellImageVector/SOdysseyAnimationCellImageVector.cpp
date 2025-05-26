// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Animation/Timeline/Cells/CellImageVector/SOdysseyAnimationCellImageVector.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimation.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#define THUMBNAIL_SIZE 32

SOdysseyAnimationCellImageVector::~SOdysseyAnimationCellImageVector()
{
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
}

void
SOdysseyAnimationCellImageVector::Construct(const FArguments& iArgs, UOdysseyAnimationCellImageVector* iCell)
{
    mShowContent = iArgs._ShowContent;

    if (!iCell)
        return;

    mCell = iCell;
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddSP(this, &SOdysseyAnimationCellImageVector::OnRenderingChanged);

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
SOdysseyAnimationCellImageVector::OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent)
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
SOdysseyAnimationCellImageVector::GetContentVisibility() const
{
    return mShowContent.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}
