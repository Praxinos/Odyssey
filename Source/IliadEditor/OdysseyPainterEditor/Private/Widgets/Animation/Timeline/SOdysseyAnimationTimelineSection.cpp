// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineSection.h"

#include "OdysseyPainterEditorAnimationTimelinePosition.h"

void
SOdysseyAnimationTimelineSection::Construct(const FArguments& iArgs)
{
    mWidthInFrames = iArgs._WidthInFrames;
    mTimelinePosition = iArgs._TimelinePosition;

    ChildSlot
    [
        SAssignNew(mBox, SBox)
        .WidthOverride_Raw(this, &SOdysseyAnimationTimelineSection::GetSectionWidth)
        .MinDesiredWidth(this, &SOdysseyAnimationTimelineSection::GetSectionWidth)
        .MaxDesiredWidth(this, &SOdysseyAnimationTimelineSection::GetSectionWidth)
        .HAlign(iArgs._HAlign)
        .VAlign(iArgs._VAlign)
        [
            iArgs._Content.Widget
        ]
    ];
}

FOptionalSize
SOdysseyAnimationTimelineSection::GetSectionWidth() const
{
    return mWidthInFrames.Get() * mTimelinePosition->GetFrameSize();
}

void
SOdysseyAnimationTimelineSection::SetContent(TSharedPtr<SWidget> iContent)
{
    mBox->SetContent(iContent.ToSharedRef());
}
