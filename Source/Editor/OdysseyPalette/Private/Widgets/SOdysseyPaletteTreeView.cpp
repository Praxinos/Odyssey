// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyPaletteTreeView.h"
#include "OdysseyStyleSet.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "ToolMenus.h"
#include "Framework/Commands/GenericCommands.h"
#include "ToolMenuContext.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Widgets/Views/STileView.h"

#define LOCTEXT_NAMESPACE "SOdysseyPaletteTreeView"

static FName contextMenuName = "OdysseyPaletteContextMenu";

SOdysseyPaletteTreeView::~SOdysseyPaletteTreeView()
{
    UOdysseyPalette::OnCurrentEntryChanged().RemoveAll(this);
    UOdysseyPalette::OnHierarchyChanged().RemoveAll(this);
	UOdysseyPaletteEntry::OnIsExpandedChanged().RemoveAll(this);
}

SOdysseyPaletteTreeView::SOdysseyPaletteTreeView()
    : mPalette(nullptr)
    , mCommandList(MakeShared<FUICommandList>())
{
    MapActionsToCommandList();
    UOdysseyPalette::OnCurrentEntryChanged().AddRaw(this, &SOdysseyPaletteTreeView::OnCurrentEntryChanged);
    UOdysseyPalette::OnHierarchyChanged().AddRaw(this, &SOdysseyPaletteTreeView::OnPaletteHierarchyChanged);
	UOdysseyPaletteEntry::OnIsExpandedChanged().AddRaw(this, &SOdysseyPaletteTreeView::OnEntryIsExpandedChanged);
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void SOdysseyPaletteTreeView::Construct(const FArguments& InArgs)
{
    mPalette = InArgs._Palette;

    TSharedRef<SHeaderRow> headerRow = SNew(SHeaderRow)
        .SplitterHandleSize(0.f) //Fixes alignment between header row and actual rows
        /* + SHeaderRow::Column("IsActivated")
            .ToolTipText(LOCTEXT("entries-tree-view.header-row.is-activated", "Toggle Entry Activation"))
            .FixedWidth(24.f)
            .HAlignHeader(HAlign_Center)
            .VAlignHeader(VAlign_Center)
            .HAlignCell(HAlign_Center)
            .VAlignCell(VAlign_Top)
            [
                SNew(SImage)
                .ColorAndOpacity(FSlateColor::UseForeground())
                .Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16"))
            ]*/
        + SHeaderRow::Column("Header")
            .DefaultLabel(FText())
            .VAlignCell(VAlign_Top)
            .FillWidth(InArgs._HeaderFillWidth)
            .FixedWidth(InArgs._HeaderFixedWidth)
            .ManualWidth(InArgs._HeaderManualWidth)
            .FillSized(InArgs._HeaderFillSized)
            [
                CreateSetWidget()
            ];

    for( SHeaderRow::FColumn::FArguments columnArguments : InArgs._AdditionalColumns)
    {
        headerRow->AddColumn(columnArguments);
    }

    const TArray<UOdysseyPaletteEntry*>* rootEntries = mPalette ? &mPalette->GetRootEntries() : nullptr;
    
    STreeView<UOdysseyPaletteEntry*>::Construct(
        STreeView<UOdysseyPaletteEntry*>::FArguments()
        .TreeItemsSource(rootEntries)
        .OnGenerateRow( InArgs._OnGenerateRow )
        .OnGetChildren( this, &SOdysseyPaletteTreeView::OnGetChildren )
        .OnExpansionChanged( this, &SOdysseyPaletteTreeView::OnExpansionChanged )
        .OnItemScrolledIntoView(this, &SOdysseyPaletteTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SOdysseyPaletteTreeView::OnContextMenuOpening )
        .SelectionMode( ESelectionMode::Multi )
        .HeaderRow(headerRow)
    );

    //Menus
    CreateContextMenu();
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides

int32
SOdysseyPaletteTreeView::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 EntryId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const
{
    int32 entryId = STreeView<UOdysseyPaletteEntry*>::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, EntryId, InWidgetStyle, bParentEnabled );

    if (mDisplayDropZone)
    {
        if (!ItemsSource || ItemsSource->Num() <= 0)
            return entryId;

        TSharedPtr<SMultiColumnTableRow<UOdysseyPaletteEntry*>> rowWidget = StaticCastSharedPtr<SMultiColumnTableRow<UOdysseyPaletteEntry*>>(WidgetFromItem(ItemsSource->Last()));
        if (!rowWidget)
            return entryId;

        const FTableRowStyle& style = FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("OdysseyLayerStack.AlternatedRows");
        const FSlateBrush* DropIndicatorBrush = &style.DropIndicator_Above;

        FGeometry geometry = rowWidget->GetPaintSpaceGeometry();
        const FVector2D& widgetSize = geometry.GetLocalSize();
        FVector2D translate(0.f, widgetSize.Y);
        FVector2D parentAbsolutePosition(geometry.AbsolutePosition.X, geometry.AbsolutePosition.Y);
        geometry = FGeometry(translate, parentAbsolutePosition, widgetSize, 1.0f);

        if (Private_GetOrientation() == Orient_Vertical)
        {
            FSlateDrawElement::MakeBox
            (
                OutDrawElements,
                entryId++,
                geometry.ToPaintGeometry(),
                DropIndicatorBrush,
                ESlateDrawEffect::None,
                DropIndicatorBrush->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
            );
        }
        else
        {
            // Reuse the drop indicator asset for horizontal, by rotating the drawn box 90 degrees.
            const FVector2D LocalSize(geometry.GetLocalSize());
            const FVector2D Pivot(LocalSize * 0.5f);
            const FVector2D RotatedLocalSize(LocalSize.Y, LocalSize.X);
            FSlateLayoutTransform RotatedTransform(Pivot - RotatedLocalSize * 0.5f);	// Make the box centered to the alloted geometry, so that it can be rotated around the center.

            FSlateDrawElement::MakeRotatedBox(
                OutDrawElements,
                entryId++,
                geometry.ToPaintGeometry(RotatedLocalSize, RotatedTransform),
                DropIndicatorBrush,
                ESlateDrawEffect::None,
                -HALF_PI,	// 90 deg CCW
                RotatedLocalSize * 0.5f,	// Relative center to the flipped
                FSlateDrawElement::RelativeToElement,
                DropIndicatorBrush->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
            );
        }
    }

	return entryId;
}

FReply
SOdysseyPaletteTreeView::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
	if (mCommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return STreeView<UOdysseyPaletteEntry*>::OnKeyDown(iGeometry, iKeyEvent);
}

FReply
SOdysseyPaletteTreeView::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    if ( !mPalette )
        return FReply::Unhandled();
    
    TSharedPtr<FOdysseyPaletteDragDropOperation> operation = DragDropEvent.GetOperationAs<FOdysseyPaletteDragDropOperation>();
    if (!operation)
        return FReply::Unhandled();

    UOdysseyPalette* operationPalette = operation->GetPalette();
	if ( !operationPalette)
		return FReply::Unhandled();

    if (!ItemsSource || ItemsSource->Num() <= 0)
        return FReply::Unhandled();

    TSharedPtr<SMultiColumnTableRow<UOdysseyPaletteEntry*>> rowWidget = StaticCastSharedPtr<SMultiColumnTableRow<UOdysseyPaletteEntry*>>(WidgetFromItem(ItemsSource->Last()));
    if (!rowWidget)
        return FReply::Unhandled();

    FGeometry geometry = rowWidget->GetTickSpaceGeometry();
    const FVector2D localPointerPos = geometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition());
    const FVector2D& widgetSize = geometry.GetLocalSize();

    //Don't display the dropzone if we don't drag under the last line
    if (localPointerPos.Y <= widgetSize.Y)
    {
        mDisplayDropZone = false;
        return FReply::Unhandled();
    }

    mDisplayDropZone = true;

	return FReply::Handled();
}

