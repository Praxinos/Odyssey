// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once
#include "CoreMinimal.h"
#include "Widgets/Views/STreeView.h"
#include "LayersGUI/OdysseyBaseLayerNode.h"

class SOdysseyLayerStackViewRow;


typedef TSharedRef<IOdysseyBaseLayerNode> IOdysseyBaseLayerNodeRef;

struct FHighlightRegion
{
    FHighlightRegion(float iTop, float iBottom)
    : mTop(iTop)
    , mBottom(iBottom)
    {}
    
    float mTop, mBottom;
};




/** Structure used to define a column in the tree view */
struct FLayerStackTreeViewColumn
{
    typedef TFunction<TSharedRef<SWidget>(const IOdysseyBaseLayerNodeRef&, const TSharedRef<SOdysseyLayerStackViewRow>&)> FOnGenerate;

    FLayerStackTreeViewColumn(const FOnGenerate& iOnGenerate, const TAttribute<float>& iWidth)
    : mGenerator(iOnGenerate)
    , mWidth(iWidth)
    {}
    
    FLayerStackTreeViewColumn(FOnGenerate&& iOnGenerate, const TAttribute<float>& iWidth)
    : mGenerator(MoveTemp(iOnGenerate))
    , mWidth(iWidth)
    {}

    /** Function used to generate a cell for this column */
    FOnGenerate mGenerator;
    /** Attribute specifying the width of this column */
    TAttribute<float> mWidth;
};




/** A delegate that is executed when adding menu content. */
DECLARE_DELEGATE_OneParam(FOnGetContextMenuContent, FMenuBuilder& /*MenuBuilder*/);

/**
 * Implements the GUI for the layer Stack
 */
class SOdysseyLayerStackTreeView : public STreeView<IOdysseyBaseLayerNodeRef>
{

public:
    //CONSTRUCTION/DESTRUCTION

    SLATE_BEGIN_ARGS(SOdysseyLayerStackTreeView){}
        /** Externally supplied scroll bar */
        SLATE_ARGUMENT( TSharedPtr<SScrollBar>, ExternalScrollbar )
		/** Node Tree to display */
		SLATE_ATTRIBUTE(TSharedPtr<FOdysseyLayerStackTree>, NodeTree)
        /** Called to populate the context menu. */
        SLATE_EVENT( FOnGetContextMenuContent, OnGetContextMenuContent )
    SLATE_END_ARGS()

    /** Construct this widget */
    void Construct(const FArguments& InArgs);

public:
    //PUBLIC API
    /** Access the underlying tree data */
    TSharedPtr<FOdysseyLayerStackTree> GetNodeTree() { return mLayerStackNodeTree.Get(); }

    /** Refresh this tree as a result of the underlying tree data changing */
    void Refresh( int iOverrideNewSelectedNodeIndex = -1 );

    TSharedPtr<IOdysseyBaseLayerNode> GetSelectedNode();

    SVerticalBox::FSlot*& GetPropertyView();

    /** Handles a change of selected layer in the layer stack  */
    void OnSelectionChanged( TSharedPtr<IOdysseyBaseLayerNode> iSelectedNode, ESelectInfo::Type iSeletionInfo = ESelectInfo::Type::Direct );

protected: //CALLBACKS

    /** Handles the context menu opening when right clicking on the tree view. */
    TSharedPtr<SWidget> OnContextMenuOpening();

    /** Gather the children from the specified node */
    void OnGetChildren(IOdysseyBaseLayerNodeRef iParent, TArray<IOdysseyBaseLayerNodeRef>& oChildren) const;

    /** Generate a row for a particular node */
    TSharedRef<ITableRow> OnGenerateRow(IOdysseyBaseLayerNodeRef iDisplayNode, const TSharedRef<STableViewBase>& iOwnerTable);

    /** Generate a widget for the specified Node and Column */
    TSharedRef<SWidget> GenerateWidgetForColumn(const IOdysseyBaseLayerNodeRef& iNode, const FName& iColumnId, const TSharedRef<SOdysseyLayerStackViewRow>& iRow) const;

protected:
    //PROTECTED API

    /** Populate the map of column definitions, and add relevant columns to the header row */
    void SetupColumns(const FArguments& InArgs);

private:

    /** The tree view's header row (hidden) */
    TSharedPtr<SHeaderRow> mHeaderRow;

    /** Pointer to the node tree data that is used to populate this tree */
    TAttribute<TSharedPtr<FOdysseyLayerStackTree>> mLayerStackNodeTree;

    /** Cached copy of the root nodes from the tree data */
    TArray<IOdysseyBaseLayerNodeRef> mRootNodes;

    /** The current selected node */
    TSharedPtr<IOdysseyBaseLayerNode> mSelectedNode;

    /** Column definitions for each of the columns in the tree view */
    TMap<FName, FLayerStackTreeViewColumn> mColumns;

    /** A global highlight for the currently hovered tree node hierarchy */
    TOptional<FHighlightRegion> mHighlightRegion;

    /** When true, the LayerStack selection is being updated from a change in the tree selection. */
    bool mUpdatingLayerStackSelection;

    /** When true, the tree selection is being updated from a change in the LayerStack selection. */
    bool mUpdatingTreeSelection;

    FOnGetContextMenuContent mOnGetContextMenuContent;

    //TSharedRef<FUICommandList> Commands;

    /** The property view of the currentSelected Node */
    SVerticalBox::FSlot*  mPropertyView;

};




/** Widget that represents a row of the layer stack */
class SOdysseyLayerStackViewRow : public SMultiColumnTableRow<IOdysseyBaseLayerNodeRef>
{

public: //SLATE DELEGATES

    DECLARE_DELEGATE_RetVal_ThreeParams(TSharedRef<SWidget>, FOnGenerateWidgetForColumn, const IOdysseyBaseLayerNodeRef&, const FName&, const TSharedRef<SOdysseyLayerStackViewRow>&);

    SLATE_BEGIN_ARGS(SOdysseyLayerStackViewRow){}

    /** Delegate to invoke to create a new column for this row */
    SLATE_EVENT(FOnGenerateWidgetForColumn, OnGenerateWidgetForColumn)

    SLATE_END_ARGS()


public: //CONSTRUCTION / DESTRUCTION

    ~SOdysseyLayerStackViewRow();

    /** Construct function for this widget */
    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, const IOdysseyBaseLayerNodeRef& iNode);


public: //PUBLIC API

    /** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the tree row. */
    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& iColumnId) override;


private: //CALLBACKS

    /** Called whenever a drag is detected by the tree view. */
    FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iPointerEvent);

    /** Called to determine whether a current drag operation is valid for this row. */
    TOptional<EItemDropZone> OnCanAcceptDrop( const FDragDropEvent& iDragDropEvent, EItemDropZone iItemDropZone, IOdysseyBaseLayerNodeRef iDisplayNode);

    /** Called to complete a drag and drop onto this drop. */
    FReply OnAcceptDrop( const FDragDropEvent& iDragDropEvent, EItemDropZone iItemDropZone, IOdysseyBaseLayerNodeRef iDisplayNode );

private:
    /** The item associated with this row of data */
    mutable TWeakPtr<IOdysseyBaseLayerNode> mNode;

    /** Delegate to call to create a new widget for a particular column. */
    FOnGenerateWidgetForColumn mOnGenerateWidgetForColumn;

    /** Column definitions for each of the columns in the tree view */
    TMap<FName, FLayerStackTreeViewColumn> mColumns;

    /** A ptr to our tree view, so we can control our selection */
    SOdysseyLayerStackTreeView* mTreeView;

};
