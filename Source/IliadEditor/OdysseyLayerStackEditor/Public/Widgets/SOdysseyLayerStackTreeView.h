// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyLayerStackDragDropOperation.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "OdysseyLayer.h"
#include "Widgets/Views/STreeView.h"

class UOdysseyLayerStack;
class FOdysseyLayerStackShortcuts;

/**
 * Implements the Layer stack widget
 */
class ODYSSEYLAYERSTACKEDITOR_API SOdysseyLayerStackTreeView
    : public STreeView<UOdysseyLayer*>
{

//SLATE_DECLARE_WIDGET(SOdysseyLayerStackTreeView, STreeView<UOdysseyLayer*>)

public:
    SLATE_BEGIN_ARGS(SOdysseyLayerStackTreeView)
        : _ExternalScrollbar(nullptr)
        {}
        SLATE_NAMED_SLOT(FArguments, HeaderContent)
        SLATE_ARGUMENT( UOdysseyLayerStack*, LayerStack )
        SLATE_ARGUMENT( TOptional<TArray<SHeaderRow::FColumn::FArguments>>, Columns )
        SLATE_EVENT( FOnGenerateRow, OnGenerateRow )
        SLATE_ARGUMENT( TSharedPtr<SScrollBar>, ExternalScrollbar )
        SLATE_EVENT( FOnTableViewScrolled, OnTreeViewScrolled )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyLayerStackTreeView();
    SOdysseyLayerStackTreeView();

    void Construct(const FArguments& InArgs);

public:
    using STreeView<UOdysseyLayer*>::ScrollTo;

    UOdysseyLayerStack* GetLayerStack() const;
    void SetIsRenamePending(bool iValue);
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
     * @brief Refreshes all rows expansion states from the state stored in the Layer
     *
     */
    void RefreshAllExpansionStates();

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
     * @brief The layer's DisplayChildren property changes
     *
     * @param iLayer
     */
    void OnLayerDisplayChildrenChanged(UOdysseyLayer* iLayer);

    /**
     * @brief The layer's DisplayOptions property changes
     *
     * @param iLayer
     */
    void OnLayerDisplayOptionsChanged(UOdysseyLayer* iLayer);

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


    int32 GetMaxIndentLevel() const;

protected:
    UOdysseyLayerStack* mLayerStack;

    bool mNeedsRefresh = false;
    bool mIsRenamePending = false;
    bool mDisplayDropZone = false;

    TSharedPtr<FOdysseyLayerStackShortcuts> mLayerStackShortcuts;
};
