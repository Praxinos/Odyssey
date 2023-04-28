// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineSection.h"

void
SOdysseyAnimationTimelineSection::Construct(
	const FArguments& iArgs, 
	FOdysseyAnimationEditor* iEditor
)
{
	mEditor = iEditor;
    mWidthInFrames = iArgs._WidthInFrames;
    mHeightInScreenUnits = iArgs._HeightInScreenUnits;

	ChildSlot
	[
		SAssignNew(mBox, SBox)
		.WidthOverride_Raw(this, &SOdysseyAnimationTimelineSection::GetSectionWidth)
		.HeightOverride_Raw(this, &SOdysseyAnimationTimelineSection::GetSectionHeight)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			iArgs._Content.Widget
		]
	];
}

FOdysseyAnimationEditor*
SOdysseyAnimationTimelineSection::GetEditor() const
{
    return mEditor;
}

FOptionalSize
SOdysseyAnimationTimelineSection::GetSectionWidth() const
{
	return mWidthInFrames.Get() * GetEditor()->Timeline()->GetFrameWidth();
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