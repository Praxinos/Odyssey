// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterCell.h"

void
SOdysseyAnimationLayerImageRasterCell::Construct(
    const FArguments& iArgs,
	TSharedPtr<FOdysseyAnimationCell> iCell
)
{
	mCell = iCell;

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameBackground"))
		.BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f))
	];
}
