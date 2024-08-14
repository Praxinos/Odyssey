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
    mLayer = iLayer;
	mTreeView = iOwnerTableView;

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
                .ExpanderImageOpened(mLayer->IconExpanded.GetIcon())
                .ExpanderImageClosed(mLayer->Icon.GetIcon())
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
	return mLayer->DisplayOptions ? EVisibility::Visible : EVisibility::Collapsed;
}

TSharedPtr<SOdysseyLayerStackTreeView>
SOdysseyLayerRow::GetTreeView() const
{
	return mTreeView.Pin();
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
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, IsActivated), iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyLayerRow::GetIsActivatedCheckBoxState() const
{
	return mLayer->IsActivated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyLayerRow::OnIsLockedCheckBoxStateChanged(ECheckBoxState iState)
{
	FScopedTransaction ScopedTransaction(LOCTEXT("layer.transaction.set-is-locked", "Change Layer Lock"));
	FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, IsLocked), iState == ECheckBoxState::Checked);
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
    FScopedTransaction ScopedTransaction(LOCTEXT("layer.transaction.set-name", "Change Layer Name"));
	FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Name), iText);
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

bool
SOdysseyLayerRow::DisplayOptions() const
{
	return mLayer->DisplayOptions;
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

	if ( !mLayer )
		return TOptional<EItemDropZone>();

	UOdysseyLayerStack* layerStack = mLayer->GetLayerStack();
	if ( !layerStack )
		return TOptional<EItemDropZone>();
    //check if CanHaveChildren
    //allow Onto
    
	TSharedPtr<FOdysseyLayerStackDragDropOperation> operation = iEvent.GetOperationAs<FOdysseyLayerStackDragDropOperation>();
    if (!operation)
        return TOptional<EItemDropZone>();

	UOdysseyLayerStack* operationLayerStack = operation->GetLayerStack();
	if ( !operationLayerStack )
		return TOptional<EItemDropZone>();

	bool isNotSupported = operation->GetLayers().ContainsByPredicate(
		[layerStack](UOdysseyLayer* iLayer)
		{
			return !layerStack->SupportsLayerClass(iLayer->GetClass());
		}
	);

	if ( isNotSupported )
		return TOptional<EItemDropZone>();

	FGeometry geometry = GetTickSpaceGeometry();
	const FVector2D localPointerPos = geometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition());
	EItemDropZone expectedDropZone = ComputeItemDropZoneForLeaf(localPointerPos, geometry.GetLocalSize(), mLayer->CanHaveChildren, mLayer->DisplayChildren);

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
					return TOptional<EItemDropZone>();
			}
			break;

			case EItemDropZone::OntoItem:
			{
				if ( !operationLayerStack->CanMoveLayers(layers, parent) )
					return TOptional<EItemDropZone>();
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

	FText moveLayersTransactionName = LOCTEXT("layer.drag-drop.transaction.move-layers", "Move Layers");
	FText copyLayersTransactionName = LOCTEXT("layer.drag-drop.transaction.copy-layers", "Copy Layers");

	switch ( iDropZone )
	{
		case EItemDropZone::AboveItem:
		{
			//do nothing
			if ( operationLayerStack == layerStack ) //droped from same layerstack, do a move of topmost dropped layers
			{				
				#ifdef WITH_EDITOR
					FScopedTransaction ScopedTransaction(moveLayersTransactionName);
				#endif
				layerStack->MoveLayers(layers, parent, index);
			}
			else
			{
				#ifdef WITH_EDITOR
					FScopedTransaction ScopedTransaction(copyLayersTransactionName);
				#endif
				layerStack->CopyLayers(layers, parent, index);
			}
		}
		break;

		case EItemDropZone::OntoItem:
		{
			if ( operationLayerStack == layerStack ) //droped from same layerstack, do a move of topmost dropped layers
			{
				#ifdef WITH_EDITOR
					FScopedTransaction ScopedTransaction(moveLayersTransactionName);
				#endif
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
				#ifdef WITH_EDITOR
					FScopedTransaction ScopedTransaction(copyLayersTransactionName);
				#endif
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
				#ifdef WITH_EDITOR
					FScopedTransaction ScopedTransaction(moveLayersTransactionName);
				#endif
				layerStack->MoveLayers(layers, parent, index + 1);
			}
			else
			{
				#ifdef WITH_EDITOR
					FScopedTransaction ScopedTransaction(copyLayersTransactionName);
				#endif
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
SOdysseyLayerRow::OnDisplayOptionsCheckBoxStateChanged(ECheckBoxState iState)
{
	FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, DisplayOptions), iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyLayerRow::GetDisplayOptionsCheckBoxState() const
{
	return mLayer->DisplayOptions ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE
