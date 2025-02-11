// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyPaletteTreeView.h"

#include "OdysseyStyleSet.h"

#include "OdysseyPalette.h"
#include "OdysseyPaletteEntryColor.h"
#include "OdysseyPaletteEntryFolder.h"

#include "SOdysseyPaletteEntryRow.h"
#include "SOdysseyPaletteFolderRow.h"
#include "SOdysseyPaletteColorRow.h"
#include "SOdysseyPaletteAddEntryButton.h"

#include "ToolMenus.h"
#include "Framework/Commands/GenericCommands.h"
#include "ToolMenuContext.h"
#include "UObject/SavePackage.h"
#include "ISinglePropertyView.h"


#define LOCTEXT_NAMESPACE "Palette"

static FName contextMenuName = "OdysseyPaletteContextMenu";

SLATE_IMPLEMENT_WIDGET(SOdysseyPaletteTreeView)
void
SOdysseyPaletteTreeView::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mPaletteAttribute, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyPaletteTreeView&>(Widget).OnPaletteChanged();
        }
    ));

    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mCurrentColorEntryAttribute, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyPaletteTreeView&>(Widget).OnCurrentColorEntryChanged();
        }
    ));

    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mCurrentSetAttribute, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyPaletteTreeView&>(Widget).OnCurrentSetChanged();
        }
    ));
}

SOdysseyPaletteTreeView::~SOdysseyPaletteTreeView()
{
    UOdysseyPalette::OnHierarchyChanged().RemoveAll(this);
    UOdysseyPalette::OnSetsChanged().RemoveAll(this);
}

SOdysseyPaletteTreeView::SOdysseyPaletteTreeView()
    : mPaletteAttribute(*this, nullptr)
    , mPalette(nullptr)
    , mCurrentColorEntryAttribute(*this, nullptr)
    , mCurrentColorEntry(nullptr)
    , mCurrentSetAttribute(*this, 0)
    , mCurrentSet(0)
    , mSelectedEntry(nullptr)
    , mCommandList(MakeShared<FUICommandList>())
{
    MapActionsToCommandList();
    UOdysseyPalette::OnHierarchyChanged().AddRaw(this, &SOdysseyPaletteTreeView::OnPaletteHierarchyChanged);
    UOdysseyPalette::OnSetsChanged().AddRaw(this, &SOdysseyPaletteTreeView::OnPaletteSetsChanged);
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void SOdysseyPaletteTreeView::Construct(const FArguments& InArgs)
{
    mPaletteAttribute.Assign(*this, InArgs._Palette);
    mPalette = mPaletteAttribute.Get();
    mCurrentColorEntryAttribute.Assign(*this, InArgs._CurrentColorEntry);
    mCurrentColorEntry = mCurrentColorEntryAttribute.Get();
    mCurrentSetAttribute.Assign(*this, InArgs._CurrentSet);
    mCurrentSet = mCurrentSetAttribute.Get();
    mSelectedEntry = mCurrentColorEntry;
    mOnCurrentColorEntrySelected = InArgs._OnCurrentColorEntrySelected;
    mOnCurrentSetSelected = InArgs._OnCurrentSetSelected;

    mItemsSource = MakeShared<UE::Slate::Containers::TObservableArray<UOdysseyPaletteEntry*>>();
    mSetsSource = MakeShared<UE::Slate::Containers::TObservableArray<TSharedPtr<int>>>();

    RefreshItemsSource();
    RefreshSetsSource();

    TSharedRef<SHeaderRow> headerRow = SNew(SHeaderRow)
        + SHeaderRow::Column("Header")
        .DefaultLabel(FText())
        .VAlignCell(VAlign_Top)
        .FillWidth(0.3f)
        [
            CreateHeaderColumnHeaderContent()
        ]

        + SHeaderRow::Column("Color")
        .DefaultLabel(FText())
        .VAlignCell(VAlign_Top)
        .FillWidth(0.6f)
        [
            CreateColorColumnHeaderContent()
        ];

    STreeView<UOdysseyPaletteEntry*>::Construct(
        STreeView<UOdysseyPaletteEntry*>::FArguments()
        .TreeItemsSource(mItemsSource)
        .SelectionMode( ESelectionMode::Multi )
        .OnGenerateRow( this, &SOdysseyPaletteTreeView::OnGenerateRow )
        .OnGetChildren( this, &SOdysseyPaletteTreeView::OnGetChildren )
        .OnSelectionChanged(this, &SOdysseyPaletteTreeView::OnSelectionChanged)
        .OnItemScrolledIntoView(this, &SOdysseyPaletteTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SOdysseyPaletteTreeView::OnContextMenuOpening )
        .HeaderRow(headerRow)
    );

    if (mCurrentColorEntry)
        SetItemSelection(mCurrentColorEntry, true);

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
        if ( GetRootItems().Num() == 0 )
            return entryId;

        TSharedPtr<SMultiColumnTableRow<UOdysseyPaletteEntry*>> rowWidget = StaticCastSharedPtr<SMultiColumnTableRow<UOdysseyPaletteEntry*>>(WidgetFromItem(GetRootItems().Last()));
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
            FSlateLayoutTransform RotatedTransform(Pivot - RotatedLocalSize * 0.5f);    // Make the box centered to the alloted geometry, so that it can be rotated around the center.

            FSlateDrawElement::MakeRotatedBox(
                OutDrawElements,
                entryId++,
                geometry.ToPaintGeometry(RotatedLocalSize, RotatedTransform),
                DropIndicatorBrush,
                ESlateDrawEffect::None,
                -HALF_PI,    // 90 deg CCW
                RotatedLocalSize * 0.5f,    // Relative center to the flipped
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

    if (GetRootItems().Num() == 0)
        return FReply::Unhandled();

    TSharedPtr<SMultiColumnTableRow<UOdysseyPaletteEntry*>> rowWidget = StaticCastSharedPtr<SMultiColumnTableRow<UOdysseyPaletteEntry*>>(WidgetFromItem(GetRootItems().Last()));
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

    TSharedPtr<SMultiColumnTableRow<UOdysseyPaletteEntry*>> rowWidget = StaticCastSharedPtr<SMultiColumnTableRow<UOdysseyPaletteEntry*>>(WidgetFromItem(GetRootItems().Last()));
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
    mOnCurrentSetSelected.ExecuteIfBound(mPalette->Sets.Num() - 1);
    return FReply::Handled();
}

FReply SOdysseyPaletteTreeView::SavePalette()
{
    if (!mPalette)
        return FReply::Unhandled();

    UPackage* package = mPalette->GetOutermost();
    mPalette->MarkPackageDirty();

    FSavePackageArgs packageArgs;
    packageArgs.SaveFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone | EObjectFlags::RF_HasExternalPackage;
    FString packageFileName = FPackageName::LongPackageNameToFilename(package->GetName(), FPackageName::GetAssetPackageExtension());

    bool isSaved = UPackage::SavePackage( package, nullptr, *packageFileName, packageArgs );

    if( isSaved )
        return FReply::Handled();
    else
        return FReply::Unhandled();
}

//PRIVATE API-----------------------------------------------------------

void
SOdysseyPaletteTreeView::OnGetChildren(UOdysseyPaletteEntry* iEntry, TArray<UOdysseyPaletteEntry*>& oChildren) const
{
    if (!mPalette)
        return;

    TArray<UOdysseyPaletteEntry*> children = iEntry->GetChildren();
    TArray<UOdysseyPaletteEntry*> colorAndFolderEntries;
    for ( UOdysseyPaletteEntry* entry : children )
    {
        if (!entry || (!entry->IsA<UOdysseyPaletteEntryColor>() && !entry->IsA<UOdysseyPaletteEntryFolder>()))
            continue;

        colorAndFolderEntries.Add(entry);
    }

    oChildren = colorAndFolderEntries;
}

void
SOdysseyPaletteTreeView::OnPaletteHierarchyChanged(UOdysseyPalette* iPalette)
{
    if (iPalette != mPalette)
        return;

    RefreshItemsSource();
}

TSharedRef<SWidget> SOdysseyPaletteTreeView::CreateHeaderColumnHeaderContent()
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("Button.NoPadding"))
            .OnClicked(this, &SOdysseyPaletteTreeView::AddColorEntry )
            [
                SNew(SImage)
                .Image(FOdysseyStyle::GetBrush("OdysseyPalette.AddColor"))
            ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("Button.NoPadding"))
            .OnClicked(this, &SOdysseyPaletteTreeView::AddFolderEntry)
            [
                SNew(SImage).Image(FOdysseyStyle::GetBrush("OdysseyPalette.AddFolder"))
            ]
        ]
}

TSharedRef<SWidget> SOdysseyPaletteTreeView::CreateColorColumnHeaderContent()
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .HAlign(HAlign_Fill)
        [
            SAssignNew(mSetComboBox, SComboBox<TSharedPtr<int>>)
            .InitiallySelectedItem(GetCurrentSet())
            .OptionsSource(mSetsSource)
            .OnSelectionChanged(this, &SOdysseyPaletteTreeView::OnSetSelectionChanged)
            .OnGenerateWidget(this, &SOdysseyPaletteTreeView::OnGenerateSetWidget)
            [
                SNew(STextBlock)
                .Text_Lambda(
                    [this]()
                    {
                        if (!mPalette)
                            return FText::GetEmpty();

                        if (mCurrentSet < 0 || mCurrentSet >= mPalette->Sets.Num())
                            return FText::GetEmpty();

                        return FText::FromName(mPalette->Sets[mCurrentSet]);
                    }
                )
            ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Top)
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("Button.NoPadding"))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyPaletteTreeView::AddSetToPalette)
            [
                SNew(SImage).Image(FOdysseyStyle::GetBrush("OdysseyPalette.AddSet"))
            ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Top)
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("Button.NoPadding"))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyPaletteTreeView::SavePalette)
            [
                SNew(SImage).Image(FOdysseyStyle::GetBrush("OdysseyPalette.Save"))
            ]
        ];
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
    SetItemSelection( SListView<UOdysseyPaletteEntry*>::GetItems(), true);
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



TSharedRef<ITableRow>
SOdysseyPaletteTreeView::OnGenerateRow(UOdysseyPaletteEntry* iEntry, const TSharedRef<STableViewBase>& iOwnerTable)
{
    if (iEntry->IsA<UOdysseyPaletteEntryFolder>())
        return SNew(SOdysseyPaletteFolderRow, SharedThis(this), Cast<UOdysseyPaletteEntryFolder>(iEntry));

    if (iEntry->IsA<UOdysseyPaletteEntryColor>())
        return SNew(SOdysseyPaletteColorRow, SharedThis(this), Cast<UOdysseyPaletteEntryColor>(iEntry));

    return SNew(STableRow<UOdysseyPaletteEntry*>, iOwnerTable);
}

void
SOdysseyPaletteTreeView::OnSelectionChanged(UOdysseyPaletteEntry* iEntry, ESelectInfo::Type iSelectInfo)
{
    if (!mPalette)
        return;

    mSelectedEntry = iEntry;

    if (!iEntry || !iEntry->IsA<UOdysseyPaletteEntryColor>())
    {
        mOnCurrentColorEntrySelected.ExecuteIfBound(nullptr);
        return;
    }

    mOnCurrentColorEntrySelected.ExecuteIfBound(Cast<UOdysseyPaletteEntryColor>(iEntry));
}

TSharedRef<SWidget>
SOdysseyPaletteTreeView::OnGenerateSetWidget(TSharedPtr<int> iSet)
{
    return SNew(STextBlock)
        .Text_Lambda(
            [this, iSet]()
            {
                if (!mPalette)
                    return FText::GetEmpty();

                if (!iSet || *iSet < 0 || *iSet >= mPalette->Sets.Num())
                    return FText::GetEmpty();

                return FText::FromName(mPalette->Sets[*iSet]);
            }
        );
}

void
SOdysseyPaletteTreeView::OnSetSelectionChanged(TSharedPtr<int> iSet, ESelectInfo::Type iSelectInfo)
{
    mOnCurrentSetSelected.ExecuteIfBound(iSet ? *iSet : INDEX_NONE);
}

void
SOdysseyPaletteTreeView::OnPaletteChanged()
{
    mPalette = mPaletteAttribute.Get();
    RefreshItemsSource();
    RefreshSetsSource();
}

void
SOdysseyPaletteTreeView::OnPaletteSetsChanged( UOdysseyPalette* iPalette)
{
    if (iPalette != mPalette)
        return;

    RefreshSetsSource();
}

