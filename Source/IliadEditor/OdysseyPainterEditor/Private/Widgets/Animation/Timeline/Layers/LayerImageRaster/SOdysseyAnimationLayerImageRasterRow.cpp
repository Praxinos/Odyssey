// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyle.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "Widgets/Animation/Timeline/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLightTableKey.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLightTableHeader.h"
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
    mEditor = InArgs._PainterEditor;

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
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageRaster, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayerImageRaster,  IsAlphaLocked), iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyAnimationLayerImageRasterRow::GetIsAlphaLockedIsChecked() const
{
    return mAnimationLayerImageRaster->IsAlphaLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE
