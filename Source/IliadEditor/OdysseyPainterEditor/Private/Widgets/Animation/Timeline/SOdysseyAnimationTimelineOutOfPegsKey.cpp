// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineOutOfPegsKey.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "OdysseyStyle.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineSection.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "Widgets/Input/SCheckBox.h"

void
SOdysseyAnimationTimelineOutOfPegsKey::Construct(const FArguments& InArgs)
{
    mCell = InArgs._Cell;
    mKey = InArgs._Key;

    mOnActivateOutOfPegs = InArgs._OnActivateOutOfPegs;
    mOnInactivateOutOfPegs = InArgs._OnInactivateOutOfPegs;
    mOnIsOutOfPegsChecked = InArgs._OnIsOutOfPegsChecked;

    const FCheckBoxStyle* checkboxStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.ToggleButton");

    ChildSlot
    [
        SNew(SOdysseyAnimationTimelineSection)
        .TimelinePosition(InArgs._TimelinePosition)
        .WidthInFrames(1)
        .HAlign(HAlign_Center)
        [
            SNew(SCheckBox)
            .IsEnabled(this, &SOdysseyAnimationTimelineOutOfPegsKey::IsOutOfPegsEnabled)
            .Style( checkboxStyle )
            .OnCheckStateChanged(this, &SOdysseyAnimationTimelineOutOfPegsKey::OnOutOfPegsCheckStateChanged)
            .IsChecked(this, &SOdysseyAnimationTimelineOutOfPegsKey::IsOutOfPegsChecked)
            .Padding(FMargin(2.f))
            [
                SNew(SImage)
                .Image(this, &SOdysseyAnimationTimelineOutOfPegsKey::GetOutOfPegsButtonImage)
            ]
        ]
    ];
}

bool
SOdysseyAnimationTimelineOutOfPegsKey::IsOutOfPegsEnabled() const
{
    return mKey.Get().bIsActivated;
}

const FSlateBrush*
SOdysseyAnimationTimelineOutOfPegsKey::GetOutOfPegsButtonImage() const
{
    UOdysseyAnimationCell* cell = mCell.Get();
    if (!cell)
        return nullptr;

    if (cell->IsOutOfPegs())
        return FOdysseyStyle::GetBrush("Animation.LightTable.OutOfPegs.Button.On");

    return FOdysseyStyle::GetBrush("Animation.LightTable.OutOfPegs.Button.Off");
}

void
SOdysseyAnimationTimelineOutOfPegsKey::OnOutOfPegsCheckStateChanged(ECheckBoxState iValue)
{
    if (iValue == ECheckBoxState::Checked)
    {
        if (mOnActivateOutOfPegs.IsBound())
            mOnActivateOutOfPegs.Execute(mCell.Get());
    }
    else
    {
        if (mOnInactivateOutOfPegs.IsBound())
            mOnInactivateOutOfPegs.Execute();
    }
}

ECheckBoxState
SOdysseyAnimationTimelineOutOfPegsKey::IsOutOfPegsChecked() const
{
    if (mOnIsOutOfPegsChecked.IsBound())
        return mOnIsOutOfPegsChecked.Execute(mCell.Get());

    return ECheckBoxState::Unchecked;
}
