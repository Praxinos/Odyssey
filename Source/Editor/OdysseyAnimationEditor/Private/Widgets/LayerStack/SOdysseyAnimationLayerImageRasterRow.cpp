// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationLayerImageRasterRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/OdysseyAnimationLayerImageRaster.h"
#include "Widgets/LayerStack/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerImageRasterRow"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyAnimationLayerImageRasterRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster)
{
    ensure(iAnimationLayerImageRaster);
    mAnimationLayerImageRaster = iAnimationLayerImageRaster;

    SOdysseyLayerRow::Construct(
        SOdysseyLayerRow::FArguments(),
        iOwnerTableView,
		iAnimationLayerImageRaster
    );

	SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterRow::GenerateHeaderWidget()
{
	TSharedRef<SWidget> defaultWidget = SOdysseyLayerRow::GenerateHeaderWidget();
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
            .Type(ESlateCheckBoxType::ToggleButton)
            .ForegroundColor(FSlateColor::UseForeground())
            .CheckedHoveredImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaLocked16"))
            .CheckedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaLocked16"))
            .CheckedPressedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaLocked16"))
            .UncheckedHoveredImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaUnlocked16"))
            .UncheckedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaUnlocked16"))
            .UncheckedPressedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaUnlocked16"))
            .OnCheckStateChanged(this, &SOdysseyAnimationLayerImageRasterRow::OnIsAlphaLockedCheckStateChanged)
            .IsChecked(this, &SOdysseyAnimationLayerImageRasterRow::GetIsAlphaLockedIsChecked)
            [
                //Just for the checkbox to take the space of an icon
                SNew(SImage)
                    .Visibility(EVisibility::Hidden)
                    .Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaLocked16"))
		    ]
        ]
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            SNew(SNumericEntryBox<int>)
            .Value_Lambda([this]() { return (int)(mAnimationLayerImageRaster->Opacity * 100.f + 0.5f);})
            .AllowSpin(true)
            .ShiftMouseMovePixelPerDelta(10)
            .Delta(1)
            .MinValue(0)
            .MinSliderValue(0)
            .MaxValue(100)
            .MaxSliderValue(100)
            .OnValueChanged(this, &SOdysseyAnimationLayerImageRasterRow::OnOpacityValueChanged)
            .OnValueCommitted(this, &SOdysseyAnimationLayerImageRasterRow::OnOpacityValueCommitted)
            .OnBeginSliderMovement(this, &SOdysseyAnimationLayerImageRasterRow::OnOpacityBeginSliderMovement)
            .OnEndSliderMovement(this, &SOdysseyAnimationLayerImageRasterRow::OnOpacityEndSliderMovement)
            //.MinDesiredValueWidth  
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterRow::GenerateOptionsWidget()
{
	return SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("OdysseyLayerImageRasterBlendingMode", "Blending Mode"))
        ]
        +SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
        [
            SNew(SEnumComboBox, StaticEnum<EOdysseyBlendingMode>())
            .CurrentValue_Lambda([this](){ return (int32)mAnimationLayerImageRaster->BlendMode;})
            .OnEnumSelectionChanged(this, &SOdysseyAnimationLayerImageRasterRow::OnBlendModeComboBoxChanged)
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterRow::GenerateTimelineWidget()
{
    return SNew(SOdysseyAnimationLayerImageRasterTimeline, mAnimationLayerImageRaster);
}

void
SOdysseyAnimationLayerImageRasterRow::OnIsAlphaLockedCheckStateChanged(ECheckBoxState iState)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Alpha Lock"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageRaster, "IsAlphaLocked", iState == ECheckBoxState::Checked);
}

void
SOdysseyAnimationLayerImageRasterRow::OnOpacityValueCommitted(int iValue, ETextCommit::Type iType)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Opacity"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageRaster, "Opacity", iValue / 100.f, EPropertyChangeType::ValueSet);
}

void
SOdysseyAnimationLayerImageRasterRow::OnOpacityValueChanged(int iValue)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageRaster, "Opacity", iValue / 100.f, EPropertyChangeType::Interactive);
}

void
SOdysseyAnimationLayerImageRasterRow::OnOpacityBeginSliderMovement()
{
    //Creating a transaction here manages entering a value using slider
    GEditor->BeginTransaction(LOCTEXT("LayerTransaction", "Change Layer Opacity"));
}

void
SOdysseyAnimationLayerImageRasterRow::OnOpacityEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

ECheckBoxState
SOdysseyAnimationLayerImageRasterRow::GetIsAlphaLockedIsChecked() const
{
	return mAnimationLayerImageRaster->IsAlphaLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyAnimationLayerImageRasterRow::OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer BlendMode"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageRaster, "BlendMode", iValue);
}

#undef LOCTEXT_NAMESPACE