void
SOdysseyPaletteTreeView::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
    mDisplayDropZone = false;
}

FReply
SOdysseyPaletteTreeView::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    mDisplayDropZone = false;

    if ( !mPalette )
        return FReply::Unhandled();
    
    TSharedPtr<FOdysseyPaletteDragDropOperation> operation = DragDropEvent.GetOperationAs<FOdysseyPaletteDragDropOperation>();
    if (!operation)
        return FReply::Unhandled();

    UOdysseyPalette* operationPalette = operation->GetPalette();
	if ( !operationPalette)
		return FReply::Unhandled();

    TSharedPtr<SMultiColumnTableRow<UOdysseyPaletteEntry*>> rowWidget = StaticCastSharedPtr<SMultiColumnTableRow<UOdysseyPaletteEntry*>>(WidgetFromItem(ItemsSource->Last()));
    if (!rowWidget)
        return FReply::Unhandled();
    
    FGeometry geometry = rowWidget->GetTickSpaceGeometry();
    const FVector2D localPointerPos = geometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition());
    const FVector2D& widgetSize = geometry.GetLocalSize();

    //Don't drop if we don't drag under the last line
    if (localPointerPos.Y <= widgetSize.Y)
        return FReply::Unhandled();

    //do nothing
	FText copyEntriesTransactionName = LOCTEXT("tree-view.drag-drop.transaction.copy-entries", "Copy Entries");
	FText moveEntriesTransactionName = LOCTEXT("tree-view.drag-drop.transaction.move-entries", "Move Entries");
    TArray<UOdysseyPaletteEntry*> entries = operation->GetPaletteEntries();
    if ( operationPalette == mPalette ) //dropped from same Palette, do a move of topmost dropped entries
    {
        #ifdef WITH_EDITOR
            FScopedTransaction ScopedTransaction(moveEntriesTransactionName);
        #endif
        mPalette->MoveEntries(entries, nullptr, mPalette->GetRootEntries().Num());
    }
    else
    {
        #ifdef WITH_EDITOR
            FScopedTransaction ScopedTransaction(copyEntriesTransactionName);
        #endif
        mPalette->CopyEntries(entries, nullptr, mPalette->GetRootEntries().Num());
    }
	return FReply::Handled();
}

void
SOdysseyPaletteTreeView::ResetDropZone()
{
    mDisplayDropZone = false;
}

FReply SOdysseyPaletteTreeView::AddSetToPalette()
{
    mPalette->AddSet();
    mPaletteSetView->OnSetSelected( mPalette->Sets.Last() );

    return FReply::Handled();
}

//PRIVATE API-----------------------------------------------------------

void
SOdysseyPaletteTreeView::OnGetChildren(UOdysseyPaletteEntry* iParent, TArray<UOdysseyPaletteEntry*>& oChildren) const
{
    if (!mPalette)
        return;
    
    oChildren = iParent->GetChildren();
}

void
SOdysseyPaletteTreeView::OnPaletteHierarchyChanged(UOdysseyPalette* iPalette)
{
    if ( !mPalette )
        return;

	if ( iPalette != mPalette )
		return;

    RefreshAllExpansionStates();
    RequestTreeRefresh();
}

void
SOdysseyPaletteTreeView::SetCurrentEntryFromSelectorItem()
{
    if ( !mPalette )
        return;

	if ( mPalette->GetEntries().Num() == 0)
		return;

    if (!SelectorItem)
    {
        FOdysseyObjectEditorUtils::SetPropertyValue(mPalette, "CurrentEntry", TSoftObjectPtr<UOdysseyPaletteEntry>(mPalette->GetRootEntries()[0]));
        return;
    }

    UOdysseyPalette* selectorPalette = SelectorItem->GetPalette();
    if (selectorPalette != mPalette )
        return;

    if (SelectorItem == mPalette->CurrentEntry)
        return;

    FOdysseyObjectEditorUtils::SetPropertyValue(mPalette, "CurrentEntry", TSoftObjectPtr<UOdysseyPaletteEntry>(SelectorItem));
}

void SOdysseyPaletteTreeView::RefreshRootEntriesArray()
{
    
}

void SOdysseyPaletteTreeView::RefreshAllExpansionStates()
{
    if ( !mPalette )
        return;

    TArray<UOdysseyPaletteEntry*> entries = mPalette->GetEntries();
    for(UOdysseyPaletteEntry* entry : entries)
    {
        if(!entry)
            continue;

        SetItemExpansion(entry, entry->IsExpanded);
    }
}

TSharedRef<SWidget> SOdysseyPaletteTreeView::CreateSetWidget()
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .HAlign(HAlign_Fill)
        [
            SAssignNew(mPaletteSetView, SOdysseyPaletteSetView)
            .Palette(mPalette)
            .OnSetSelected(this, &SOdysseyPaletteTreeView::OnSetSelected)
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Top)
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.PreviousKey"))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyPaletteTreeView::AddSetToPalette)
        ];
 }

void
SOdysseyPaletteTreeView::Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo)
{
    if ( !mPalette )
    {
        STreeView< UOdysseyPaletteEntry* >::Private_SignalSelectionChanged(SelectInfo);
        return;
    }

    //Ensure selectorItem = currentEntry if currentEntry is selected
    UOdysseyPaletteEntry* currentEntry = mPalette->CurrentEntry.Get();
    if ( currentEntry && Private_IsItemSelected(currentEntry) )
    {
        Private_SetItemSelection(currentEntry, true, true);
    }
    else
    {
        if ( !Private_IsItemSelected(SelectorItem) )
            Private_SetItemSelection(SelectorItem, true, true);

        SetCurrentEntryFromSelectorItem();
    }

    STreeView< UOdysseyPaletteEntry* >::Private_SignalSelectionChanged(SelectInfo);
}

void
SOdysseyPaletteTreeView::OnCurrentEntryChanged(UOdysseyPalette* iPalette)
{
    if ( !mPalette )
        return;

    if (iPalette != mPalette )
        return;

    Private_ClearSelection();

    UOdysseyPaletteEntry* currentEntry = mPalette->CurrentEntry.Get();
    if( currentEntry )
    {
        Private_SetItemSelection(currentEntry, true, true);
        Private_SignalSelectionChanged(ESelectInfo::Direct);
    }
}

// ContextMenu

TSharedPtr<SWidget>
SOdysseyPaletteTreeView::OnContextMenuOpening()
{
    //Create a new command, so that we can add context menu specific entries 
    TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
    commandList->Append(mCommandList);

    //Allows us to extend the menu context by inserting entries everywhere we want
    //Overriding CreateContextMenu does not allow that
    TArray<TSharedPtr<FExtender>> extenders = ExtendContextMenu();
    TSharedPtr<FExtender> extender = FExtender::Combine(extenders);

    //Build menu
    FToolMenuContext menuContext(commandList, extender);
    return UToolMenus::Get()->GenerateWidget(contextMenuName, menuContext);
}

void SOdysseyPaletteTreeView::CreateContextMenu()
{
    UToolMenus* ToolMenus = UToolMenus::Get();
    if (!ensure(ToolMenus))
        return;
    
    if (ToolMenus->IsMenuRegistered(contextMenuName))
        return;

    UToolMenu* Menu = ToolMenus->RegisterMenu(contextMenuName);
    
    FToolMenuSection& selectionSection = Menu->AddSection("Selection", LOCTEXT("entries-tree-view.context-menu.selection-section", "Selection"));
    {
        selectionSection.AddMenuEntry(FGenericCommands::Get().SelectAll);
    }

    FToolMenuSection& commonSection = Menu->AddSection("Common", LOCTEXT("entries-tree-view.context-menu.common-section", "Common"));
    {
        commonSection.AddMenuEntry(FGenericCommands::Get().Delete);
        commonSection.AddMenuEntry(FGenericCommands::Get().Duplicate);
        commonSection.AddMenuEntry(FGenericCommands::Get().Rename);
    }
}

void
SOdysseyPaletteTreeView::OnSetSelected(FName iSet)
{
    int index = mPalette->Sets.Find(iSet);

    if( index >= 0 )
        mPalette->UsedSet = index;
}

TArray<TSharedPtr<FExtender>>
SOdysseyPaletteTreeView::ExtendContextMenu()
{
	return TArray< TSharedPtr<FExtender> >();
}

void
SOdysseyPaletteTreeView::MapActionsToCommandList()
{
    mCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw(this, &SOdysseyPaletteTreeView::SelectAllEntries)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(this, &SOdysseyPaletteTreeView::DeleteSelectedEntries),
        FCanExecuteAction::CreateRaw(this, &SOdysseyPaletteTreeView::CanDeleteSelectedEntries)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Duplicate,
        FExecuteAction::CreateRaw(this, &SOdysseyPaletteTreeView::DuplicateSelectedEntries)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Rename,
        FExecuteAction::CreateRaw(this, &SOdysseyPaletteTreeView::RenameCurrentEntry)
    );
}

