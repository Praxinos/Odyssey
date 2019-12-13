// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once
#include "CoreMinimal.h"
#include "Widgets/Views/STreeView.h"
#include "LayersGUI/OdysseyBaseLayerNode.h"

class SOdysseyLayerStackViewRow;


typedef TSharedRef<OdysseyBaseLayerNode> OdysseyBaseLayerNodeRef;

struct FHighlightRegion
{
    FHighlightRegion(float InTop, float InBottom) : Top(InTop), Bottom(InBottom) {}
    float Top, Bottom;
};




/** Structure used to define a column in the tree view */
struct FLayerStackTreeViewColumn
{
    typedef TFunction<TSharedRef<SWidget>(const OdysseyBaseLayerNodeRef&, const TSharedRef<SOdysseyLayerStackViewRow>&)> FOnGenerate;

    FLayerStackTreeViewColumn(const FOnGenerate& InOnGenerate, const TAttribute<float>& InWidth) : Generator(InOnGenerate), Width(InWidth) {}
    FLayerStackTreeViewColumn(FOnGenerate&& InOnGenerate, const TAttribute<float>& InWidth) : Generator(MoveTemp(InOnGenerate)), Width(InWidth) {}

    /** Function used to generate a cell for this column */
    FOnGenerate Generator;
    /** Attribute specifying the width of this column */
    TAttribute<float> Width;
};




/** A delegate that is executed when adding menu content. */
DECLARE_DELEGATE_OneParam(FOnGetContextMenuContent, FMenuBuilder& /*MenuBuilder*/);

/**
 * Implements the GUI for the layer Stack
 */
class SOdysseyLayerStackTreeView : public STreeView<OdysseyBaseLayerNodeRef>
{

public:
    //CONSTRUCTION/DESTRUCTION

    SLATE_BEGIN_ARGS(SOdysseyLayerStackTreeView){}
        /** Externally supplied scroll bar */
        SLATE_ARGUMENT( TSharedPtr<SScrollBar>, ExternalScrollbar )
        /** Called to populate the context menu. */
        SLATE_EVENT( FOnGetContextMenuContent, OnGetContextMenuContent )
    SLATE_END_ARGS()

    /** Construct this widget */
    void Construct(const FArguments& InArgs, const TSharedRef<FOdysseyLayerStackTree>& InNodeTree);

public:
    //PUBLIC API
    /** Access the underlying tree data */
    TSharedPtr<FOdysseyLayerStackTree> GetNodeTree() { return LayerStackNodeTree; }

    /** Refresh this tree as a result of the underlying tree data changing */
    void Refresh( int OverrideNewSelectedNodeIndex = -1 );

    TSharedPtr<OdysseyBaseLayerNode> GetSelectedNode();

    void SetSelectedNode( TSharedPtr<OdysseyBaseLayerNode> iNode );

    SVerticalBox::FSlot*& GetPropertyView();

    /** Handles a change of selected layer in the layer stack  */
    void OnSelectionChanged( TSharedPtr<OdysseyBaseLayerNode> InSelectedNode, ESelectInfo::Type InSeletionInfo = ESelectInfo::Type::Direct );

protected: //CALLBACKS

    /** Handles the context menu opening when right clicking on the tree view. */
    TSharedPtr<SWidget> OnContextMenuOpening();

    /** Gather the children from the specified node */
    void OnGetChildren(OdysseyBaseLayerNodeRef InParent, TArray<OdysseyBaseLayerNodeRef>& OutChildren) const;

    /** Generate a row for a particular node */
    TSharedRef<ITableRow> OnGenerateRow(OdysseyBaseLayerNodeRef InDisplayNode, const TSharedRef<STableViewBase>& OwnerTable);

    /** Generate a widget for the specified Node and Column */
    TSharedRef<SWidget> GenerateWidgetForColumn(const OdysseyBaseLayerNodeRef& Node, const FName& ColumnId, const TSharedRef<SOdysseyLayerStackViewRow>& Row) const;

protected:
    //PROTECTED API

    /** Populate the map of column definitions, and add relevant columns to the header row */
    void SetupColumns(const FArguments& InArgs);

private:

    /** The tree view's header row (hidden) */
    TSharedPtr<SHeaderRow> HeaderRow;

    /** Pointer to the node tree data that is used to populate this tree */
    TSharedPtr<FOdysseyLayerStackTree> LayerStackNodeTree;

    /** Cached copy of the root nodes from the tree data */
    TArray<OdysseyBaseLayerNodeRef> RootNodes;

    /** The current selected node */
    TSharedPtr<OdysseyBaseLayerNode> SelectedNode;

    /** Column definitions for each of the columns in the tree view */
    TMap<FName, FLayerStackTreeViewColumn> Columns;

    /** A global highlight for the currently hovered tree node hierarchy */
    TOptional<FHighlightRegion> HighlightRegion;

    /** When true, the LayerStack selection is being updated from a change in the tree selection. */
    bool bUpdatingLayerStackSelection;

    /** When true, the tree selection is being updated from a change in the LayerStack selection. */
    bool bUpdatingTreeSelection;

    FOnGetContextMenuContent OnGetContextMenuContent;

    //TSharedRef<FUICommandList> Commands;

    /** The property view of the currentSelected Node */
    SVerticalBox::FSlot*  PropertyView;

};




/** Widget that represents a row of the layer stack */
class SOdysseyLayerStackViewRow : public SMultiColumnTableRow<OdysseyBaseLayerNodeRef>
{

public: //SLATE DELEGATES

    DECLARE_DELEGATE_RetVal_ThreeParams(TSharedRef<SWidget>, FOnGenerateWidgetForColumn, const OdysseyBaseLayerNodeRef&, const FName&, const TSharedRef<SOdysseyLayerStackViewRow>&);

    SLATE_BEGIN_ARGS(SOdysseyLayerStackViewRow){}

    /** Delegate to invoke to create a new column for this row */
    SLATE_EVENT(FOnGenerateWidgetForColumn, OnGenerateWidgetForColumn)

    SLATE_END_ARGS()


public: //CONSTRUCTION / DESTRUCTION

    ~SOdysseyLayerStackViewRow();

    /** Construct function for this widget */
    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, const OdysseyBaseLayerNodeRef& InNode);


public: //PUBLIC API

    /** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the tree row. */
    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnId) override;


private: //CALLBACKS

    /** Called whenever a drag is detected by the tree view. */
    FReply OnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InPointerEvent);

    /** Called to determine whether a current drag operation is valid for this row. */
    TOptional<EItemDropZone> OnCanAcceptDrop( const FDragDropEvent& DragDropEvent, EItemDropZone ItemDropZone, OdysseyBaseLayerNodeRef DisplayNode);

    /** Called to complete a drag and drop onto this drop. */
    FReply OnAcceptDrop( const FDragDropEvent& DragDropEvent, EItemDropZone ItemDropZone, OdysseyBaseLayerNodeRef DisplayNode );

private:
    /** The item associated with this row of data */
    mutable TWeakPtr<OdysseyBaseLayerNode> Node;

    /** Delegate to call to create a new widget for a particular column. */
    FOnGenerateWidgetForColumn OnGenerateWidgetForColumn;

    /** Column definitions for each of the columns in the tree view */
    TMap<FName, FLayerStackTreeViewColumn> Columns;

    /** A ptr to our tree view, so we can control our selection */
    SOdysseyLayerStackTreeView* TreeView;

};
