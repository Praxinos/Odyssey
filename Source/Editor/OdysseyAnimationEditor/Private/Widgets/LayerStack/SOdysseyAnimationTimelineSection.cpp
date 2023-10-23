// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineSection.h"

void
SOdysseyAnimationTimelineSection::Construct(
	const FArguments& iArgs, 
	FOdysseyAnimationEditorExtension* iExtension
)
{
	mExtension = iExtension;
    mWidthInFrames = iArgs._WidthInFrames;
    mHeightInScreenUnits = iArgs._HeightInScreenUnits;

	ChildSlot
	[
		SAssignNew(mBox, SBox)
		.WidthOverride_Raw(this, &SOdysseyAnimationTimelineSection::GetSectionWidth)
		.HeightOverride_Raw(this, &SOdysseyAnimationTimelineSection::GetSectionHeight)
		.MinDesiredWidth(this, &SOdysseyAnimationTimelineSection::GetSectionWidth)
		.MinDesiredHeight(this, &SOdysseyAnimationTimelineSection::GetSectionHeight)
		.MaxDesiredWidth(this, &SOdysseyAnimationTimelineSection::GetSectionWidth)
		.MaxDesiredHeight(this, &SOdysseyAnimationTimelineSection::GetSectionHeight)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			iArgs._Content.Widget
		]
	];
}

FOdysseyAnimationEditorExtension*
SOdysseyAnimationTimelineSection::GetExtension() const
{
    return mExtension;
}

FOptionalSize
SOdysseyAnimationTimelineSection::GetSectionWidth() const
{
	return mWidthInFrames.Get() * mExtension->Timeline()->GetFrameWidth();
}

FOptionalSize
SOdysseyAnimationTimelineSection::GetSectionHeight() const
{
    return mHeightInScreenUnits.Get();
}

void
SOdysseyAnimationTimelineSection::SetContent(TSharedPtr<SWidget> iContent)
{
	mBox->SetContent(iContent.ToSharedRef());
}