// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterCellSection.h"

void
SOdysseyAnimationLayerImageRasterCellSection::Construct(
    const FArguments& iArgs,
	TSharedPtr<SOdysseyAnimationLayerStack> iLayerStackWidget,
	TSharedPtr<FOdysseyAnimationCell> iCell
)
{
	mCell = iCell;

	SOdysseyAnimationTimelineSection::Construct(
		SOdysseyAnimationTimelineSection::FArguments(),
		iLayerStackWidget
	);

	TSharedPtr<SWidget> content = SNew(SBorder)
			.BorderImage(FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameBackground"))
			.BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f));

	SetContent(content);
}

FOptionalSize
SOdysseyAnimationLayerImageRasterCellSection::GetSectionWidth() const
{
    return mCell.Pin()->GetLength() * GetLayerStackWidget()->GetTimelineFrameWidth();
}

FOptionalSize
SOdysseyAnimationLayerImageRasterCellSection::GetSectionHeight() const
{
    return GetLayerStackWidget()->GetTimelineBaseFrameSize();
}