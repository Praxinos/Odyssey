// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyLayer.h"
#include "Widgets/Views/STreeView.h"
#include "DragDropOperations/OdysseyLayerStackDragDropOperation.h"

class UOdysseyLayerStack;

/**
 * Implements the Layer stack widget
 */
class ODYSSEYLAYERSTACKEDITOR_API SOdysseyLayerStackTreeView
    : public STreeView<UOdysseyLayer*>
{

//SLATE_DECLARE_WIDGET(SOdysseyLayerStackTreeView, STreeView<UOdysseyLayer*>)

public:
    SLATE_BEGIN_ARGS(SOdysseyLayerStackTreeView)
        : _HeaderFillWidth( 1.0f )
        {}
        SLATE_ARGUMENT( UOdysseyLayerStack*, LayerStack )
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
    ~SOdysseyLayerStackTreeView();
    SOdysseyLayerStackTreeView();
    
    void Construct(const FArguments& InArgs);
    
public:
    TSharedPtr<FOdysseyLayerStackDragDropOperation> CreateDragDropOperation() const;

protected:
    //SWidget overrides
    int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;
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
     * @brief Creates and registers layerstack the context menu
     * 
     */
    virtual void CreateContextMenu();

    /**
     * @brief Extends the context menu
     * Allows us to insert entries wherever we want in the context menu
     * CreateContextMenu() does not allow us to do that
     */
    virtual TArray<TSharedPtr<FExtender>> ExtendContextMenu();

private:
    /**
     * @brief Set the layerstack's Current Layer From the treeview Selector Item
     * 
     */
    void SetCurrentLayerFromSelectorItem();

    /**
     * @brief Refresh the list of root layers
     * 
     */
    void RefreshRootLayersArray();

    /**
     * @brief Refreshes all rows expansion states from the state stored in the Layer
     * 
     */
	void RefreshAllExpansionStates();

protected:
    //CommandList Actions
    /**
     * @brief Maps layerstacks actions to the commandList
     * 
     */
    virtual void MapActionsToCommandList();

    /**
     * @brief Selects all visible layers
     * 
     */
    void SelectAllLayers();

    /**
     * @brief Delete Selected Layers
     * 
     */
    void DeleteSelectedLayers();

    /**
     * @brief Defines wether the "DeleteSelectedLayers" can be called
     * 
     * @return true 
     * @return false 
     */
    bool CanDeleteSelectedLayers();

    /**
     * @brief Duplicates Selected Layers and Select them afterward
     * Also changes the current layer to be the duplicated version of the former current layer
     * 
     */
    void DuplicateSelectedLayers();

    /**
     * @brief Start renaming the current layer
     * Also, will scroll the layer into view first.
     * 
     */
    void RenameCurrentLayer();

    /**
     * @brief Merge Selected Layers
     * 
     */
    void MergeSelectedLayers();

    /**
     * @brief Defines wether the "MergeSelectedLayers" can be called
     * 
     * @return true 
     * @return false 
     */
    bool CanMergeSelectedLayers();

    /**
     * @brief Flatten Selected Layers
     * 
     */
    void FlattenSelectedLayers();

    /**
     * @brief Defines wether the "FlattenSelectedLayers" can be called
     * 
     * @return true 
     * @return false 
     */
    bool CanFlattenSelectedLayers();

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
    void OnGetChildren(UOdysseyLayer* iParent, TArray<UOdysseyLayer*>& oChildren) const;

    /**
     * @brief Called when the treeview changed one of its row expansion state
     * Allows synchronization between treeview expansion state and layer expansion state
     * 
     * @param iLayer 
     * @param iIsExpanded 
     */
    void OnExpansionChanged(UOdysseyLayer* iLayer, bool iIsExpanded);

    /**
     * @brief Called when the Layer changed the expansion state it stores
     * Allows synchronization between treeview expansion state and layer expansion state
     * 
     * @param iLayer 
     */
    void OnLayerIsExpandedChanged(UOdysseyLayer* iLayer);

    /**
     * @brief Called when the layerstack's currentLayer changed to another layer
     * 
     * @param iLayerStack 
     */
    void OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack);

    /**
     * @brief Called when the layerstack hierarchy has changed
     * 
     * @param iParentLayers 
     */
    void OnLayerStackHierarchyChanged( UOdysseyLayerStack* iLayerStack);

    /**
     * @brief Called when layers has been removed from another layer
     * 
     * @param iParent 
     * @param iChildren 
     */
    void OnItemScrolledIntoView(UOdysseyLayer* iLayer, const TSharedPtr<ITableRow>& iRow);

public:
    /**
     * @brief Hides the dropzone of the layerstack
     * Used by rows to avoid having 2 dropzones displayed at the same time
     */
    void ResetDropZone();

protected:
    //TSlateAttribute<UOdysseyLayerStack*> mLayerStack;
    UOdysseyLayerStack* mLayerStack;

    //const TArray<UOdysseyLayer*>* mRootLayers = nullptr;
    TSharedRef<FUICommandList> mCommandList;

    bool mNeedsRefresh = false;
    bool mIsRenamePending = false;
    bool mDisplayDropZone = false;
};