void
SOdysseyPaletteTreeView::OnCurrentColorEntryChanged()
{
    bool canClearSelection = mCurrentColorEntry == mSelectedEntry;
    mCurrentColorEntry = mCurrentColorEntryAttribute.Get();

    if (!mCurrentColorEntry)
    {
        if (canClearSelection)
            ClearSelection();
        return;
    }

    if (!IsItemSelected(mCurrentColorEntry) && canClearSelection)
        ClearSelection();

    SetItemSelection(mCurrentColorEntry, true);
}

void
SOdysseyPaletteTreeView::OnCurrentSetChanged()
{
    mCurrentSet = mCurrentSetAttribute.Get();
    mSetComboBox->SetSelectedItem(GetCurrentSet());
}

void
SOdysseyPaletteTreeView::RefreshItemsSource()
{
    mItemsSource->Reset();

    if (!mPalette)
        return;

    TArray<UOdysseyPaletteEntry*> colorAndFolderEntries;
    for ( UOdysseyPaletteEntry* entry : mPalette->GetRootEntries() )
    {
        if (!entry || (!entry->IsA<UOdysseyPaletteEntryColor>() && !entry->IsA<UOdysseyPaletteEntryFolder>()))
            continue;

        colorAndFolderEntries.Add(entry);
    }

    mItemsSource->Append(colorAndFolderEntries);
}

void
SOdysseyPaletteTreeView::RefreshSetsSource()
{
    mSetsSource->Reset();

    if (!mPalette)
        return;

    TArray<TSharedPtr<int>> sets;
    for ( int i = 0; i < mPalette->Sets.Num() ; i++ )
    {
        sets.Add(MakeShared<int>(i));
    }

    mSetsSource->Append(sets);
}

UOdysseyPalette*
SOdysseyPaletteTreeView::GetPalette() const
{
    return mPalette;
}

UOdysseyPaletteEntryColor*
SOdysseyPaletteTreeView::GetCurrentColorEntry() const
{
    return mCurrentColorEntry;
}

TSharedPtr<int>
SOdysseyPaletteTreeView::GetCurrentSet() const
{
    if (mCurrentSet < 0 || mCurrentSet >= mSetsSource->Num())
        return nullptr;

    return (*mSetsSource)[mCurrentSet];
}

FReply
SOdysseyPaletteTreeView::AddColorEntry()
{
    if (!mPalette)
        return FReply::Unhandled();

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("palette-tree-view.transaction.add-color-entry", "Add Color Entry"));
#endif

    TArray<UOdysseyPaletteEntry*> selectedItems = GetSelectedItems();
    UOdysseyPaletteEntry* selectedItem = nullptr;
    if (!selectedItems.IsEmpty())
        selectedItem = selectedItems.Last();

    if (selectedItem)
    {
        if (selectedItem->CanHaveChildren)
        {
            selectedItem = mPalette->AddEntry(UOdysseyPaletteEntryColor::StaticClass(), selectedItem);
        }
        else
        {
            UOdysseyPaletteEntry* parent = selectedItem->GetParent();
            int index = selectedItem->GetIndexInParent();
            selectedItem = mPalette->AddEntry(UOdysseyPaletteEntryColor::StaticClass(), parent, index);
        }
    }
    else
    {
        selectedItem = mPalette->AddEntry(UOdysseyPaletteEntryColor::StaticClass());
    }

    SetItemSelection(selectedItem, true);

    return FReply::Handled();
}

FReply
SOdysseyPaletteTreeView::AddFolderEntry()
{
    if (!mPalette)
        return FReply::Unhandled();

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("add-folder-entry-button.transaction.add-folder-entry", "Add Folder Entry"));
#endif

    TArray<UOdysseyPaletteEntry*> selectedItems = GetSelectedItems();
    UOdysseyPaletteEntry* selectedItem = nullptr;
    if (!selectedItems.IsEmpty())
        selectedItem = selectedItems.Last();

    if (selectedItem)
    {
        if (selectedItem->CanHaveChildren)
        {
            selectedItem = mPalette->AddEntry(UOdysseyPaletteEntryFolder::StaticClass(), selectedItem);
        }
        else
        {
            UOdysseyPaletteEntry* parent = selectedItem->GetParent();
            int index = selectedItem->GetIndexInParent();
            selectedItem = mPalette->AddEntry(UOdysseyPaletteEntryFolder::StaticClass(), parent, index);
        }
    }
    else
    {
        selectedItem = mPalette->AddEntry(UOdysseyPaletteEntryFolder::StaticClass());
    }

    SetItemSelection(selectedItem, true);

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
