// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerImageVectorRow"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyAnimationLayerImageVectorRow::Construct(
    const FArguments& InArgs,
    const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
)
{
    ensure(iAnimationLayerImageVector);
    mAnimationLayerImageVector = iAnimationLayerImageVector;

    SOdysseyAnimationLayerRow::Construct(
        SOdysseyAnimationLayerRow::FArguments(),
        iOwnerTableView,
        iExtension,
		iAnimationLayerImageVector
    );

	SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorRow::GenerateHeaderWidget()
{
    const FCheckBoxStyle* lightTableToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Animation.LightTableToggle");
    const FCheckBoxStyle* alphaLockedToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Animation.AlphaLockedToggle");

    return SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        [
            //LayerName
            SOdysseyLayerRow::GenerateHeaderWidget()
        ]
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            //AlphaLock
            SNew(SCheckBox)
            .Style(lightTableToggleStyle)
            .OnCheckStateChanged(this, &SOdysseyAnimationLayerImageVectorRow::OnLightTableCheckStateChanged)
            .IsChecked(this, &SOdysseyAnimationLayerImageVectorRow::GetLightTableIsChecked)
        ]
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            //AlphaLock
            SNew(SCheckBox)
            .Style(alphaLockedToggleStyle)
            .OnCheckStateChanged(this, &SOdysseyAnimationLayerImageVectorRow::OnIsAlphaLockedCheckStateChanged)
            .IsChecked(this, &SOdysseyAnimationLayerImageVectorRow::GetIsAlphaLockedIsChecked)
        ]
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            SNew(SNumericEntryBox<int>)
            .Value_Lambda([this]() { return (int)(mAnimationLayerImageVector->Opacity * 100.f + 0.5f);})
            .AllowSpin(true)
            .ShiftMouseMovePixelPerDelta(10)
            .Delta(1)
            .MinValue(0)
            .MinSliderValue(0)
            .MaxValue(100)
            .MaxSliderValue(100)
            .OnValueChanged(this, &SOdysseyAnimationLayerImageVectorRow::OnOpacityValueChanged)
            .OnValueCommitted(this, &SOdysseyAnimationLayerImageVectorRow::OnOpacityValueCommitted)
            .OnBeginSliderMovement(this, &SOdysseyAnimationLayerImageVectorRow::OnOpacityBeginSliderMovement)
            .OnEndSliderMovement(this, &SOdysseyAnimationLayerImageVectorRow::OnOpacityEndSliderMovement)
            //.MinDesiredValueWidth  
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorRow::GenerateOptionsWidget()
{
	return SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("OdysseyLayerImageVectorBlendingMode", "Blending Mode"))
        ]
        +SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
        [
            SNew(SEnumComboBox, StaticEnum<EOdysseyBlendingMode>())
            .CurrentValue_Lambda([this](){ return (int32)mAnimationLayerImageVector->BlendMode;})
            .OnEnumSelectionChanged(this, &SOdysseyAnimationLayerImageVectorRow::OnBlendModeComboBoxChanged)
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorRow::GenerateTimelineWidget()
{
    return SNew(SOdysseyAnimationLayerImageVectorTimeline, GetExtension(), mAnimationLayerImageVector);
}

void
SOdysseyAnimationLayerImageVectorRow::OnLightTableCheckStateChanged(ECheckBoxState iState)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageVector, "bIsLightTableActivated", iState == ECheckBoxState::Checked);
}

void
SOdysseyAnimationLayerImageVectorRow::OnIsAlphaLockedCheckStateChanged(ECheckBoxState iState)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageVector, "IsAlphaLocked", iState == ECheckBoxState::Checked);
}

void
SOdysseyAnimationLayerImageVectorRow::OnOpacityValueCommitted(int iValue, ETextCommit::Type iType)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Opacity"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageVector, "Opacity", iValue / 100.f, EPropertyChangeType::ValueSet);
}

void
SOdysseyAnimationLayerImageVectorRow::OnOpacityValueChanged(int iValue)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageVector, "Opacity", iValue / 100.f, EPropertyChangeType::Interactive);
}

void
SOdysseyAnimationLayerImageVectorRow::OnOpacityBeginSliderMovement()
{
    //Creating a transaction here manages entering a value using slider
    GEditor->BeginTransaction(LOCTEXT("LayerTransaction", "Change Layer Opacity"));
}

void
SOdysseyAnimationLayerImageVectorRow::OnOpacityEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

ECheckBoxState
SOdysseyAnimationLayerImageVectorRow::GetLightTableIsChecked() const
{
	return mAnimationLayerImageVector->bIsLightTableActivated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState
SOdysseyAnimationLayerImageVectorRow::GetIsAlphaLockedIsChecked() const
{
	return mAnimationLayerImageVector->IsAlphaLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyAnimationLayerImageVectorRow::OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer BlendMode"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageVector, "BlendMode", iValue);
}

#undef LOCTEXT_NAMESPACE
