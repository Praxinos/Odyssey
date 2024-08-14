// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyTextureLayerImageRasterRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Math/UnitConversion.h"
#include "SEnumCombo.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
SOdysseyTextureLayerImageRasterRow::SOdysseyTextureLayerImageRasterRow()
    : mSetOpacityTransactionName(LOCTEXT("layer-image-raster.transaction.set-opacity", "Change Layer Opacity"))
{
    
}

void SOdysseyTextureLayerImageRasterRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster)
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
    
    const FCheckBoxStyle* alphaLockedToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Texture.AlphaLockedToggle");

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
            SNew(SNumericEntryBox<int>)
            .Visibility(this, &SOdysseyTextureLayerImageRasterRow::GetCollapsedOpacityVisibility)
            .Value_Lambda([this]() { return (int)(mTextureLayerImageRaster->Opacity * 100.f + 0.5f);})
            .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
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
        ]
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            //AlphaLock
            SNew(SCheckBox)
            .Style(alphaLockedToggleStyle)
            .OnCheckStateChanged(this, &SOdysseyTextureLayerImageRasterRow::OnIsAlphaLockedCheckStateChanged)
            .IsChecked(this, &SOdysseyTextureLayerImageRasterRow::GetIsAlphaLockedIsChecked)
        ];
}

TSharedRef<SWidget>
SOdysseyTextureLayerImageRasterRow::GenerateOptionsWidget()
{
	return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .Padding(FMargin(0, 2.f, 0, 0))
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
            .Padding(FMargin(0, 0, 1.f, 0))
            [
                SNew(SNumericEntryBox<int>)
                .IsEnabled_Lambda([this](){ return !mTextureLayerImageRaster->GetIsLockedRecursively();})
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
            ]
            +SHorizontalBox::Slot()
            .Padding(FMargin(1.f, 0, 0, 0))
            .VAlign(VAlign_Center)
            [
                SNew(SEnumComboBox, StaticEnum<EOdysseyBlendingMode>())
                .IsEnabled_Lambda([this](){ return !mTextureLayerImageRaster->GetIsLockedRecursively();})
                .CurrentValue_Lambda([this](){ return (int32)mTextureLayerImageRaster->BlendMode;})
                .ContentPadding(FMargin(0))
                .OnEnumSelectionChanged(this, &SOdysseyTextureLayerImageRasterRow::OnBlendModeComboBoxChanged)
            ]
        ];
}

void
SOdysseyTextureLayerImageRasterRow::OnIsAlphaLockedCheckStateChanged(ECheckBoxState iState)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageRaster, GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageRaster, IsAlphaLocked), iState == ECheckBoxState::Checked);
}

void
SOdysseyTextureLayerImageRasterRow::OnOpacityValueCommitted(int iValue, ETextCommit::Type iType)
{
    if ( mTextureLayerImageRaster->GetIsLockedRecursively() )
        return;

    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(mSetOpacityTransactionName);
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageRaster, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Opacity), iValue / 100.f, EPropertyChangeType::ValueSet);
}

void
SOdysseyTextureLayerImageRasterRow::OnOpacityValueChanged(int iValue)
{
    if ( mTextureLayerImageRaster->GetIsLockedRecursively() )
        return;

    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageRaster, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Opacity), iValue / 100.f, EPropertyChangeType::Interactive);
}

void
SOdysseyTextureLayerImageRasterRow::OnOpacityBeginSliderMovement()
{
    //Creating a transaction here manages entering a value using slider
    GEditor->BeginTransaction(mSetOpacityTransactionName);
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

void
SOdysseyTextureLayerImageRasterRow::OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    if ( mTextureLayerImageRaster->GetIsLockedRecursively() )
        return;

    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-raster.transaction.set-blend-mode", "Change Layer BlendMode"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageRaster, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, BlendMode), EOdysseyBlendingMode(iValue));
}

EVisibility
SOdysseyTextureLayerImageRasterRow::GetCollapsedOpacityVisibility() const
{
    return DisplayOptions() ? EVisibility::Collapsed : EVisibility::Visible;
}

#undef LOCTEXT_NAMESPACE
