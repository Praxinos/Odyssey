// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyPaletteEntryRow.h"
#include "OdysseyPaletteDragDropOperation.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyle.h"
#include "OdysseyPalette.h"
#include "Widgets/Input/SCheckBox.h"
#include "OdysseyPaletteEntryColor.h"
#include "Widgets/SOdysseyPaletteTreeView.h"

#define LOCTEXT_NAMESPACE "Palette"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyPaletteEntryRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyPaletteTreeView>& iTreeView, UOdysseyPaletteEntry* iEntry)
{
    ensure(iEntry);
    mEntry = iEntry;
    mTreeView = iTreeView;
    mIsReadOnly = InArgs._IsReadOnly;

    SMultiColumnTableRow<UOdysseyPaletteEntry*>::FArguments args;
    args.Style(&FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("OdysseyLayerStack.AlternatedRows"))
        .Padding(FMargin(0, 2, 0, 2))
        .OnCanAcceptDrop(this, &SOdysseyPaletteEntryRow::OnRowCanAcceptDrop)
        .OnAcceptDrop(this, &SOdysseyPaletteEntryRow::OnRowAcceptDrop)
        .OnDragDetected(this, &SOdysseyPaletteEntryRow::OnRowDragDetected);

    SMultiColumnTableRow<UOdysseyPaletteEntry*>::Construct(
        args,
        iTreeView
    );
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyPaletteEntryRow::GenerateWidgetForColumn( const FName& InColumnName )
{
    if (InColumnName == "Header")
    {
        return GenerateHeaderWidget();
    }

    return SNullWidget::NullWidget;
}

const FSlateBrush*
SOdysseyPaletteEntryRow::GetIcon() const
{
    return FOdysseyStyle::Get().GetBrush("OdysseyPalette.EntryColor");
}

FSlateColor
SOdysseyPaletteEntryRow::GetIconColorAndOpacity() const
{
    return FLinearColor::White;
}

TSharedRef<SWidget>
SOdysseyPaletteEntryRow::GenerateHeaderWidget()
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Fill)
        [
            SNew(SExpanderArrow, SharedThis(this) )
            .ShouldDrawWires(true)
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SNew(SImage)
            .Image(this, &SOdysseyPaletteEntryRow::GetIcon)
            .ColorAndOpacity(this, &SOdysseyPaletteEntryRow::GetIconColorAndOpacity)
        ]
        + SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
        [
            SAssignNew(mNameWidget, SInlineEditableTextBlock)
            .Text_Lambda(
                [this]()
                {
                    return mEntry->EntryName;
                }
            )
            .Font_Lambda(
                [this]()
                {
                    TSharedPtr<SOdysseyPaletteTreeView> treeView = mTreeView.Pin();
                    if (!treeView)
                        return FAppStyle::Get().GetFontStyle("NormalFont");

                    if (mEntry == treeView->GetCurrentColorEntry())
                        return FAppStyle::Get().GetFontStyle("NormalFontBold");

                    return FAppStyle::Get().GetFontStyle("NormalFont");
                }
            )
            .OnTextCommitted(this, &SOdysseyPaletteEntryRow::OnEntryNameCommited)
            .IsSelected(this, &SOdysseyPaletteEntryRow::IsSelectedExclusively) //Allows edition to work
            .IsReadOnly(mIsReadOnly)
        ];
}

void
SOdysseyPaletteEntryRow::OnEntryNameCommited(const FText& iText, ETextCommit::Type iType)
{
    if (mIsReadOnly)
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("entry-row.set-name", "Change Entry Name"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mEntry, GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntry, EntryName), iText);
}

void
SOdysseyPaletteEntryRow::Rename()
{
    if (mIsReadOnly)
        return;

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
    StaticCastSharedPtr<SOdysseyPaletteTreeView>(OwnerTablePtr.Pin())->ResetDropZone();

    EItemDropZone emptyDropZone;
    if (mIsReadOnly)
        return emptyDropZone;

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

    UOdysseyPalette* operationPalette = operation->GetPalette();
    if ( !operationPalette )
        return emptyDropZone;

    FGeometry geometry = GetTickSpaceGeometry();
    const FVector2D localPointerPos = geometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition());
    EItemDropZone expectedDropZone = ComputeItemDropZoneForLeaf(localPointerPos, geometry.GetLocalSize(), mEntry->CanHaveChildren, mEntry->IsExpanded);

    if ( operationPalette == palette ) //dropped from same palette, do a move of topmost dropped entries
    {
        UOdysseyPaletteEntry* parent = mEntry->GetParent();
        TArray<UOdysseyPaletteEntry*> entries = operation->GetPaletteEntries();
        switch (expectedDropZone)
        {
            case EItemDropZone::AboveItem:
            case EItemDropZone::BelowItem:
            {
                if ( !operationPalette->CanMoveEntries(entries, parent) )
                    return emptyDropZone;
            }
            break;

            case EItemDropZone::OntoItem:
            {
                if ( !operationPalette->CanMoveEntries(entries, parent) )
                    return emptyDropZone;
            }
            break;
        }
    }

    return expectedDropZone;
}

