// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableKey.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableMutator.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow

SOdysseyAnimationLayerImageRasterRow::SOdysseyAnimationLayerImageRasterRow()
    : mSetOpacityTransactionName(LOCTEXT("layer-image-raster.transaction.set-opacity", "Change Layer Opacity"))
    , mLightTablePreviousKeysColorBlockWidget(nullptr)
    , mLightTableNextKeysColorBlockWidget(nullptr)
{
    
}

void SOdysseyAnimationLayerImageRasterRow::Construct(
    const FArguments& InArgs,
    const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
)
{
    ensure(iAnimationLayerImageRaster);
    mAnimationLayerImageRaster = iAnimationLayerImageRaster;

    SOdysseyAnimationLayerRow::Construct(
        SOdysseyAnimationLayerRow::FArguments(),
        iOwnerTableView,
        iExtension,
		iAnimationLayerImageRaster
    );

	SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterRow::GenerateHeaderWidget()
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
            SNew(SNumericEntryBox<int>)
            .Visibility(this, &SOdysseyAnimationLayerImageRasterRow::GetCollapsedOpacityVisibility)
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
        ]
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            //LightTable
            SNew(SCheckBox)
            .Style(lightTableToggleStyle)
            .OnCheckStateChanged(this, &SOdysseyAnimationLayerImageRasterRow::OnLightTableCheckStateChanged)
            .IsChecked(this, &SOdysseyAnimationLayerImageRasterRow::GetLightTableIsChecked)
        ]
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            //AlphaLock
            SNew(SCheckBox)
            .Style(alphaLockedToggleStyle)
            .OnCheckStateChanged(this, &SOdysseyAnimationLayerImageRasterRow::OnIsAlphaLockedCheckStateChanged)
            .IsChecked(this, &SOdysseyAnimationLayerImageRasterRow::GetIsAlphaLockedIsChecked)
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterRow::GenerateOptionsWidget()
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
            ]
            +SHorizontalBox::Slot()
            .Padding(FMargin(1.f, 0, 0, 0))
            .VAlign(VAlign_Center)
            [
                SNew(SEnumComboBox, StaticEnum<EOdysseyBlendingMode>())
                .CurrentValue_Lambda([this](){ return (int32)mAnimationLayerImageRaster->BlendMode;})
                .ContentPadding(FMargin(0))
                .OnEnumSelectionChanged(this, &SOdysseyAnimationLayerImageRasterRow::OnBlendModeComboBoxChanged)
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SBox)
            .HeightOverride(FOptionalSize(SOdysseyAnimationTimelineLightTableKey::mDesiredHeight))
            .Visibility(this, &SOdysseyAnimationLayerImageRasterRow::GetLightTableVisibility)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("layer-image-raster.timeline-header.lighttable", "LightTable"))
                ]
                + SVerticalBox::Slot()
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    [
                        SAssignNew(mLightTablePreviousKeysColorBlockWidget, SColorBlock)
					    .CornerRadius(FVector4(4.0f,4.0f,4.0f,4.0f))
                        .Color(this, &SOdysseyAnimationLayerImageRasterRow::GetLightTablePreviousKeysColor)
                        .UseSRGB(true)
                        .OnMouseButtonDown(this, &SOdysseyAnimationLayerImageRasterRow::OnLightTablePreviousKeysColorMouseButtonDown)
                        .ShowBackgroundForAlpha(true)
                        //.IgnoreAlpha(true)
                    ]
                    + SHorizontalBox::Slot()
                    [
                        SAssignNew(mLightTableNextKeysColorBlockWidget, SColorBlock)
                        .CornerRadius(FVector4(4.0f,4.0f,4.0f,4.0f))
                        .Color(this, &SOdysseyAnimationLayerImageRasterRow::GetLightTableNextKeysColor)
                        .UseSRGB(true)
                        .OnMouseButtonDown(this, &SOdysseyAnimationLayerImageRasterRow::OnLightTableNextKeysColorMouseButtonDown)
                        .ShowBackgroundForAlpha(true)
                        //.IgnoreAlpha(true)
                    ]
                ]
            ]
        ];
}

FLinearColor
SOdysseyAnimationLayerImageRasterRow::GetLightTablePreviousKeysColor() const
{
    TSharedPtr<FOdysseyAnimationLightTable> lightTable = mAnimationLayerImageRaster->GetLightTable();
    return lightTable->GetPreviousKeysColor();
}

FLinearColor
SOdysseyAnimationLayerImageRasterRow::GetLightTableNextKeysColor() const
{
    TSharedPtr<FOdysseyAnimationLightTable> lightTable = mAnimationLayerImageRaster->GetLightTable();
    return lightTable->GetNextKeysColor();
}

