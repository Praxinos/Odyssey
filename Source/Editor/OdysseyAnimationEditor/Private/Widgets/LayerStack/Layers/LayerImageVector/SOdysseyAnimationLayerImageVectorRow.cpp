// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableKey.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Math/UnitConversion.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow

SOdysseyAnimationLayerImageVectorRow::SOdysseyAnimationLayerImageVectorRow()
    : mSetOpacityTransactionName(LOCTEXT("layer-image-vector.transaction.set-opacity", "Change Layer Opacity"))
{
    
}

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
            .Visibility(this, &SOdysseyAnimationLayerImageVectorRow::GetCollapsedOpacityVisibility)
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
        ]
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            //LightTable
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
            //WireframeLock
            SNew(SCheckBox)
            .Style(wireframeToggleStyle)
            .OnCheckStateChanged(this, &SOdysseyAnimationLayerImageVectorRow::OnIsWireframeCheckStateChanged)
            .IsChecked(this, &SOdysseyAnimationLayerImageVectorRow::GetIsWireframeIsChecked)
        ]
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            //ColoredLock
            SNew(SCheckBox)
            .Style(coloredToggleStyle)
            .OnCheckStateChanged(this, &SOdysseyAnimationLayerImageVectorRow::OnIsColoredCheckStateChanged)
            .IsChecked(this, &SOdysseyAnimationLayerImageVectorRow::GetIsColoredIsChecked)
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorRow::GenerateOptionsWidget()
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
                .Value_Lambda([this]() { return (int)(mAnimationLayerImageVector->Opacity * 100.f + 0.5f);})
                .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
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
            ]
            +SHorizontalBox::Slot()
            .Padding(FMargin(1.f, 0, 0, 0))
            .VAlign(VAlign_Center)
            [
                SNew(SEnumComboBox, StaticEnum<EOdysseyBlendingMode>())
                .CurrentValue_Lambda([this](){ return (int32)mAnimationLayerImageVector->BlendMode;})
                .ContentPadding(FMargin(0))
                .OnEnumSelectionChanged(this, &SOdysseyAnimationLayerImageVectorRow::OnBlendModeComboBoxChanged)
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyAnimationTimelineLightTableHeader)
            .LightTable(mAnimationLayerImageVector->GetLightTable())
		    .Visibility(this, &SOdysseyAnimationLayerImageVectorRow::GetLightTableVisibility)
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorRow::GenerateTimelineWidget()
{
    return SNew(SOdysseyAnimationLayerImageVectorTimeline, GetExtension(), mAnimationLayerImageVector)
        .IsCollapsed(this, &SOdysseyAnimationLayerImageVectorRow::IsCollapsed);
}

void
SOdysseyAnimationLayerImageVectorRow::OnIsWireframeCheckStateChanged( ECheckBoxState iState )
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-vector.transaction.set-wireframe", "Change Layer Wireframe status"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageVector, "IsWireframe", iState == ECheckBoxState::Checked);
}

void
SOdysseyAnimationLayerImageVectorRow::OnIsColoredCheckStateChanged( ECheckBoxState iState )
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-vector.transaction.set-coloring", "Change Layer Coloring"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageVector, "IsColored", iState == ECheckBoxState::Checked);
}

void
SOdysseyAnimationLayerImageVectorRow::OnLightTableCheckStateChanged(ECheckBoxState iState)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageVector, "bIsLightTableActivated", iState == ECheckBoxState::Checked);
}

void
SOdysseyAnimationLayerImageVectorRow::OnOpacityValueCommitted(int iValue, ETextCommit::Type iType)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(mSetOpacityTransactionName);
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
    GEditor->BeginTransaction(mSetOpacityTransactionName);
}

void
SOdysseyAnimationLayerImageVectorRow::OnOpacityEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

ECheckBoxState
SOdysseyAnimationLayerImageVectorRow::GetIsWireframeIsChecked() const
{
	return mAnimationLayerImageVector->IsWireframe ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState
SOdysseyAnimationLayerImageVectorRow::GetIsColoredIsChecked() const
{
	return mAnimationLayerImageVector->IsColored ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState
SOdysseyAnimationLayerImageVectorRow::GetLightTableIsChecked() const
{
	return mAnimationLayerImageVector->bIsLightTableActivated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyAnimationLayerImageVectorRow::OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-vector.transaction.set-blend-mode", "Change Layer BlendMode"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageVector, "BlendMode", iValue);
}

EVisibility
SOdysseyAnimationLayerImageVectorRow::GetLightTableVisibility() const
{
    return mAnimationLayerImageVector->bIsLightTableActivated ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
SOdysseyAnimationLayerImageVectorRow::GetCollapsedOpacityVisibility() const
{
    return IsCollapsed() ? EVisibility::Visible : EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE
