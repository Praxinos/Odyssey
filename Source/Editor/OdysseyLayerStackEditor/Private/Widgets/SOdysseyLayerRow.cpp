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

#define LOCTEXT_NAMESPACE "SOdysseyLayerRow"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyLayerRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyLayer* iLayer)
{
    ensure(iLayer);
    mLayer = iLayer;

    SMultiColumnTableRow<UOdysseyLayer*>::FArguments args;
    args.Style(&FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("OdysseyLayerStack.AlternatedRows"))
        .OnCanAcceptDrop(this, &SOdysseyLayerRow::OnRowCanAcceptDrop)
        .OnAcceptDrop(this, &SOdysseyLayerRow::OnRowAcceptDrop)
        .OnDragDetected(this, &SOdysseyLayerRow::OnRowDragDetected, TWeakPtr<SOdysseyLayerStackTreeView>(iOwnerTableView));

    SMultiColumnTableRow<UOdysseyLayer*>::Construct(
        args,
        iOwnerTableView
    );

	SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
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
	else if (InColumnName == "IsOptionsDisplayed")
    {
        return GenerateIsOptionsDisplayedWidget();
    }
    else if (InColumnName == "Header")
    {
        return GenerateExpandableHeaderWidget();
    }
    return SNullWidget::NullWidget;
}

