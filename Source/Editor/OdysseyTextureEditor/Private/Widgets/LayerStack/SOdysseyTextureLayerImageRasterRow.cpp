// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyTextureLayerImageRasterRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"

#define LOCTEXT_NAMESPACE "SOdysseyTextureLayerImageRasterRow"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyTextureLayerImageRasterRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyLayerStackNewTreeView>& iOwnerTableView, UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster)
{
    ensure(iTextureLayerImageRaster);
    mTextureLayerImageRaster = iTextureLayerImageRaster;

    SOdysseyLayerRow::Construct(
        SOdysseyLayerRow::FArguments(),
        iOwnerTableView,
		iTextureLayerImageRaster
    );

	SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyTextureLayerImageRasterRow::GenerateHeaderWidget()
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
            .OnCheckStateChanged(this, &SOdysseyTextureLayerImageRasterRow::OnIsAlphaLockedCheckStateChanged)
            .IsChecked(this, &SOdysseyTextureLayerImageRasterRow::GetIsAlphaLockedIsChecked)
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
            .Value_Lambda([this]() { return (int)(mTextureLayerImageRaster->Opacity * 100.f + 0.5f);})
            .AllowSpin(true)
            .ShiftMouseMovePixelPerDelta(10)
            .Delta(1)
            .MinValue(0)
            .MinSliderValue(0)
            .MaxValue(100)
            .MaxSliderValue(100)
            .OnValueChanged(this, &SOdysseyTextureLayerImageRasterRow::OnOpacityValueChanged)
            .OnValueCommitted(this, &SOdysseyTextureLayerImageRasterRow::OnOpacityValueCommitted)
            .OnBeginSliderMovement(this, &SOdysseyTextureLayerImageRasterRow::OnOpacityBeginSliderMovement)
            .OnEndSliderMovement(this, &SOdysseyTextureLayerImageRasterRow::OnOpacityEndSliderMovement)
            //.MinDesiredValueWidth  
        ];
}

void
SOdysseyTextureLayerImageRasterRow::OnIsAlphaLockedCheckStateChanged(ECheckBoxState iState)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Alpha Lock"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageRaster, "IsAlphaLocked", iState == ECheckBoxState::Checked);
}

void
SOdysseyTextureLayerImageRasterRow::OnOpacityValueCommitted(int iValue, ETextCommit::Type iType)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Opacity"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageRaster, "Opacity", iValue / 100.f, EPropertyChangeType::ValueSet);
}

void
SOdysseyTextureLayerImageRasterRow::OnOpacityValueChanged(int iValue)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageRaster, "Opacity", iValue / 100.f, EPropertyChangeType::Interactive);
}

void
SOdysseyTextureLayerImageRasterRow::OnOpacityBeginSliderMovement()
{
    //Creating a transaction here manages entering a value using slider
    GEditor->BeginTransaction(LOCTEXT("LayerTransaction", "Change Layer Opacity"));
}

void
SOdysseyTextureLayerImageRasterRow::OnOpacityEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

ECheckBoxState
SOdysseyTextureLayerImageRasterRow::GetIsAlphaLockedIsChecked() const
{
	return mTextureLayerImageRaster->IsAlphaLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

TSharedRef<SWidget>
SOdysseyTextureLayerImageRasterRow::GenerateOptionsWidget()
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
            .CurrentValue_Lambda([this](){ return (int32)mTextureLayerImageRaster->BlendMode;})
            .OnEnumSelectionChanged(this, &SOdysseyTextureLayerImageRasterRow::OnBlendModeComboBoxChanged)
        ];
}

void
SOdysseyTextureLayerImageRasterRow::OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer BlendMode"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageRaster, "BlendMode", iValue);
}

#undef LOCTEXT_NAMESPACE
