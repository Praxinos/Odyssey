// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Animation/Timeline/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyle.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "Widgets/Animation/Timeline/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLighttableKey.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLighttableHeader.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationLayerStack.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Math/UnitConversion.h"
#include "SEnumCombo.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow

void SOdysseyAnimationLayerImageRasterRow::Construct(
    const FArguments& InArgs,
    const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
    UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
)
{
    ensure(iAnimationLayerImageRaster);
    mAnimationLayerImageRaster = iAnimationLayerImageRaster;

    SOdysseyAnimationLayerRow::Construct(
        SOdysseyAnimationLayerRow::FArguments()
            .TimelinePosition(InArgs._TimelinePosition),
        iOwnerTableView,
        iAnimationLayerImageRaster
    );
}

//PRIVATE API-----------------------------------------------------------

TArray<TSharedPtr<SWidget>>
SOdysseyAnimationLayerImageRasterRow::GenerateMainRowHeaderOptionWidgets()
{
    TArray<TSharedPtr<SWidget>> widgets = SOdysseyAnimationLayerRow::GenerateMainRowHeaderOptionWidgets();

    const FCheckBoxStyle* alphaLockedToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Animation.AlphaLockedToggle");

    //AlphaLock
    widgets.Add(
        SNew(SCheckBox)
        .Style(alphaLockedToggleStyle)
        .OnCheckStateChanged(this, &SOdysseyAnimationLayerImageRasterRow::OnIsAlphaLockedCheckStateChanged)
        .IsChecked(this, &SOdysseyAnimationLayerImageRasterRow::GetIsAlphaLockedIsChecked)
    );

    return widgets;
}

void
SOdysseyAnimationLayerImageRasterRow::OnIsAlphaLockedCheckStateChanged(ECheckBoxState iState)
{
    mAnimationLayerImageRaster->SetIsAlphaLocked(iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyAnimationLayerImageRasterRow::GetIsAlphaLockedIsChecked() const
{
    return mAnimationLayerImageRaster->IsAlphaLocked() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE
