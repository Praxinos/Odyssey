// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineSection.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"

void
SOdysseyAnimationTimelineSection::Construct(
    const FArguments& iArgs,
    FOdysseyAnimationEditorExtension* iExtension
)
{
    mExtension = iExtension;
    mWidthInFrames = iArgs._WidthInFrames;

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

void
SOdysseyAnimationTimelineSection::SetContent(TSharedPtr<SWidget> iContent)
{
    mBox->SetContent(iContent.ToSharedRef());
}
