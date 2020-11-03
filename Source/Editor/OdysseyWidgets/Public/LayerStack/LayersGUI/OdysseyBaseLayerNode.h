// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "LayerStack/FOdysseyLayerStackTree.h"
#include "Widgets/SWidget.h"
#include "Styling/SlateColor.h"

class FMenuBuilder;
class SOdysseyLayerStackViewRow;
class FOdysseyLayerStackNodeDragDropOp;
struct FSlateBrush;
enum class EItemDropZone;

/**
 * Structure used to define padding for a particular node.
 */
struct FNodePadding
{
    FNodePadding(float iLeft, float iTop, float iBottom)
    : mLeft( iLeft )
    , mTop( iTop )
    , mBottom( iBottom )
    {}

    /** @return The sum total of the separate padding values */
    float CombinedTopLeft() const
    {
        return mTop + mBottom;
    }

    /** Padding to be applied to the left of the node */
    float mLeft;
    
    /** Padding to be applied to the top of the node */
    float mTop;

    /** Padding to be applied to the bottom of the node */
    float mBottom;
};




/** Base node GUI for a layer node in the layerStack */
class IOdysseyBaseLayerNode : public TSharedFromThis<IOdysseyBaseLayerNode>
{
public:

    /**
     * Create and initialize a new instance.
     *
     * @param iNodeName    The name identifier of the node
     * @param iParentNode    The parent of this node or nullptr if this is a root node
     * @param iParentTree    The tree this node is in
     */
    IOdysseyBaseLayerNode( FName iNodeName, FOdysseyLayerStackTree& iParentTree, TSharedPtr<IOdysseyLayer> iLayerDataPtr );

    /** Virtual destructor. */
    virtual ~IOdysseyBaseLayerNode(){}


public: //EVENTS

    DECLARE_EVENT(IOdysseyBaseLayerNode, FRequestRenameEvent);
    FRequestRenameEvent& OnRenameRequested() { return mRenameRequestedEvent; }


public: // PUBLIC API

    /** @return Whether or not this node can be selected */
    virtual bool IsSelectable() const;

    /** @return The desired height of the node when displayed */
    virtual float GetNodeHeight() const = 0;

    /** @return The desired padding of the node when displayed */
    virtual FNodePadding GetNodePadding() const = 0;

    /** @return true if this node can be renamed, false otherwise. */
    virtual bool CanRenameNode() const;

    /** @return The localized display name of this node */
    virtual FText GetDisplayName() const;

    /** @return the color used to draw the display name. */
    virtual FLinearColor GetDisplayNameColor() const;

    /** @return the text to display for the tool tip for the display name. */
    virtual FText GetDisplayNameToolTipText() const;

    /** @param NewDisplayName the display name to set. */
    virtual void SetDisplayName(const FText& NewDisplayName);

    /** @return Generated Property View widget */
    virtual TSharedRef<SWidget> GenerateContainerWidgetForPropertyView() = 0;

    /** @return Generated outliner widget */
    virtual TSharedRef<SWidget> GenerateContainerWidgetForOutliner(const TSharedRef<SOdysseyLayerStackViewRow>& iRow);

    /** @return Content to display on the outliner node */
    virtual TSharedRef<SWidget> GetCustomIconContent() = 0;
    
    /** @return Content to display on the outliner node */
    virtual TSharedRef<SWidget> GetCustomOutlinerContent() = 0;

    /** @return This node's representative icon */
    virtual const FSlateBrush* GetIconBrush() const;

    /** @return This node's representative color */
    virtual FSlateColor GetIconColor() const;

    /** Summon context menu */
    TSharedPtr<SWidget> OnSummonContextMenu();

    /** What sort of context menu this node summons */
    virtual void BuildContextMenu(FMenuBuilder& iMenuBuilder) = 0;

    /** @return The name of the node (for identification purposes) */
    FName GetNodeName() const;

    /** @return Whether this node is explicitly hidden from the view or not */
    virtual bool IsHidden() const = 0;

    /** @return true if hovered, false otherwise. */
    bool IsHovered() const;

    /** Initialize this node virtual offsets */
    void Initialize(float iVirtualTop, float iVirtualBottom);

    /** @return this node's virtual offset from the top of the tree*/
    float GetVirtualTop() const;

    /** @return this node's virtual offset from the bottom of the tree* */
    float GetVirtualBottom() const;

    /** Determines if there is a valid drop zone based on the current drag drop operation and the zone the items were dragged onto. */
    virtual TOptional<EItemDropZone> CanDrop( FOdysseyLayerStackNodeDragDropOp& DragDropOp, EItemDropZone ItemDropZone ) const = 0;

    /** Handles a drop of items onto this display node. */
    virtual void Drop( const TArray<TSharedRef<IOdysseyBaseLayerNode>>& iDraggedNodes, EItemDropZone iDropZone ) = 0;

    /** Action to do when this node is moved to another node */
    void MoveNodeTo( EItemDropZone iItemDropZone, TSharedRef<IOdysseyBaseLayerNode> iCurrentNode );
    
    /** Gets the layerStack that owns this node */
    FOdysseyLayerStackModel& GetLayerStack() const;

    /** Gets the parent tree that this node is in */
    FOdysseyLayerStackTree& GetParentTree() const;

    /** Gets the pointer to the data contained in this node */
    TSharedPtr<IOdysseyLayer> GetLayerDataPtr() const;


private: // HANDLES

    /** Callback for executing a "Rename Node" context menu action. */
    void HandleContextMenuRenameNodeExecute();

    /** Callback for determining whether a "Rename Node" context menu action can execute. */
    bool HandleContextMenuRenameNodeCanExecute() const;

protected:

    /** The virtual offset of this item from the top of the tree, irrespective of expansion states. */
    float mVirtualTop;

    /** The virtual offset + virtual height of this item, irrespective of expansion states. */
    float mVirtualBottom;


protected:
    /** List of children belonging to this node */
    TArray<TSharedRef<IOdysseyBaseLayerNode>> mChildNodes;

    /** Parent tree that this node is in */
    FOdysseyLayerStackTree& mParentTree;

    /** The name identifier of this node */
    FName mNodeName;

    /** Event that is triggered when rename is requested */
    FRequestRenameEvent mRenameRequestedEvent;

    /** The interface ptr to the data represented by this node, only used as verification purposes */
    TSharedPtr<IOdysseyLayer> mLayerDataPtr;
};
