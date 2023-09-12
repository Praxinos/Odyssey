// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerImageRasterTimeline"

SOdysseyAnimationLayerImageRasterTimeline::~SOdysseyAnimationLayerImageRasterTimeline()
{
}

SOdysseyAnimationLayerImageRasterTimeline::SOdysseyAnimationLayerImageRasterTimeline()
{
}

void
SOdysseyAnimationLayerImageRasterTimeline::Construct(
    const FArguments& InArgs,
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
)
{
    ensure(iAnimationLayerImageRaster);

    mExtension = iExtension;
    mAnimationLayerImageRaster = iAnimationLayerImageRaster;
    
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyAnimationCells, mExtension, mAnimationLayerImageRaster, mAnimationLayerImageRaster->GetCellsContainer())
            .OnCreateCell(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCreateCell)
            .OnCreateCellWidget(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCreateCellWidget)
            .OnBuildCellContextMenu(this, &SOdysseyAnimationLayerImageRasterTimeline::OnBuildCellContextMenu)
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyAnimationTimelineFrameSelector, mExtension)
            .Visibility(this, &SOdysseyAnimationLayerImageRasterTimeline::GetFrameSelectorVisibility)
        ]
    ];
}

TSharedRef<FOdysseyAnimationCell>
SOdysseyAnimationLayerImageRasterTimeline::OnCreateCell()
{
    UOdysseyAnimation* animation = mAnimationLayerImageRaster->GetAnimation();
    return FOdysseyAnimationCellImageRaster::Create(mAnimationLayerImageRaster, animation->Width(), animation->Height(), animation->Format());
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterTimeline::OnCreateCellWidget(TSharedPtr<FOdysseyAnimationCell> iCell)
{
    return SNew(SOdysseyAnimationLayerImageRasterCell);
}

EVisibility
SOdysseyAnimationLayerImageRasterTimeline::GetFrameSelectorVisibility() const
{
    bool isCurrentLayer = mAnimationLayerImageRaster->GetLayerStack()->CurrentLayer == mAnimationLayerImageRaster;
	return isCurrentLayer ? EVisibility::Visible : EVisibility::Hidden;
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnBuildCellContextMenu(FMenuBuilder& iMenuBuilder, int iFrame)
{
}

#undef LOCTEXT_NAMESPACE