// Commands

void
SOdysseyPaletteTreeView::SelectAllEntries()
{
    if ( !mPalette )
        return;

    //ItemsSource is the ListView::ItemsSource, which contains all displayed items, even deep children
    //It is NOT the same as TreeItemsSource or mRootEntries which only contain root elements
    SetItemSelection(*ItemsSource, true);
}

void
SOdysseyPaletteTreeView::DeleteSelectedEntries()
{
    if ( !mPalette )
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("tree-view.transaction.remove-selected-entries", "Remove Entries"));
#endif

    TArray<UOdysseyPaletteEntry*> selectedEntries = GetSelectedItems();
    mPalette->RemoveEntries(selectedEntries);
}

bool
SOdysseyPaletteTreeView::CanDeleteSelectedEntries()
{
    if ( !mPalette )
        return false;

    TArray<UOdysseyPaletteEntry*> selectedEntries = GetSelectedItems();
    if (selectedEntries.Num() <= 0)
        return false;

    //If one of the root layers is not selected, we can delete selected entries
    const TArray<UOdysseyPaletteEntry*>& rootEntries = mPalette->GetRootEntries();
    for (UOdysseyPaletteEntry* rootEntry : rootEntries)
    {
        if (!selectedEntries.Contains(rootEntry))
            return true;
    }
    
    return false;
}

void
SOdysseyPaletteTreeView::DuplicateSelectedEntries()
{
    if ( !mPalette )
        return;

    TArray<UOdysseyPaletteEntry*> selectedEntries = GetSelectedItems();
    if (selectedEntries.Num() <= 0)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("tree-view.transaction.duplicate-selected-entries", "Duplicate Entries"));
#endif

	TArray<UOdysseyPaletteEntry*> duplicatedEntries = mPalette->DuplicateEntries(selectedEntries);
    SetItemSelection(duplicatedEntries, true);
}

void
SOdysseyPaletteTreeView::RenameCurrentEntry()
{
    if ( !mPalette )
        return;

    if (!mPalette->CurrentEntry)
        return;
    
    mIsRenamePending = true; //has to come before ScrollItemIntoView() in case the item is already into view, which will trigger OnItemScrolledIntoView() immediately
	RequestScrollIntoView(mPalette->CurrentEntry.Get());
}
    
void
SOdysseyPaletteTreeView::OnEntryIsExpandedChanged(UOdysseyPaletteEntry* iEntryNode)
{
    if ( !mPalette )
        return;
    
    if ( iEntryNode->GetPalette() != mPalette )
        return;
    
    if( IsItemExpanded(Cast<UOdysseyPaletteEntry>(iEntryNode)) == iEntryNode->IsExpanded )
        return;

    SetItemExpansion(Cast<UOdysseyPaletteEntry>(iEntryNode), iEntryNode->IsExpanded);
}

void
SOdysseyPaletteTreeView::OnExpansionChanged( UOdysseyPaletteEntry* iEntryNode, bool iIsExpanded )
{
    FOdysseyObjectEditorUtils::SetPropertyValue(iEntryNode, "IsExpanded", iIsExpanded);
}

void
SOdysseyPaletteTreeView::OnItemScrolledIntoView(UOdysseyPaletteEntry* iEntry, const TSharedPtr<ITableRow>& iRow)
{
    if ( !mPalette )
        return;

    if (!iEntry || !iRow)
        return;

    if (mIsRenamePending && iEntry == mPalette->CurrentEntry)
    {
        TSharedPtr<SOdysseyPaletteEntryRow> entryRow = StaticCastSharedPtr<SOdysseyPaletteEntryRow>(iRow);
        entryRow->Rename();
        mIsRenamePending = false;
    }
}
    
TSharedPtr<FOdysseyPaletteDragDropOperation>
SOdysseyPaletteTreeView::CreateDragDropOperation() const
{
    if ( !mPalette )
        return nullptr;

    TSharedRef<FOdysseyPaletteDragDropOperation> operation =  MakeShared<FOdysseyPaletteDragDropOperation>(mPalette, GetSelectedItems());
	operation->Construct();
    return operation;
}

#undef LOCTEXT_NAMESPACE
