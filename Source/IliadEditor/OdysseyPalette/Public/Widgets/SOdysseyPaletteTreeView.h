// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPaletteEntry.h"
#include "Widgets/Views/STreeView.h"
#include "DragDropOperations/OdysseyPaletteDragDropOperation.h"
#include "Widgets/Input/SComboBox.h"

class UOdysseyPalette;
class UOdysseyPaletteEntryColor;

/**
 * Implements the Palette widget
 */
class ODYSSEYPALETTE_API SOdysseyPaletteTreeView
    : public STreeView<UOdysseyPaletteEntry*>
{
    SLATE_DECLARE_WIDGET(SOdysseyPaletteTreeView, STreeView<UOdysseyPaletteEntry*>)

public:
    DECLARE_DELEGATE_OneParam(FOnCurrentColorEntrySelected, UOdysseyPaletteEntryColor*)
    DECLARE_DELEGATE_OneParam(FOnCurrentSetSelected, int)

public:
    SLATE_BEGIN_ARGS(SOdysseyPaletteTreeView)
        {}
        SLATE_ATTRIBUTE(UOdysseyPalette*, Palette)
        SLATE_ATTRIBUTE(UOdysseyPaletteEntryColor*, CurrentColorEntry)
        SLATE_ATTRIBUTE(int, CurrentSet)
        SLATE_EVENT(FOnCurrentColorEntrySelected, OnCurrentColorEntrySelected)
        SLATE_EVENT(FOnCurrentSetSelected, OnCurrentSetSelected)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPaletteTreeView();
    SOdysseyPaletteTreeView();

    void Construct(const FArguments& InArgs);

public:
    TSharedPtr<FOdysseyPaletteDragDropOperation> CreateDragDropOperation() const;
    UOdysseyPaletteEntryColor* GetCurrentColorEntry() const;
    TSharedPtr<int> GetCurrentSet() const;
    UOdysseyPalette* GetPalette() const;

protected:
    //SWidget overrides
    int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 EntryId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;
    virtual FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;

    virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
    virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;

protected:
    //Context menu
    /**
     * @brief Creates and registers palette context menu
     *
     */
    virtual void CreateContextMenu();

    void OnSetSelected(FName iSet);

    /**
     * @brief Extends the context menu
     * Allows us to insert entries wherever we want in the context menu
     * CreateContextMenu() does not allow us to do that
     */
    virtual TArray<TSharedPtr<FExtender>> ExtendContextMenu();

private:
    TSharedRef<SWidget> CreateHeaderColumnHeaderContent();
    TSharedRef<SWidget> CreateColorColumnHeaderContent();

protected:
    //CommandList Actions
    /**
     * @brief Maps palette actions to the commandList
     *
     */
    virtual void MapActionsToCommandList();

    /**
     * @brief Selects all visible entries
     *
     */
    void SelectAllEntries();

    /**
     * @brief Delete Selected Entries
     *
     */
    void DeleteSelectedEntries();

    /**
     * @brief Defines whether the "DeleteSelectedEntries" can be called
     *
     * @return true
     * @return false
     */
    bool CanDeleteSelectedEntries();

    /**
     * @brief Duplicates Selected Entries and Select them afterward
     * Also changes the current entry to be the duplicated version of the former current entry
     *
     */
    void DuplicateSelectedEntries();

    /**
     * @brief Start renaming the current entry
     * Also, will scroll the entry into view first.
     *
     */
    void RenameCurrentEntry();

protected:
    //Callbacks
    /**
     * @brief Called when the treeview asks for a contextmenu to be opened (rightclick)
     *
     * @return TSharedPtr<SWidget>
     */
    TSharedPtr<SWidget> OnContextMenuOpening();

    /**
     * @brief Called when the treeview asks for the children of a specific item
     *
     * @param iParent
     * @param oChildren
     */
    void OnGetChildren(UOdysseyPaletteEntry* iParent, TArray<UOdysseyPaletteEntry*>& oChildren) const;

    /**
     * @brief Called when the palette hierarchy has changed
     *
     * @param iPalette
     */
    void OnPaletteHierarchyChanged( UOdysseyPalette* iPalette);
    void OnPaletteSetsChanged( UOdysseyPalette* iPalette);

    /**
     * @brief Called when entries have been removed from a folder entry
     *
     * @param iParent
     * @param iChildren
     */
    void OnItemScrolledIntoView(UOdysseyPaletteEntry* iEntry, const TSharedPtr<ITableRow>& iRow);

    TSharedRef<ITableRow> OnGenerateRow(UOdysseyPaletteEntry* iEntry, const TSharedRef<STableViewBase>& iOwnerTable);
    void OnSelectionChanged(UOdysseyPaletteEntry* iEntry, ESelectInfo::Type iSelectInfo);
    void OnSetSelectionChanged(TSharedPtr<int> iSet, ESelectInfo::Type iSelectInfo);

public:
    /**
     * @brief Hides the dropzone of the Palette
     * Used by rows to avoid having 2 dropzones displayed at the same time
     */
    void ResetDropZone();

    /**
     * @brief Adds a new set to the palette represented by this view
     */
    FReply AddSetToPalette();

    /**
     * @brief Saves palette
     */
    FReply SavePalette();

private:
    void OnPaletteChanged();
    void OnCurrentColorEntryChanged();
    void OnCurrentSetChanged();
    void RefreshItemsSource();
    void RefreshSetsSource();

    FReply AddColorEntry();
    FReply AddFolderEntry();
    TSharedRef<SWidget> OnGenerateSetWidget(TSharedPtr<int> iSet);

protected:
    TSlateAttribute<UOdysseyPalette*> mPaletteAttribute;
    UOdysseyPalette* mPalette;
    TSlateAttribute<UOdysseyPaletteEntryColor*> mCurrentColorEntryAttribute;
    UOdysseyPaletteEntryColor* mCurrentColorEntry;
    TSlateAttribute<int> mCurrentSetAttribute;
    int mCurrentSet;
    UOdysseyPaletteEntry* mSelectedEntry;
    FOnCurrentColorEntrySelected mOnCurrentColorEntrySelected;
    FOnCurrentSetSelected mOnCurrentSetSelected;
    TSharedPtr<UE::Slate::Containers::TObservableArray<UOdysseyPaletteEntry*>> mItemsSource;
    TSharedPtr<UE::Slate::Containers::TObservableArray<TSharedPtr<int>>> mSetsSource;

    TSharedRef<FUICommandList> mCommandList;

    bool mIsRenamePending = false;
    bool mDisplayDropZone = false;

    TSharedPtr<SComboBox<TSharedPtr<int>>> mSetComboBox;
};