FReply
SOdysseyAnimationLayerImageRasterRow::OnLightTablePreviousKeysColorMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) const
{
    FColorPickerArgs PickerArgs;
	{
		PickerArgs.bUseAlpha = true;
		PickerArgs.bOnlyRefreshOnMouseUp = false;
		PickerArgs.bOnlyRefreshOnOk = false;
		PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateLambda(
            [this](FLinearColor iColor)
            {
                TSharedPtr<FOdysseyAnimationLightTable> lightTable = mAnimationLayerImageRaster->GetLightTable();
                FOdysseyAnimationLightTableMutator mutator(lightTable);
                mutator.SetPreviousKeysColor(iColor);
            }
        );
		//PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateSP(this, &FColorStructCustomization::OnColorPickerCancelled);
		//PickerArgs.OnColorPickerWindowClosed = FOnWindowClosed::CreateSP(this, &FColorStructCustomization::OnColorPickerWindowClosed);
		//PickerArgs.OnInteractivePickBegin = FSimpleDelegate::CreateSP(this, &FColorStructCustomization::OnColorPickerInteractiveBegin);
		//PickerArgs.OnInteractivePickEnd = FSimpleDelegate::CreateSP(this, &FColorStructCustomization::OnColorPickerInteractiveEnd);
		PickerArgs.InitialColor = GetLightTablePreviousKeysColor();
		PickerArgs.ParentWidget = mLightTablePreviousKeysColorBlockWidget;
		PickerArgs.bOpenAsMenu = true;
	}

	OpenColorPicker(PickerArgs);

    return FReply::Handled();
}

FReply
SOdysseyAnimationLayerImageRasterRow::OnLightTableNextKeysColorMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) const
{
    FColorPickerArgs PickerArgs;
	{
		PickerArgs.bUseAlpha = true;
		PickerArgs.bOnlyRefreshOnMouseUp = false;
		PickerArgs.bOnlyRefreshOnOk = false;
		PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateLambda(
            [this](FLinearColor iColor)
            {
                TSharedPtr<FOdysseyAnimationLightTable> lightTable = mAnimationLayerImageRaster->GetLightTable();
                FOdysseyAnimationLightTableMutator mutator(lightTable);
                mutator.SetNextKeysColor(iColor);
            }
        );
		//PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateSP(this, &FColorStructCustomization::OnColorPickerCancelled);
		//PickerArgs.OnColorPickerWindowClosed = FOnWindowClosed::CreateSP(this, &FColorStructCustomization::OnColorPickerWindowClosed);
		//PickerArgs.OnInteractivePickBegin = FSimpleDelegate::CreateSP(this, &FColorStructCustomization::OnColorPickerInteractiveBegin);
		//PickerArgs.OnInteractivePickEnd = FSimpleDelegate::CreateSP(this, &FColorStructCustomization::OnColorPickerInteractiveEnd);
		PickerArgs.InitialColor = GetLightTableNextKeysColor();
		PickerArgs.ParentWidget = mLightTableNextKeysColorBlockWidget;
		PickerArgs.bOpenAsMenu = true;
	}

	OpenColorPicker(PickerArgs);

    return FReply::Handled();
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterRow::GenerateTimelineWidget()
{
    return SNew(SOdysseyAnimationLayerImageRasterTimeline, GetExtension(), mAnimationLayerImageRaster)
        .IsCollapsed(this, &SOdysseyAnimationLayerImageRasterRow::IsCollapsed);
}

void
SOdysseyAnimationLayerImageRasterRow::OnLightTableCheckStateChanged(ECheckBoxState iState)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageRaster, "bIsLightTableActivated", iState == ECheckBoxState::Checked);
}

void
SOdysseyAnimationLayerImageRasterRow::OnIsAlphaLockedCheckStateChanged(ECheckBoxState iState)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageRaster, "IsAlphaLocked", iState == ECheckBoxState::Checked);
}

void
SOdysseyAnimationLayerImageRasterRow::OnOpacityValueCommitted(int iValue, ETextCommit::Type iType)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(mSetOpacityTransactionName);
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
    GEditor->BeginTransaction(mSetOpacityTransactionName);
}

void
SOdysseyAnimationLayerImageRasterRow::OnOpacityEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

ECheckBoxState
SOdysseyAnimationLayerImageRasterRow::GetLightTableIsChecked() const
{
	return mAnimationLayerImageRaster->bIsLightTableActivated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
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
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-raster.transaction.set-blend-mode", "Change Layer BlendMode"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageRaster, "BlendMode", iValue);
}

EVisibility
SOdysseyAnimationLayerImageRasterRow::GetLightTableVisibility() const
{
    return mAnimationLayerImageRaster->bIsLightTableActivated ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
SOdysseyAnimationLayerImageRasterRow::GetCollapsedOpacityVisibility() const
{
    return IsCollapsed() ? EVisibility::Visible : EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE
