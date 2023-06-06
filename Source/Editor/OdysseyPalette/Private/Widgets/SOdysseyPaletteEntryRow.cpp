// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPaletteEntryRow.h"
#include "OdysseyPaletteDragDropOperation.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "SOdysseyPaletteEntryRow"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyPaletteEntryRow::Construct(const FArguments& InArgs, const TSharedRef<STreeView<UOdysseyPaletteEntry*>>& iOwnerTableView, UOdysseyPaletteEntry* iEntry)
{	
    ensure(iEntry);
    mEntry = iEntry;

    SMultiColumnTableRow<UOdysseyPaletteEntry*>::FArguments args;
    args.Style(&FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("OdysseyLayerStack.AlternatedRows"))
        .OnCanAcceptDrop(this, &SOdysseyPaletteEntryRow::OnRowCanAcceptDrop)
        .OnAcceptDrop(this, &SOdysseyPaletteEntryRow::OnRowAcceptDrop)
        .OnDragDetected(this, &SOdysseyPaletteEntryRow::OnRowDragDetected, TWeakPtr<STreeView<UOdysseyPaletteEntry*>>(iOwnerTableView));

    SMultiColumnTableRow<UOdysseyPaletteEntry*>::Construct(
        args,
		iOwnerTableView
    );

	SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyPaletteEntryRow::GenerateWidgetForColumn( const FName& InColumnName )
{

    if (InColumnName == "IsActivated")
    {
        return GenerateIsActivatedWidget();
    }
	else if (InColumnName == "Header")
	{
		return GenerateHeaderWidget();
	}

	return SNullWidget::NullWidget;
}


const FSlateBrush*
SOdysseyPaletteEntryRow::GetBorder() const 
{
    const FSlateBrush* borderBrush = SMultiColumnTableRow<UOdysseyPaletteEntry*>::GetBorder();

    if (!mEntry) 
        return borderBrush;

	UOdysseyPalette* palette = mEntry->GetPalette();
	if ( !palette || palette->CurrentEntry != mEntry)
		return borderBrush;

    const bool bIsActive = OwnerTablePtr.Pin()->AsWidget()->HasKeyboardFocus();
    return bIsActive ? FOdysseyStyle::GetBrush("OdysseyLayerStack.CurrentLayerBackgroundBrush") : FOdysseyStyle::GetBrush("OdysseyLayerStack.CurrentLayerInactiveBackgroundBrush");
}


TSharedRef<SWidget>
SOdysseyPaletteEntryRow::GenerateHeaderWidget()
{
    return SAssignNew(mNameWidget, SInlineEditableTextBlock)
        .Text(this, &SOdysseyPaletteEntryRow::GetEntryName)
        .Font(this, &SOdysseyPaletteEntryRow::GetEntryNameFont)
        .OnTextCommitted(this, &SOdysseyPaletteEntryRow::OnEntryNameCommited)
        .IsSelected(this, &SOdysseyPaletteEntryRow::IsSelectedExclusively); //Allows edition to work
}

TSharedRef<SWidget>
SOdysseyPaletteEntryRow::GenerateIsActivatedWidget()
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
		//.OnCheckStateChanged(this, &SOdysseyPaletteEntryRow::OnIsActivatedCheckBoxStateChanged)
		//.IsChecked(this, &SOdysseyPaletteEntryRow::GetIsActivatedCheckBoxState)
		[
			//Just for the checkbox to take the space of an icon
			SNew(SImage)
				.Visibility(EVisibility::Hidden)
				.Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16"))
		];
}

/*
void
SOdysseyPaletteEntryRow::OnIsActivatedCheckBoxStateChanged(ECheckBoxState iState)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Active"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mEntry, "IsActivated", iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyPaletteEntryRow::GetIsActivatedCheckBoxState() const
{
	return mEntry->IsActivated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyPaletteEntryRow::OnIsLockedCheckBoxStateChanged(ECheckBoxState iState)
{
	FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Lock"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mEntry, "IsLocked", iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyPaletteEntryRow::GetIsLockedCheckBoxState() const
{
	return mEntry->IsLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}
*/

FText
SOdysseyPaletteEntryRow::GetEntryName() const
{
    return mEntry->EntryName;
}

FSlateFontInfo
SOdysseyPaletteEntryRow::GetEntryNameFont() const
{
    return FAppStyle::Get().GetFontStyle("NormalFontBold");
}

/*
UOdysseyLayer*
SOdysseyPaletteEntryRow::GetLayer()
{
    return mEntry;
}
*/
void
SOdysseyPaletteEntryRow::OnEntryNameCommited(const FText& iText, ETextCommit::Type iType)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("PaletteTransaction", "Change Entry Name"));
	FOdysseyObjectEditorUtils::SetPropertyValue(mEntry, "EntryName", iText);
}

void
SOdysseyPaletteEntryRow::Rename()
{
	mNameWidget->EnterEditingMode();
}

