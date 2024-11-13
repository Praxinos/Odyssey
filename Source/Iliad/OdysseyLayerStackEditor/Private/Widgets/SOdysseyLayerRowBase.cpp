// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyLayerRowBase.h"
#include "OdysseyStyleSet.h"
#include "Widgets/SOdysseyLayerStackTreeView.h"
#include "OdysseyLayerStack.h"

#define LOCTEXT_NAMESPACE "LayerStackEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyLayerRowBase::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyLayer* iLayer)
{
    ensure(iLayer);
    mLayer = iLayer;
    mTreeView = iOwnerTableView;

    SMultiColumnTableRow<UOdysseyLayer*>::FArguments args;
    args.Style(&FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("OdysseyLayerStack.AlternatedRows"))
        .OnCanAcceptDrop(this, &SOdysseyLayerRowBase::OnRowCanAcceptDrop)
        .OnAcceptDrop(this, &SOdysseyLayerRowBase::OnRowAcceptDrop)
        .OnDragDetected(this, &SOdysseyLayerRowBase::OnRowDragDetected, TWeakPtr<SOdysseyLayerStackTreeView>(iOwnerTableView));

    SMultiColumnTableRow<UOdysseyLayer*>::Construct(
        args,
        iOwnerTableView
    );

    SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyLayerRowBase::GenerateWidgetForColumn( const FName& InColumnName )
{
    TSharedRef<SVerticalBox> verticalBox = SNew(SVerticalBox);
    TArray<FName> rows = GetLayer()->GetRows();
    FMargin columnPadding = GetColumnPadding(InColumnName);
    for (const FName& row : rows)
    {
        FMargin padding = GetRowPadding(row);
        padding.Left += columnPadding.Left;
        padding.Right += columnPadding.Right;
        if (row == rows[0])
        {
            padding.Top += columnPadding.Top;
        }
        if (row == rows.Last())
        {
            padding.Bottom += columnPadding.Bottom;
        }

        verticalBox->AddSlot()
        .AutoHeight()
        .Padding(padding)
        [
            SNew(SBox)
            .HeightOverride(this, &SOdysseyLayerRowBase::GetRowHeight, row)
            .Visibility(this, &SOdysseyLayerRowBase::GetRowVisibility, row)
            [
                GenerateWidget(row, InColumnName)
            ]
        ];
    }

    return verticalBox;
}

TSharedRef<SWidget>
SOdysseyLayerRowBase::GenerateWidget( const FName& iRow, const FName& iColumn )
{
    return SNullWidget::NullWidget;
}

FOptionalSize
SOdysseyLayerRowBase::GetRowHeight(FName iRow) const
{
    return GetLayer()->GetRowHeight(iRow);
}

EVisibility
SOdysseyLayerRowBase::GetRowVisibility(FName iRow) const
{
    return GetLayer()->IsRowVisible(iRow) ? EVisibility::Visible : EVisibility::Collapsed;
}

FMargin
SOdysseyLayerRowBase::GetRowPadding( FName iRow ) const
{
    return GetLayer()->GetRowPadding(iRow);
}

FMargin
SOdysseyLayerRowBase::GetColumnPadding( FName iColumn ) const
{
    return FMargin(0);
}

const FSlateBrush*
SOdysseyLayerRowBase::GetBorder() const
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

TSharedPtr<SOdysseyLayerStackTreeView>
SOdysseyLayerRowBase::GetTreeView() const
{
    return mTreeView.Pin();
}

UOdysseyLayer*
SOdysseyLayerRowBase::GetLayer() const
{
    return mLayer;
}

/** @return the zone (above, onto, below) based on where the user is hovering over within the row */
EItemDropZone
SOdysseyLayerRowBase::ComputeItemDropZoneForLeaf(FVector2D iLocalPointerPos, FVector2D iLocalSize, bool iCanHaveChildren, bool iIsExpanded)
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
SOdysseyLayerRowBase::OnRowCanAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyLayer* iLayer)
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
SOdysseyLayerRowBase::OnRowAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyLayer* iLayer)
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
SOdysseyLayerRowBase::OnRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent, TWeakPtr<SOdysseyLayerStackTreeView> iTreeView)
{
    return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE
