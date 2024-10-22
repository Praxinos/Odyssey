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
#include "OdysseyLayerStack.h"

#define LOCTEXT_NAMESPACE "LayerStackEditor"

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
SOdysseyLayerRow::GenerateWidgetForColumn( const FName& InColumnName )
{
    if (InColumnName == "IsActivated")
    {
        return GenerateIsActivatedWidget();
    }
    else if (InColumnName == "IsLocked")
    {
        return GenerateIsLockedWidget();
    }
	else if (InColumnName == "DisplayOptions")
    {
        return GenerateDisplayOptionsWidget();
    }
    else if (InColumnName == "Header")
    {
        return GenerateExpandableHeaderWidget();
    }
    return SNullWidget::NullWidget;
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateHeaderWidget()
{
	return SAssignNew(mNameWidget, SInlineEditableTextBlock)
		.Text(this, &SOdysseyLayerRow::GetLayerName)
		.Font(this, &SOdysseyLayerRow::GetLayerNameFont)
		.OnTextCommitted(this, &SOdysseyLayerRow::OnLayerNameCommited)
		.IsSelected(this, &SOdysseyLayerRow::IsSelectedExclusively); //Allows edition to work
		//.Clipping(EWidgetClipping::ClipToBounds)
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateExpandableHeaderWidget()
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .AutoWidth()
        [
            SNew( SOdysseyLayerExpanderArrow, SharedThis(this) )
                .ArrowPadding(FMargin(0.f, 2.f, 0.f, 0.f))
                .ExpanderImageOpened(GetLayer()->IconExpanded.GetIcon())
                .ExpanderImageClosed(GetLayer()->Icon.GetIcon())
                .IndentAmount(16.f)
                .ShouldDrawWires( true )
        ]
		+ SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 2.f, 0.f, 2.f))
		[
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .Padding(FMargin(0.f, 0.f, 0.f, 2.f))
            .AutoHeight()
            [
			    GenerateHeaderWidget()
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
				SNew(SBox)
				.Visibility(this, &SOdysseyLayerRow::OptionsWidgetVisibility)
                [
                    GenerateOptionsWidget()
                ]
            ]
		];
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateOptionsWidget()
{
    TSharedRef<SWidget> optionsWidget = SNullWidget::NullWidget;
    return optionsWidget;
}

EVisibility
SOdysseyLayerRow::OptionsWidgetVisibility() const
{
	return GetLayer()->DisplayOptions ? EVisibility::Visible : EVisibility::Collapsed;
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateDisplayOptionsWidget()
{
	const FCheckBoxStyle* displayOptionsToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("LayerStack.DisplayOptionsToggle");
	return SNew(SCheckBox)
		.Style(displayOptionsToggleStyle)
		.OnCheckStateChanged(this, &SOdysseyLayerRow::OnDisplayOptionsCheckBoxStateChanged)
		.IsChecked(this, &SOdysseyLayerRow::GetDisplayOptionsCheckBoxState);
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateIsActivatedWidget()
{
	const FCheckBoxStyle* isActivatedToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("LayerStack.IsActivatedToggle");

	return SNew(SCheckBox)
		.Style(isActivatedToggleStyle)
		.OnCheckStateChanged(this, &SOdysseyLayerRow::OnIsActivatedCheckBoxStateChanged)
		.IsChecked(this, &SOdysseyLayerRow::GetIsActivatedCheckBoxState);
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateIsLockedWidget()
{
	const FCheckBoxStyle* isLockedToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("LayerStack.IsLockedToggle");

	return SNew(SCheckBox)
		.Style(isLockedToggleStyle)
		.OnCheckStateChanged(this, &SOdysseyLayerRow::OnIsLockedCheckBoxStateChanged)
		.IsChecked(this, &SOdysseyLayerRow::GetIsLockedCheckBoxState);
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
			//		 and Drag'n'Drop, which can lead to cases where after dropping an element,
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

#undef LOCTEXT_NAMESPACE