EItemDropZone
SOdysseyPaletteEntryRow::ComputeItemDropZoneForLeaf(FVector2D iLocalPointerPos, FVector2D iLocalSize, bool iCanHaveChildren, bool iIsExpanded)
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
SOdysseyPaletteEntryRow::OnRowCanAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyPaletteEntry* iEntry)
{
    //StaticCastSharedPtr<STreeView<UOdysseyPaletteEntry*>>(OwnerTablePtr.Pin())->ResetDropZone();

    EItemDropZone emptyDropZone;
	if ( !mEntry )
		return emptyDropZone;

	UOdysseyPalette* palette = mEntry->GetPalette();
	if ( !palette )
		return emptyDropZone;
    //check if CanHaveChildren
    //allow Onto
    
	TSharedPtr<FOdysseyPaletteDragDropOperation> operation = iEvent.GetOperationAs<FOdysseyPaletteDragDropOperation>();
    if (!operation)
        return emptyDropZone;

	UOdysseyPalette* operationLayerStack = operation->GetPalette();
	if ( !operationLayerStack )
		return emptyDropZone;

	FGeometry geometry = GetTickSpaceGeometry();
	const FVector2D localPointerPos = geometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition());
	EItemDropZone expectedDropZone = ComputeItemDropZoneForLeaf(localPointerPos, geometry.GetLocalSize(), false, true); //mEntry->CanHaveChildren, mEntry->IsExpanded);

	if ( operationLayerStack == palette ) //droped from same layerstack, do a move of topmost dropped layers
	{
		UOdysseyPaletteEntry* parent = mEntry;//mEntry->GetParent();
		TArray<UOdysseyPaletteEntry*> entries = operation->GetPaletteEntries();
		/*switch (expectedDropZone)
		{
			case EItemDropZone::AboveItem:
			case EItemDropZone::BelowItem:
			{
				if ( !operationLayerStack->CanMoveLayers(entries, parent) )
					return emptyDropZone;
			}
			break;

			case EItemDropZone::OntoItem:
			{
				if ( !operationLayerStack->CanMoveLayers(entries, parent) )
					return emptyDropZone;
			}
			break;
		}*/
	}

	return expectedDropZone;
}

FReply
SOdysseyPaletteEntryRow::OnRowAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyPaletteEntry* iEntry)
{
    //StaticCastSharedPtr<STreeView<UOdysseyPaletteEntry*>>(OwnerTablePtr.Pin())->ResetDropZone();

	TOptional<EItemDropZone> dropZone = OnRowCanAcceptDrop(iEvent, iDropZone, iEntry);
    if (!dropZone.IsSet())
        return FReply::Unhandled();

	TSharedPtr<FOdysseyPaletteDragDropOperation> operation = iEvent.GetOperationAs<FOdysseyPaletteDragDropOperation>();
    if (!operation)
        return FReply::Unhandled();

    UOdysseyPalette* palette = mEntry->GetPalette();
	if ( !palette )
		return FReply::Unhandled();

	UOdysseyPalette* operationPalette = operation->GetPalette();
	if ( !operationPalette )
		return FReply::Unhandled();

	UOdysseyPaletteEntry* parent = mEntry;//mEntry->GetParent();
    TArray<UOdysseyPaletteEntry*> entries = operation->GetPaletteEntries();
	int index = 0;//mEntry->GetIndexInParent();

	switch ( iDropZone )
	{
		case EItemDropZone::AboveItem:
		{
			//do nothing
			if ( operationPalette == palette ) //dropped from same Palette, do a move of topmost dropped layers
			{
				//palette->MoveLayers(entries, parent, index);
			}
			else
			{
				//palette->CopyLayers(entries, parent, index);
			}
		}
		break;

		case EItemDropZone::OntoItem:
		{
			if ( operationPalette == palette ) //droped from same layerstack, do a move of topmost dropped layers
			{
				/*if (mEntry->CanHaveChildren)
				{
					palette->MoveLayers(entries, mEntry, 0);
				}
				else
				{
					palette->MoveLayers(entries, parent, index);
				}*/
			}
			else
			{
				/*if (mEntry->CanHaveChildren)
				{
					palette->CopyLayers(entries, mEntry, 0);
				}
				else
				{
					palette->CopyLayers(entries, parent, index);
				}*/
			}
		}
		break;

		case EItemDropZone::BelowItem:
		{
			/*if (operationPalette == palette) //droped from same layerstack, do a move of topmost dropped layers
			{
				palette->MoveLayers(entries, parent, index + 1);
			}
			else
			{
				palette->CopyLayers(entries, parent, index + 1);
			}*/
		}
		break;
	}

    return FReply::Handled();
}


FReply
SOdysseyPaletteEntryRow::OnRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent, TWeakPtr<STreeView<UOdysseyPaletteEntry*>> iTreeView)
{
    TSharedPtr<STreeView<UOdysseyPaletteEntry*>> treeView = iTreeView.Pin();
	if (treeView.IsValid() && iEvent.IsMouseButtonDown( EKeys::LeftMouseButton ))
	{
		//TSharedPtr<FOdysseyLayerStackDragDropOperation> operation = treeView->CreateDragDropOperation();
        //TSharedPtr<FOdysseyPaletteDragDropOperation> operation = MakeShared<FOdysseyPaletteDragDropOperation>( mPalette, treeView->GetSelectedItems());
        TSharedPtr<FOdysseyPaletteDragDropOperation> operation = MakeShared<FOdysseyPaletteDragDropOperation>( mEntry->GetPalette(), treeView->GetSelectedItems());

		operation->Construct();

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

/*
void
SOdysseyPaletteEntryRow::OnIsOptionsDisplayedCheckBoxStateChanged(ECheckBoxState iState)
{
	mIsOptionsDisplayed = (iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyPaletteEntryRow::GetIsOptionsDisplayedCheckBoxState() const
{
	return mIsOptionsDisplayed ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}
*/
#undef LOCTEXT_NAMESPACE
