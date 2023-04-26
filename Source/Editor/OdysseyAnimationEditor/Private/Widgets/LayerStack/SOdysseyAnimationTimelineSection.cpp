// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineSection.h"

void
SOdysseyAnimationTimelineSection::Construct(
	const FArguments& iArgs, 
	TSharedPtr<SOdysseyAnimationLayerStack> iLayerStackWidget
)
{
    mWidthInFrames = iArgs._WidthInFrames;
    mHeightInScreenUnits = iArgs._HeightInScreenUnits;
    mLayerStackWidget = iLayerStackWidget;

	ChildSlot
	[
		SAssignNew(mBox, SBox)
		.WidthOverride_Raw(this, &SOdysseyAnimationTimelineSection::GetSectionWidth)
		.HeightOverride_Raw(this, &SOdysseyAnimationTimelineSection::GetSectionHeight)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
	];
}

TSharedPtr<SOdysseyAnimationLayerStack>
SOdysseyAnimationTimelineSection::GetLayerStackWidget() const
{
    return mLayerStackWidget.Pin();
}

FOptionalSize
SOdysseyAnimationTimelineSection::GetSectionWidth() const
{
	return mWidthInFrames.Get() * GetLayerStackWidget()->GetTimelineFrameWidth();
}

FOptionalSize
SOdysseyAnimationTimelineSection::GetSectionHeight() const
{
    return mHeightInScreenUnits.Get(); //GetLayerStackWidget()->GetTimelineBaseFrameSize();
}

void
SOdysseyAnimationTimelineSection::SetContent(TSharedPtr<SWidget> iContent)
{
	mBox->SetContent(iContent.ToSharedRef());
}