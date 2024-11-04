// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyLayerRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyleSet.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Framework/Commands/GenericCommands.h"
#include "Widgets/SOdysseyLayerStackTreeView.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyLayerStack.h"
#include "Widgets/SOdysseyLayerExpanderArrow.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "OdysseyLayerStack.h"
#include "Math/UnitConversion.h"
#include "SEnumCombo.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"

#define LOCTEXT_NAMESPACE "LayerStackEditor"

SOdysseyLayerRow::SOdysseyLayerRow()
    : mSetOpacityTransactionName(LOCTEXT("layer-image-raster.transaction.set-opacity", "Change Layer Opacity"))
{
}

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyLayerRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyLayer* iLayer)
{
    ensure(iLayer);

    SOdysseyLayerRowBase::Construct(
        InArgs,
        iOwnerTableView,
		iLayer
    );
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateWidget( const FName& iRow, const FName& iColumn )
{
	if (iRow == "Main")
	{
		if (iColumn == "IsActivated")
		{
			return GenerateMainRowIsActivatedWidget();
		}
		else if (iColumn == "IsLocked")
		{
			return GenerateMainRowIsLockedWidget();
		}
		else if (iColumn == "DisplayOptions")
		{
			return GenerateMainRowDisplayOptionsWidget();
		}
		else if (iColumn == "Header")
		{
			return GenerateMainRowHeaderWidget();
		}
	}
	if (iRow == "Blend")
	{
		if (iColumn == "Header")
		{
			return GenerateBlendRowHeaderWidget();
		}
	}
    return SOdysseyLayerRowBase::GenerateWidget(iRow, iColumn);
}

FMargin
SOdysseyLayerRow::GetColumnPadding( FName iColumn ) const
{
	if (iColumn == "Header")
		return FMargin(0.f, 0.f, 2.f, 0.f);
	
	return SOdysseyLayerRowBase::GetColumnPadding(iColumn);
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateMainRowHeaderWidget()
{
	TSharedRef<SHorizontalBox> horizontalBox = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(FMargin(2.f, 0.f, 0.f, 0.f))
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew( SOdysseyLayerExpanderArrow, SharedThis(this) )
				.ArrowPadding(FMargin(0.f, 2.f, 0.f, 0.f))
				.ExpanderImageOpened(GetLayer()->IconExpanded.GetIcon())
				.ExpanderImageClosed(GetLayer()->Icon.GetIcon())
				.IndentAmount(16.f)
				.ShouldDrawWires( true )
		]
		+ SHorizontalBox::Slot()
		.Padding(FMargin(2.f, 0.f, 0.f, 0.f))
		.VAlign(VAlign_Center)
		[
			SAssignNew(mNameWidget, SInlineEditableTextBlock)
				.Text(this, &SOdysseyLayerRow::GetLayerName)
				.Font(this, &SOdysseyLayerRow::GetLayerNameFont)
				.OnTextCommitted(this, &SOdysseyLayerRow::OnLayerNameCommited)
				.IsSelected(this, &SOdysseyLayerRow::IsSelectedExclusively) //Allows edition to work
		]
        + SHorizontalBox::Slot()
        .Padding(FMargin(2.f, 0.f, 0.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            SNew(SNumericEntryBox<int>)
            .Visibility(this, &SOdysseyLayerRow::GetCollapsedOpacityVisibility)
            .Value_Lambda([this]() { return (int)(GetLayer()->Opacity * 100.f + 0.5f);})
            .AllowSpin(true)
            .ShiftMouseMovePixelPerDelta(10)
            .Delta(1)
            .MinValue(0)
            .MinSliderValue(0)
            .MaxValue(100)
            .MaxSliderValue(100)
            .OnValueChanged(this, &SOdysseyLayerRow::OnOpacityValueChanged)
            .OnValueCommitted(this, &SOdysseyLayerRow::OnOpacityValueCommitted)
            .OnBeginSliderMovement(this, &SOdysseyLayerRow::OnOpacityBeginSliderMovement)
            .OnEndSliderMovement(this, &SOdysseyLayerRow::OnOpacityEndSliderMovement)
            //.MinDesiredValueWidth  
        ];

	TArray<TSharedPtr<SWidget>> optionWidgets = GenerateMainRowHeaderOptionWidgets();
	for (TSharedPtr<SWidget> widget : optionWidgets)
	{
		horizontalBox->AddSlot()
		.AutoWidth()
		.Padding(FMargin(2.f, 0.f, 0.f, 0.f))
		.VAlign(VAlign_Center)
		[
			widget.ToSharedRef()
		];
	}

	return horizontalBox;
}

TArray<TSharedPtr<SWidget>>
SOdysseyLayerRow::GenerateMainRowHeaderOptionWidgets()
{
	return {};
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateBlendRowHeaderWidget()
{
	return SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.Padding(FMargin(0, 0, 1.f, 0))
		[
			SNew(SNumericEntryBox<int>)
			.IsEnabled_Lambda([this](){ return !GetLayer()->IsLockedRecursively();})
			.Value_Lambda([this]() { return (int)(GetLayer()->Opacity * 100.f + 0.5f);})
			.TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
			.AllowSpin(true)
			.ShiftMouseMovePixelPerDelta(10)
			.Delta(1)
			.MinValue(0)
			.MinSliderValue(0)
			.MaxValue(100)
			.MaxSliderValue(100)
			.OnValueChanged(this, &SOdysseyLayerRow::OnOpacityValueChanged)
			.OnValueCommitted(this, &SOdysseyLayerRow::OnOpacityValueCommitted)
			.OnBeginSliderMovement(this, &SOdysseyLayerRow::OnOpacityBeginSliderMovement)
			.OnEndSliderMovement(this, &SOdysseyLayerRow::OnOpacityEndSliderMovement)
			//.MinDesiredValueWidth  
		]
		+SHorizontalBox::Slot()
		.Padding(FMargin(1.f, 0, 0, 0))
		.VAlign(VAlign_Center)
		[
			SNew(SEnumComboBox, StaticEnum<EOdysseyBlendingMode>())
			.IsEnabled_Lambda([this](){ return !GetLayer()->IsLockedRecursively();})
			.CurrentValue_Lambda([this](){ return (int32)GetLayer()->BlendMode;})
			.ContentPadding(FMargin(0))
			.OnEnumSelectionChanged(this, &SOdysseyLayerRow::OnBlendModeComboBoxChanged)
		];
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateMainRowDisplayOptionsWidget()
{
	const FCheckBoxStyle* displayOptionsToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("LayerStack.DisplayOptionsToggle");
	
	return SNew(SBox)
		.VAlign(VAlign_Center)
		[
			SNew(SCheckBox)
			.Style(displayOptionsToggleStyle)
			.OnCheckStateChanged(this, &SOdysseyLayerRow::OnDisplayOptionsCheckBoxStateChanged)
			.IsChecked(this, &SOdysseyLayerRow::GetDisplayOptionsCheckBoxState)
		];
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateMainRowIsActivatedWidget()
{
    const FCheckBoxStyle* isActivatedToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("LayerStack.IsActivatedToggle");

	return SNew(SBox)
		.VAlign(VAlign_Center)
		[
			SNew(SCheckBox)
			.Style(isActivatedToggleStyle)
			.OnCheckStateChanged(this, &SOdysseyLayerRow::OnIsActivatedCheckBoxStateChanged)
			.IsChecked(this, &SOdysseyLayerRow::GetIsActivatedCheckBoxState)
		];
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateMainRowIsLockedWidget()
{
    const FCheckBoxStyle* isLockedToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("LayerStack.IsLockedToggle");

	return SNew(SBox)
		.VAlign(VAlign_Center)
		[
			SNew(SCheckBox)
			.Style(isLockedToggleStyle)
			.OnCheckStateChanged(this, &SOdysseyLayerRow::OnIsLockedCheckBoxStateChanged)
			.IsChecked(this, &SOdysseyLayerRow::GetIsLockedCheckBoxState)
		];
}

void
SOdysseyLayerRow::OnIsActivatedCheckBoxStateChanged(ECheckBoxState iState)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer.transaction.set-is-activated", "Change Layer Active"));
    FOdysseyObjectEditorUtils::SetPropertyValue(GetLayer(), GET_MEMBER_NAME_CHECKED(UOdysseyLayer, IsActivated), iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyLayerRow::GetIsActivatedCheckBoxState() const
{
	return GetLayer()->IsActivated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyLayerRow::OnIsLockedCheckBoxStateChanged(ECheckBoxState iState)
{
	FScopedTransaction ScopedTransaction(LOCTEXT("layer.transaction.set-is-locked", "Change Layer Lock"));
	FOdysseyObjectEditorUtils::SetPropertyValue(GetLayer(), GET_MEMBER_NAME_CHECKED(UOdysseyLayer, IsLocked), iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyLayerRow::GetIsLockedCheckBoxState() const
{
	return GetLayer()->IsLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

FText
SOdysseyLayerRow::GetLayerName() const
{
    return GetLayer()->Name;
}

void
SOdysseyLayerRow::OnLayerNameCommited(const FText& iText, ETextCommit::Type iType)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer.transaction.set-name", "Change Layer Name"));
	FOdysseyObjectEditorUtils::SetPropertyValue(GetLayer(), GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Name), iText);
}

FSlateFontInfo
SOdysseyLayerRow::GetLayerNameFont() const
{
	if ( !GetLayer() )
		return FStyleDefaults::GetFontInfo();

	UOdysseyLayerStack* layerStack = GetLayer()->GetLayerStack();

	if (!layerStack || layerStack->CurrentLayer != GetLayer())
		return FStyleDefaults::GetFontInfo();
	
	return FAppStyle::Get().GetFontStyle("NormalFontBold");
}

void
SOdysseyLayerRow::Rename()
{
    mNameWidget->EnterEditingMode();
}

FReply
SOdysseyLayerRow::OnRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent, TWeakPtr<SOdysseyLayerStackTreeView> iTreeView)
{
    TSharedPtr<SOdysseyLayerStackTreeView> treeView = iTreeView.Pin();
    if (treeView.IsValid() && iEvent.IsMouseButtonDown( EKeys::LeftMouseButton ))
    {
        TSharedPtr<FOdysseyLayerStackDragDropOperation> operation = treeView->CreateDragDropOperation();

        if (operation.IsValid())
        {
            //PATCH: Unreal does not manage collision between SInlineWidget entering editiong mode
            //         and Drag'n'Drop, which can lead to cases where after dropping an element,
            //       a layer will start editing its name, which is not an expected behaviour
            //       We call OnDragOver on the namewidget to reset the timer responsible for
            //       entering editing mode.
            //       As the handle to the timerdelegate is private, it's the only way to do it
            //       in an almost clean way.
            mNameWidget->OnDragOver(FGeometry(), FDragDropEvent(FPointerEvent(), nullptr));
            //END OF PATCH:

            return FReply::Handled().BeginDragDrop(operation.ToSharedRef());
        }
    }

    return FReply::Unhandled();
}

void
SOdysseyLayerRow::OnDisplayOptionsCheckBoxStateChanged(ECheckBoxState iState)
{
	FOdysseyObjectEditorUtils::SetPropertyValue(GetLayer(), GET_MEMBER_NAME_CHECKED(UOdysseyLayer, DisplayOptions), iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyLayerRow::GetDisplayOptionsCheckBoxState() const
{
	return GetLayer()->DisplayOptions ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyLayerRow::OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    if ( GetLayer()->IsLockedRecursively() )
        return;

    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("layer.transaction.set-blend-mode", "Change Layer BlendMode"));
    FOdysseyObjectEditorUtils::SetPropertyValue(GetLayer(), GET_MEMBER_NAME_CHECKED(UOdysseyLayer, BlendMode), EOdysseyBlendingMode(iValue));
}

void
SOdysseyLayerRow::OnOpacityValueCommitted(int iValue, ETextCommit::Type iType)
{
    if ( GetLayer()->IsLockedRecursively() )
        return;

    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(mSetOpacityTransactionName);
    FOdysseyObjectEditorUtils::SetPropertyValue(GetLayer(), GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Opacity), iValue / 100.f, EPropertyChangeType::ValueSet);
}

void
SOdysseyLayerRow::OnOpacityValueChanged(int iValue)
{
    if ( GetLayer()->IsLockedRecursively() )
        return;

    FOdysseyObjectEditorUtils::SetPropertyValue(GetLayer(), GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Opacity), iValue / 100.f, EPropertyChangeType::Interactive);
}

void
SOdysseyLayerRow::OnOpacityBeginSliderMovement()
{
    //Creating a transaction here manages entering a value using slider
    GEditor->BeginTransaction(mSetOpacityTransactionName);
}

void
SOdysseyLayerRow::OnOpacityEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

EVisibility
SOdysseyLayerRow::GetCollapsedOpacityVisibility() const
{
    return GetLayer()->DisplayOptions ? EVisibility::Collapsed : EVisibility::Visible;
}

#undef LOCTEXT_NAMESPACE
