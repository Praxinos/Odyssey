// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPaletteEntry.h"
#include "Widgets/Views/STreeView.h"
#include "DragDropOperations/OdysseyPaletteDragDropOperation.h"
#include "SOdysseyPaletteSetView.h"

class UOdysseyPalette;

/**
 * Implements the Palette widget
 */
class ODYSSEYPALETTE_API SOdysseyPaletteTreeView
    : public STreeView<UOdysseyPaletteEntry*>
{

public:
    SLATE_BEGIN_ARGS(SOdysseyPaletteTreeView)
        : _HeaderFillWidth( 1.0f )
        {}
        SLATE_ARGUMENT( UOdysseyPalette*, Palette )
        SLATE_ARGUMENT( TArray<SHeaderRow::FColumn::FArguments>, AdditionalColumns )
        /** Set the HeaderColumn Size Mode to Fill. It's a fraction between 0 and 1 */
        SLATE_ATTRIBUTE( float, HeaderFillWidth )
        /** Set the HeaderColumn Size Mode to Fixed. */
        SLATE_ARGUMENT( TOptional< float >, HeaderFixedWidth )
        /** Set the HeaderColumn Size Mode to Manual. */
        SLATE_ATTRIBUTE( float, HeaderManualWidth )
        /** Set the HeaderColumn Size Mode to Fill Sized. */
        SLATE_ARGUMENT(TOptional< float >, HeaderFillSized)
        SLATE_EVENT( FOnGenerateRow, OnGenerateRow )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPaletteTreeView();
    SOdysseyPaletteTreeView();
    
    void Construct(const FArguments& InArgs);
    
public:
    TSharedPtr<FOdysseyPaletteDragDropOperation> CreateDragDropOperation() const;

protected:
    //SWidget overrides
    int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 EntryId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;
    virtual FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;

    virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
    virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;

protected:
    //STreeView overrides
    virtual void Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo) override;

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
    /**
     * @brief Set the palette selected entry from the treeview Selector Item
     * 
     */
    void SetCurrentEntryFromSelectorItem();

    /**
     * @brief Refreshes all rows expansion states from the state stored in the entry
     * 
     */
    void RefreshAllExpansionStates();

    /**
     * @brief Creation of the widget to add and delete sets of palette
     *
     */
    TSharedRef<SWidget> CreateSetWidget();

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
     * @brief Called when the treeview changed one of its row expansion state
     * Allows synchronization between treeview expansion state and entry expansion state
     * 
     * @param iEntry 
     * @param iIsExpanded 
     */
    void OnExpansionChanged(UOdysseyPaletteEntry* iEntry, bool iIsExpanded);

    /**
     * @brief Called when the entry changed the expansion state it stores
     * Allows synchronization between treeview expansion state and entry expansion state
     * 
     * @param iEntry 
     */
    void OnEntryIsExpandedChanged(UOdysseyPaletteEntry* iEntry);

    /**
     * @brief Called when the current entry changed to another entry
     * 
     * @param iPalette 
     */
    void OnCurrentEntryChanged(UOdysseyPalette* iPalette);

    /**
     * @brief Called when the palette hierarchy has changed
     * 
     * @param iPalette
     */
    void OnPaletteHierarchyChanged( UOdysseyPalette* iPalette);

    /**
     * @brief Called when entries have been removed from a folder entry
     * 
     * @param iParent 
     * @param iChildren 
     */
    void OnItemScrolledIntoView(UOdysseyPaletteEntry* iEntry, const TSharedPtr<ITableRow>& iRow);

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

protected:
    UOdysseyPalette* mPalette;

    TSharedRef<FUICommandList> mCommandList;

    bool mNeedsRefresh = false;
    bool mIsRenamePending = false;
    bool mDisplayDropZone = false;

    TSharedPtr<SOdysseyPaletteSetView> mPaletteSetView;
};