FReply
SOdysseyPaletteEntryRow::OnRowAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyPaletteEntry* iEntry)
{
    StaticCastSharedPtr<SOdysseyPaletteTreeView>(OwnerTablePtr.Pin())->ResetDropZone();
    if (mIsReadOnly)
        return FReply::Unhandled();

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

    UOdysseyPaletteEntry* parent = mEntry->GetParent();
    TArray<UOdysseyPaletteEntry*> entries = operation->GetPaletteEntries();
    int index = mEntry->GetIndexInParent();

    FText copyEntriesTransactionName = LOCTEXT("entry-row.drag-drop.transaction.copy-entries", "Copy Entries");
    FText moveEntriesTransactionName = LOCTEXT("entry-row.transaction.move-entries", "Move Entries");

    switch ( iDropZone )
    {
        case EItemDropZone::AboveItem:
        {
            #if WITH_EDITOR
                FScopedTransaction ScopedTransaction(moveEntriesTransactionName);
            #endif
            //do nothing
            if ( operationPalette == palette ) //dropped from same Palette, do a move of topmost dropped entries
            {
                #if WITH_EDITOR
                    for (UOdysseyPaletteEntry* entry : entries)
                    {
                        bool bChangeParent = entry->Parent != parent;

                        if (bChangeParent)
                        {
                            FOdysseyObjectEditorUtils::PreChangePropertyValue(entry, "Parent");
                            FOdysseyObjectEditorUtils::PreChangePropertyValue(entry->Parent, "Children");
                        }
                        FOdysseyObjectEditorUtils::PreChangePropertyValue(parent, "Children");
                    }
                #endif

                palette->MoveEntries(entries, parent, index);

                #if WITH_EDITOR
                    for (UOdysseyPaletteEntry* entry : entries)
                    {
                        bool bChangeParent = entry->Parent != parent;

                        if (bChangeParent)
                        {
                            FOdysseyObjectEditorUtils::PostChangePropertyValue(entry, "Parent", EPropertyChangeType::ValueSet);
                            FOdysseyObjectEditorUtils::PostChangePropertyValue(entry, "Children", EPropertyChangeType::ArrayRemove);
                        }
                        FOdysseyObjectEditorUtils::PostChangePropertyValue(parent, "Children", EPropertyChangeType::ArrayAdd);
                    }
                #endif
            }
            else
            {
                palette->CopyEntries(entries, parent, index);
            }
        }
        break;

        case EItemDropZone::OntoItem:
        {
            if ( operationPalette == palette ) //dropped from same palette, do a move of topmost dropped entries
            {
                #if WITH_EDITOR
                    FScopedTransaction ScopedTransaction(moveEntriesTransactionName);
                #endif

                if (mEntry->CanHaveChildren)
                {
                    #if WITH_EDITOR
                        for (UOdysseyPaletteEntry* entry : entries)
                        {
                            bool bChangeParent = entry->Parent != mEntry;

                            if (bChangeParent)
                            {
                                FOdysseyObjectEditorUtils::PreChangePropertyValue(entry, "Parent");
                                FOdysseyObjectEditorUtils::PreChangePropertyValue(entry->Parent, "Children");
                            }
                            FOdysseyObjectEditorUtils::PreChangePropertyValue(mEntry, "Children");
                        }
                    #endif

                    palette->MoveEntries(entries, mEntry, 0);

                    #if WITH_EDITOR
                        for (UOdysseyPaletteEntry* entry : entries)
                        {
                            bool bChangeParent = entry->Parent != mEntry;

                            if (bChangeParent)
                            {
                                FOdysseyObjectEditorUtils::PostChangePropertyValue(entry, "Parent", EPropertyChangeType::ValueSet);
                                FOdysseyObjectEditorUtils::PostChangePropertyValue(entry, "Children", EPropertyChangeType::ArrayRemove);
                            }
                            FOdysseyObjectEditorUtils::PostChangePropertyValue(mEntry, "Children", EPropertyChangeType::ArrayAdd);
                        }
                    #endif
                }
                else
                {
                    #if WITH_EDITOR
                        for (UOdysseyPaletteEntry* entry : entries)
                        {
                            bool bChangeParent = entry->Parent != parent;

                            if (bChangeParent)
                            {
                                FOdysseyObjectEditorUtils::PreChangePropertyValue(entry, "Parent");
                                FOdysseyObjectEditorUtils::PreChangePropertyValue(entry->Parent, "Children");
                            }
                            FOdysseyObjectEditorUtils::PreChangePropertyValue(parent, "Children");
                        }
                    #endif

                    palette->MoveEntries(entries, parent, index);

                    #if WITH_EDITOR
                        for (UOdysseyPaletteEntry* entry : entries)
                        {
                            bool bChangeParent = entry->Parent != parent;

                            if (bChangeParent)
                            {
                                FOdysseyObjectEditorUtils::PostChangePropertyValue(entry, "Parent", EPropertyChangeType::ValueSet);
                                FOdysseyObjectEditorUtils::PostChangePropertyValue(entry, "Children", EPropertyChangeType::ArrayRemove);
                            }
                            FOdysseyObjectEditorUtils::PostChangePropertyValue(parent, "Children", EPropertyChangeType::ArrayAdd);
                        }
                    #endif
                }
            }
            else
            {
                #if WITH_EDITOR
                    FScopedTransaction ScopedTransaction(copyEntriesTransactionName);
                #endif
                if (mEntry->CanHaveChildren)
                {
                    palette->CopyEntries(entries, mEntry, 0);
                }
                else
                {
                    palette->CopyEntries(entries, parent, index);
                }
            }
        }
        break;

        case EItemDropZone::BelowItem:
        {
            if (operationPalette == palette) //droped from same palette, do a move of topmost dropped entries
            {
                #if WITH_EDITOR
                    FScopedTransaction ScopedTransaction(moveEntriesTransactionName);
                #endif

                #if WITH_EDITOR
                    for (UOdysseyPaletteEntry* entry : entries)
                    {
                        bool bChangeParent = entry->Parent != parent;

                        if (bChangeParent)
                        {
                            FOdysseyObjectEditorUtils::PreChangePropertyValue(entry, "Parent");
                            FOdysseyObjectEditorUtils::PreChangePropertyValue(entry->Parent, "Children");
                        }
                        FOdysseyObjectEditorUtils::PreChangePropertyValue(parent, "Children");
                    }
                #endif

                palette->MoveEntries(entries, parent, index + 1);

                #if WITH_EDITOR
                    for (UOdysseyPaletteEntry* entry : entries)
                    {
                        bool bChangeParent = entry->Parent != parent;

                        if (bChangeParent)
                        {
                            FOdysseyObjectEditorUtils::PostChangePropertyValue(entry, "Parent", EPropertyChangeType::ValueSet);
                            FOdysseyObjectEditorUtils::PostChangePropertyValue(entry, "Children", EPropertyChangeType::ArrayRemove);
                        }
                        FOdysseyObjectEditorUtils::PostChangePropertyValue(parent, "Children", EPropertyChangeType::ArrayAdd);
                    }
                #endif
            }
            else
            {
                #if WITH_EDITOR
                    FScopedTransaction ScopedTransaction(copyEntriesTransactionName);
                #endif
                palette->CopyEntries(entries, parent, index + 1);
            }
        }
        break;
    }

    return FReply::Handled();
}


FReply
SOdysseyPaletteEntryRow::OnRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (mIsReadOnly)
        return FReply::Unhandled();

    TSharedPtr<SOdysseyPaletteTreeView> treeView = mTreeView.Pin();
    if (treeView.IsValid() && iEvent.IsMouseButtonDown( EKeys::LeftMouseButton ))
    {
        TSharedPtr<FOdysseyPaletteDragDropOperation> operation = treeView->CreateDragDropOperation();

        if (operation.IsValid())
        {
            //PATCH: Unreal does not manage collision between SInlineWidget entering editiong mode
            //         and Drag'n'Drop, which can lead to cases where after dropping an element,
            //       an entry will start editing its name, which is not an expected behaviour
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

#undef LOCTEXT_NAMESPACE
