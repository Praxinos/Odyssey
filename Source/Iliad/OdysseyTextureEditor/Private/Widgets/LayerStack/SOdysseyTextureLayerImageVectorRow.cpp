// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyTextureLayerImageVectorRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Math/UnitConversion.h"
#include "SEnumCombo.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
SOdysseyTextureLayerImageVectorRow::SOdysseyTextureLayerImageVectorRow()
    : mSetOpacityTransactionName(LOCTEXT("layer-image-vector.transaction.set-opacity", "Change Layer Opacity"))
{

}

void SOdysseyTextureLayerImageVectorRow::Construct( const FArguments& InArgs
                                                  , const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView
                                                  , UOdysseyTextureLayerImageVector* iTextureLayerImageVector )
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
    const FCheckBoxStyle* coloredToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Texture.ColoredToggle");
    const FCheckBoxStyle* wireframeToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Texture.WireframeToggle");

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
            .Visibility(this, &SOdysseyTextureLayerImageVectorRow::GetCollapsedOpacityVisibility)
            .Value_Lambda([this]() { return (int)(mTextureLayerImageVector->Opacity * 100.f + 0.5f);})
            .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
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
        ]
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            //WireframeLock
            SNew(SCheckBox)
            .Style(wireframeToggleStyle)
            .OnCheckStateChanged(this, &SOdysseyTextureLayerImageVectorRow::OnIsWireframeCheckStateChanged)
            .IsChecked(this, &SOdysseyTextureLayerImageVectorRow::GetIsWireframeIsChecked)
        ]
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            //ColoredLock
            SNew(SCheckBox)
            .Style(coloredToggleStyle)
            .OnCheckStateChanged(this, &SOdysseyTextureLayerImageVectorRow::OnIsColoredCheckStateChanged)
            .IsChecked(this, &SOdysseyTextureLayerImageVectorRow::GetIsColoredIsChecked)
        ];
}

TSharedRef<SWidget>
SOdysseyTextureLayerImageVectorRow::GenerateOptionsWidget()
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
                .IsEnabled_Lambda([this](){ return !mTextureLayerImageVector->IsLockedRecursively();})
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
            ]
            +SHorizontalBox::Slot()
            .Padding(FMargin(1.f, 0, 0, 0))
            .VAlign(VAlign_Center)
            [
                SNew(SEnumComboBox, StaticEnum<EOdysseyBlendingMode>())
                .IsEnabled_Lambda([this](){ return !mTextureLayerImageVector->IsLockedRecursively();})
                .CurrentValue_Lambda([this](){ return (int32)mTextureLayerImageVector->BlendMode;})
                .ContentPadding(FMargin(0))
                .OnEnumSelectionChanged(this, &SOdysseyTextureLayerImageVectorRow::OnBlendModeComboBoxChanged)
            ]
        ];
}

void
SOdysseyTextureLayerImageVectorRow::OnIsWireframeCheckStateChanged( ECheckBoxState iState )
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-vector.transaction.set-wireframe", "Change Layer Wireframe status"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageVector, GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageVector, IsWireframe), iState == ECheckBoxState::Checked);
}

void
SOdysseyTextureLayerImageVectorRow::OnIsColoredCheckStateChanged( ECheckBoxState iState )
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-vector.transaction.set-coloring", "Change Layer Coloring"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageVector, GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageVector, IsColored), iState == ECheckBoxState::Checked);
}

void
SOdysseyTextureLayerImageVectorRow::OnOpacityValueCommitted(int iValue, ETextCommit::Type iType)
{
    if ( mTextureLayerImageVector->IsLockedRecursively() )
        return;

    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(mSetOpacityTransactionName);
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageVector, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Opacity), iValue / 100.f, EPropertyChangeType::ValueSet);
}

void
SOdysseyTextureLayerImageVectorRow::OnOpacityValueChanged(int iValue)
{
    if ( mTextureLayerImageVector->IsLockedRecursively() )
        return;

    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageVector, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Opacity), iValue / 100.f, EPropertyChangeType::Interactive);
}

void
SOdysseyTextureLayerImageVectorRow::OnOpacityBeginSliderMovement()
{
    //Creating a transaction here manages entering a value using slider
    GEditor->BeginTransaction(mSetOpacityTransactionName);
}

void
SOdysseyTextureLayerImageVectorRow::OnOpacityEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

ECheckBoxState
SOdysseyTextureLayerImageVectorRow::GetIsWireframeIsChecked() const
{
    return mTextureLayerImageVector->IsWireframe ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState
SOdysseyTextureLayerImageVectorRow::GetIsColoredIsChecked() const
{
    return mTextureLayerImageVector->IsColored ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyTextureLayerImageVectorRow::OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    if ( mTextureLayerImageVector->IsLockedRecursively() )
        return;

    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-vector.transaction.set-blend-mode", "Change Layer BlendMode"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageVector, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, BlendMode), EOdysseyBlendingMode(iValue));
}

EVisibility
SOdysseyTextureLayerImageVectorRow::GetCollapsedOpacityVisibility() const
{
    return DisplayOptions() ? EVisibility::Collapsed : EVisibility::Visible;
}

#undef LOCTEXT_NAMESPACE
