// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyTextureLayerImageVectorRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"

#define LOCTEXT_NAMESPACE "SOdysseyTextureLayerImageVectorRow"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyTextureLayerImageVectorRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyTextureLayerImageVector* iTextureLayerImageVector)
{
    ensure(iTextureLayerImageVector);
    mTextureLayerImageVector = iTextureLayerImageVector;

    SOdysseyLayerRow::Construct(
        SOdysseyLayerRow::FArguments(),
        iOwnerTableView,
		iTextureLayerImageVector
    );

	SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyTextureLayerImageVectorRow::GenerateHeaderWidget()
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
/*
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
            .OnCheckStateChanged(this, &SOdysseyTextureLayerImageVectorRow::OnIsAlphaLockedCheckStateChanged)
            .IsChecked(this, &SOdysseyTextureLayerImageVectorRow::GetIsAlphaLockedIsChecked)
            [
                //Just for the checkbox to take the space of an icon
                SNew(SImage)
                    .Visibility(EVisibility::Hidden)
                    .Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaLocked16"))
		    ]
        ]
*/
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            SNew(SNumericEntryBox<int>)
            .Value_Lambda([this]() { return (int)(mTextureLayerImageVector->Opacity * 100.f + 0.5f);})
            .AllowSpin(true)
            .ShiftMouseMovePixelPerDelta(10)
            .Delta(1)
            .MinValue(0)
            .MinSliderValue(0)
            .MaxValue(100)
            .MaxSliderValue(100)
            .OnValueChanged(this, &SOdysseyTextureLayerImageVectorRow::OnOpacityValueChanged)
            .OnValueCommitted(this, &SOdysseyTextureLayerImageVectorRow::OnOpacityValueCommitted)
            .OnBeginSliderMovement(this, &SOdysseyTextureLayerImageVectorRow::OnOpacityBeginSliderMovement)
            .OnEndSliderMovement(this, &SOdysseyTextureLayerImageVectorRow::OnOpacityEndSliderMovement)
            //.MinDesiredValueWidth  
        ];
}

/*
void
SOdysseyTextureLayerImageVectorRow::OnIsAlphaLockedCheckStateChanged(ECheckBoxState iState)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Alpha Lock"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageVector, "IsAlphaLocked", iState == ECheckBoxState::Checked);
}
*/

void
SOdysseyTextureLayerImageVectorRow::OnOpacityValueCommitted(int iValue, ETextCommit::Type iType)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Opacity"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageVector, "Opacity", iValue / 100.f, EPropertyChangeType::ValueSet);
}

void
SOdysseyTextureLayerImageVectorRow::OnOpacityValueChanged(int iValue)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageVector, "Opacity", iValue / 100.f, EPropertyChangeType::Interactive);
}

void
SOdysseyTextureLayerImageVectorRow::OnOpacityBeginSliderMovement()
{
    //Creating a transaction here manages entering a value using slider
    GEditor->BeginTransaction(LOCTEXT("LayerTransaction", "Change Layer Opacity"));
}

void
SOdysseyTextureLayerImageVectorRow::OnOpacityEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

/*
ECheckBoxState
SOdysseyTextureLayerImageVectorRow::GetIsAlphaLockedIsChecked() const
{
	return mTextureLayerImageVector->IsAlphaLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}
*/

TSharedRef<SWidget>
SOdysseyTextureLayerImageVectorRow::GenerateOptionsWidget()
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
            .CurrentValue_Lambda([this](){ return (int32)mTextureLayerImageVector->BlendMode;})
            .OnEnumSelectionChanged(this, &SOdysseyTextureLayerImageVectorRow::OnBlendModeComboBoxChanged)
        ];
}

void
SOdysseyTextureLayerImageVectorRow::OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer BlendMode"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageVector, "BlendMode", iValue);
}

#undef LOCTEXT_NAMESPACE