const FSlateBrush*
SOdysseyLayerRow::GetBorder() const 
{
    const FSlateBrush* borderBrush = SMultiColumnTableRow<UOdysseyLayer*>::GetBorder();

    if (!mLayer) 
        return borderBrush;

	UOdysseyLayerStack* layerStack = mLayer->GetLayerStack();
	if ( !layerStack || layerStack->CurrentLayer != mLayer)
		return borderBrush;

    const bool bIsActive = OwnerTablePtr.Pin()->AsWidget()->HasKeyboardFocus();
    return bIsActive ? FOdysseyStyle::GetBrush("OdysseyLayerStack.CurrentLayerBackgroundBrush") : FOdysseyStyle::GetBrush("OdysseyLayerStack.CurrentLayerInactiveBackgroundBrush");
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
                .ExpanderImageOpened(&mLayer->IconExpanded)
                .ExpanderImageClosed(&mLayer->Icon)
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
                SNew(SWidgetSwitcher)
				.WidgetIndex_Lambda([this]() { return mIsOptionsDisplayed ? 1 : 0; })
                +SWidgetSwitcher::Slot()
                [
                    SNullWidget::NullWidget
                ]
                +SWidgetSwitcher::Slot()
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
	/*
    if(mLayer->OptionsWidget) //use this widget class
    {
		if (!mLayer->OptionsWidgetInstance)
        	mLayer->OptionsWidgetInstance = NewObject<UUserWidget>(mLayer, mLayer->OptionsWidget, NAME_None, RF_Public);
        optionsWidget = mLayer->OptionsWidgetInstance->TakeWidget();
    }
	*/
    return optionsWidget;
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateIsOptionsDisplayedWidget()
{
	return SNew(SCheckBox)
		.Type(ESlateCheckBoxType::ToggleButton)
		.ForegroundColor(FSlateColor::UseForeground())
		.CheckedHoveredImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.OptionsDisplayed16"))
		.CheckedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.OptionsDisplayed16"))
		.CheckedPressedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.OptionsDisplayed16"))
		.UncheckedHoveredImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.OptionsHidden16"))
		.UncheckedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.OptionsHidden16"))
		.UncheckedPressedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.OptionsHidden16"))
		.OnCheckStateChanged(this, &SOdysseyLayerRow::OnIsOptionsDisplayedCheckBoxStateChanged)
		.IsChecked(this, &SOdysseyLayerRow::GetIsOptionsDisplayedCheckBoxState)
		[
			//Just for the checkbox to take the space of an icon
			SNew(SImage)
				.Visibility(EVisibility::Hidden)
				.Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.OptionsDisplayed16"))
		];
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateIsActivatedWidget()
{
	return SNew(SCheckBox)
		.Type(ESlateCheckBoxType::ToggleButton)
		.ForegroundColor(FSlateColor::UseForeground())
		.CheckedHoveredImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16"))
		.CheckedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16"))
		.CheckedPressedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16"))
		.UncheckedHoveredImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.NotVisible16"))
		.UncheckedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.NotVisible16"))
		.UncheckedPressedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.NotVisible16"))
		.OnCheckStateChanged(this, &SOdysseyLayerRow::OnIsActivatedCheckBoxStateChanged)
		.IsChecked(this, &SOdysseyLayerRow::GetIsActivatedCheckBoxState)
		[
			//Just for the checkbox to take the space of an icon
			SNew(SImage)
				.Visibility(EVisibility::Hidden)
				.Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16"))
		];
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateIsLockedWidget()
{
	return SNew(SCheckBox)
		.Type(ESlateCheckBoxType::ToggleButton)
		.ForegroundColor(FSlateColor::UseForeground())
		.CheckedHoveredImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.Locked16"))
		.CheckedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.Locked16"))
		.CheckedPressedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.Locked16"))
		.UncheckedHoveredImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.Unlocked16"))
		.UncheckedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.Unlocked16"))
		.UncheckedPressedImage(FOdysseyStyle::GetBrush("OdysseyLayerStack.Unlocked16"))
		.OnCheckStateChanged(this, &SOdysseyLayerRow::OnIsLockedCheckBoxStateChanged)
		.IsChecked(this, &SOdysseyLayerRow::GetIsLockedCheckBoxState)
		[
			//Just for the checkbox to take the space of an icon
			SNew(SImage)
				.Visibility(EVisibility::Hidden)
				.Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.Locked16"))
		];
}

void
SOdysseyLayerRow::OnIsActivatedCheckBoxStateChanged(ECheckBoxState iState)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Active"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, "IsActivated", iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyLayerRow::GetIsActivatedCheckBoxState() const
{
	return mLayer->IsActivated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyLayerRow::OnIsLockedCheckBoxStateChanged(ECheckBoxState iState)
{
	FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Lock"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, "IsLocked", iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyLayerRow::GetIsLockedCheckBoxState() const
{
	return mLayer->IsLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

FText
SOdysseyLayerRow::GetLayerName() const
{
    return mLayer->Name;
}

UOdysseyLayer*
SOdysseyLayerRow::GetLayer()
{
    return mLayer;
}

void
SOdysseyLayerRow::OnLayerNameCommited(const FText& iText, ETextCommit::Type iType)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Name"));
	FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, "Name", iText);
}

FSlateFontInfo
SOdysseyLayerRow::GetLayerNameFont() const
{
	if ( !mLayer )
		return FStyleDefaults::GetFontInfo();

	UOdysseyLayerStack* layerStack = mLayer->GetLayerStack();

	if (!layerStack || layerStack->CurrentLayer != mLayer)
		return FStyleDefaults::GetFontInfo();
	
	return FAppStyle::Get().GetFontStyle("NormalFontBold");
}

void
SOdysseyLayerRow::Rename()
{
	mNameWidget->EnterEditingMode();
}

/** @return the zone (above, onto, below) based on where the user is hovering over within the row */
EItemDropZone
SOdysseyLayerRow::ComputeItemDropZoneForLeaf(FVector2D iLocalPointerPos, FVector2D iLocalSize, bool iCanHaveChildren, bool iIsExpanded)
{
	EOrientation orientation = OwnerTablePtr.Pin()->Private_GetOrientation();
	const float pointerPos = orientation == EOrientation::Orient_Horizontal ? iLocalPointerPos.X : iLocalPointerPos.Y;
	const float size = orientation == EOrientation::Orient_Horizontal ? iLocalSize.X : iLocalSize.Y;

	if ( iCanHaveChildren )
	{
		EItemDropZone dropZone = ZoneFromPointerPosition(iLocalPointerPos, iLocalSize, orientation);
		if (iIsExpanded && dropZone == EItemDropZone::BelowItem)
		{
			return EItemDropZone::OntoItem;
		}
		else
		{
			//default behaviour where we can drop anywhere (below, above and onto)
			return dropZone;
		}
	}
	else
	{
		const float middle = FMath::Max(size * 0.5f, 3.0f);
		if ( pointerPos < middle )
		{
			return EItemDropZone::AboveItem;
		}
		else
		{
			return EItemDropZone::BelowItem;
		}
	}

	
}

TOptional<EItemDropZone>
SOdysseyLayerRow::OnRowCanAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyLayer* iLayer)
{
    StaticCastSharedPtr<SOdysseyLayerStackTreeView>(OwnerTablePtr.Pin())->ResetDropZone();

    EItemDropZone emptyDropZone;
	if ( !mLayer )
		return emptyDropZone;

	UOdysseyLayerStack* layerStack = mLayer->GetLayerStack();
	if ( !layerStack )
		return emptyDropZone;
    //check if CanHaveChildren
    //allow Onto
    
	TSharedPtr<FOdysseyLayerStackDragDropOperation> operation = iEvent.GetOperationAs<FOdysseyLayerStackDragDropOperation>();
    if (!operation)
        return emptyDropZone;

	UOdysseyLayerStack* operationLayerStack = operation->GetLayerStack();
	if ( !operationLayerStack )
		return emptyDropZone;

	FGeometry geometry = GetTickSpaceGeometry();
	const FVector2D localPointerPos = geometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition());
	EItemDropZone expectedDropZone = ComputeItemDropZoneForLeaf(localPointerPos, geometry.GetLocalSize(), mLayer->CanHaveChildren, mLayer->IsExpanded);

	if ( operationLayerStack == layerStack ) //droped from same layerstack, do a move of topmost dropped layers
	{
		UOdysseyLayer* parent = mLayer->GetParent();
		TArray<UOdysseyLayer*> layers = operation->GetLayers();
		switch ( expectedDropZone )
		{
			case EItemDropZone::AboveItem:
			case EItemDropZone::BelowItem:
			{
				if ( !operationLayerStack->CanMoveLayers(layers, parent) )
					return emptyDropZone;
			}
			break;

			case EItemDropZone::OntoItem:
			{
				if ( !operationLayerStack->CanMoveLayers(layers, parent) )
					return emptyDropZone;
			}
			break;
		}
	}

	return expectedDropZone;
}

FReply
SOdysseyLayerRow::OnRowAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyLayer* iLayer)
{
    StaticCastSharedPtr<SOdysseyLayerStackTreeView>(OwnerTablePtr.Pin())->ResetDropZone();

	TOptional<EItemDropZone> dropZone = OnRowCanAcceptDrop(iEvent, iDropZone, iLayer);
    if (!dropZone.IsSet())
        return FReply::Unhandled();

	TSharedPtr<FOdysseyLayerStackDragDropOperation> operation = iEvent.GetOperationAs<FOdysseyLayerStackDragDropOperation>();
    if (!operation)
        return FReply::Unhandled();

    UOdysseyLayerStack* layerStack = mLayer->GetLayerStack();
	if ( !layerStack )
		return FReply::Unhandled();

	UOdysseyLayerStack* operationLayerStack = operation->GetLayerStack();
	if ( !operationLayerStack )
		return FReply::Unhandled();

	UOdysseyLayer* parent = mLayer->GetParent();
    TArray<UOdysseyLayer*> layers = operation->GetLayers();
	int index = mLayer->GetIndexInParent();

	switch ( iDropZone )
	{
		case EItemDropZone::AboveItem:
		{
			//do nothing
			if ( operationLayerStack == layerStack ) //droped from same layerstack, do a move of topmost dropped layers
			{
				layerStack->MoveLayers(layers, parent, index);
			}
			else
			{
				layerStack->CopyLayers(layers, parent, index);
			}
		}
		break;

		case EItemDropZone::OntoItem:
		{
			if ( operationLayerStack == layerStack ) //droped from same layerstack, do a move of topmost dropped layers
			{
				if ( mLayer->CanHaveChildren )
				{
					layerStack->MoveLayers(layers, mLayer, 0);
				}
				else
				{
					layerStack->MoveLayers(layers, parent, index);
				}
			}
			else
			{
				if ( mLayer->CanHaveChildren )
				{
					layerStack->CopyLayers(layers, mLayer, 0);
				}
				else
				{
					layerStack->CopyLayers(layers, parent, index);
				}
			}
		}
		break;

		case EItemDropZone::BelowItem:
		{
			if ( operationLayerStack == layerStack ) //droped from same layerstack, do a move of topmost dropped layers
			{
				layerStack->MoveLayers(layers, parent, index + 1);
			}
			else
			{
				layerStack->CopyLayers(layers, parent, index + 1);
			}
		}
		break;
	}

    return FReply::Handled();
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
SOdysseyLayerRow::OnIsOptionsDisplayedCheckBoxStateChanged(ECheckBoxState iState)
{
	mIsOptionsDisplayed = (iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyLayerRow::GetIsOptionsDisplayedCheckBoxState() const
{
	return mIsOptionsDisplayed ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE
