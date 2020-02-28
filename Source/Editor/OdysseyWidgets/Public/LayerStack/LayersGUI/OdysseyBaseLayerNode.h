// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "../FOdysseyLayerStackTree.h"
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




/**
 * Base node GUI for a layer node in the layerStack
 */
class OdysseyBaseLayerNode : public TSharedFromThis<OdysseyBaseLayerNode>
{
public:

    /**
     * Create and initialize a new instance.
     *
     * @param InNodeName    The name identifier of the node
     * @param InParentNode    The parent of this node or nullptr if this is a root node
     * @param InParentTree    The tree this node is in
     */
    OdysseyBaseLayerNode( FName InNodeName, TSharedPtr<OdysseyBaseLayerNode> InParentNode, FOdysseyLayerStackTree& InParentTree, IOdysseyLayer* InLayerDataPtr );

    /** Virtual destructor. */
    virtual ~OdysseyBaseLayerNode(){}


public: //EVENTS

    DECLARE_EVENT(OdysseyBaseLayerNode, FRequestRenameEvent);
    FRequestRenameEvent& OnRenameRequested() { return mRenameRequestedEvent; }


public: // PUBLIC API

    /** @return Whether or not this node can be selected */
    virtual bool IsSelectable() const
    {
        return true;
    }

    /**
     * @return The desired height of the node when displayed
     */
    virtual float GetNodeHeight() const = 0;

    /**
     * @return The desired padding of the node when displayed
     */
    virtual FNodePadding GetNodePadding() const = 0;

    /**
     * Whether the node can be renamed.
     *
     * @return true if this node can be renamed, false otherwise.
     */
    virtual bool CanRenameNode() const;

    /**
     * @return The localized display name of this node
     */
    virtual FText GetDisplayName() const;

    /**
     * @return the color used to draw the display name.
     */
    virtual FLinearColor GetDisplayNameColor() const;

    /**
     * @return the text to display for the tool tip for the display name.
     */
    virtual FText GetDisplayNameToolTipText() const;

    /**
     * Set the node's display name.
     *
     * @param NewDisplayName the display name to set.
     */
    virtual void SetDisplayName(const FText& NewDisplayName);

    /**
     * @return Whether this node handles resize events
     */
    virtual bool IsResizable() const
    {
        return false;
    }

    /**
     * Resize this node
     */
    virtual void Resize(float NewSize)
    {

    }

    /**
     * Generates a widget for display in the property view section of the layer (where you can set the opacity, blend mode...)
     *
     * @return Generated Property View widget
     */
    virtual TSharedRef<SWidget> GenerateContainerWidgetForPropertyView() = 0;

    /**
     * Generates a widget for display in the LayerStack section
     *
     * @return Generated outliner widget
     */
    virtual TSharedRef<SWidget> GenerateContainerWidgetForOutliner(const TSharedRef<SOdysseyLayerStackViewRow>& InRow);


    /**
     * Customizes the icon widget of the node
     *
     * @return Content to display on the outliner node
     */
    virtual TSharedRef<SWidget> GetCustomIconContent() = 0;
    
    /**
     * Customizes an outliner widget that is to represent this node
     *
     * @return Content to display on the outliner node
     */
    virtual TSharedRef<SWidget> GetCustomOutlinerContent() = 0;


    /**
     * Gets an icon that represents this sequencer display node
     *
     * @return This node's representative icon
     */
    virtual const FSlateBrush* GetIconBrush() const;

    /**
     * Gets the color for the icon brush
     *
     * @return This node's representative color
     */
    virtual FSlateColor GetIconColor() const;

    /**
     * Get the tooltip text to display for this node's icon
     *
     * @return Text to display on the icon
     */
    virtual FText GetIconToolTipText() const;

    /**
     * @return the path to this node starting with the outermost parent
     */
    FString GetPathName() const;

    /** Summon context menu */
    TSharedPtr<SWidget> OnSummonContextMenu();

    /** What sort of context menu this node summons */
    virtual void BuildContextMenu(FMenuBuilder& MenuBuilder) = 0;

    /**
     * @return The name of the node (for identification purposes)
     */
    FName GetNodeName() const
    {
        return mNodeName;
    }

    /**
     * @return The number of child nodes belonging to this node
     */
    uint32 GetNumChildren() const
    {
        return mChildNodes.Num();
    }

    /**
     * @return A List of all Child nodes belonging to this node
     */
    const TArray<TSharedRef<OdysseyBaseLayerNode>>& GetChildNodes() const
    {
        return mChildNodes;
    }

    /**
     * @return The parent of this node
     */
    TSharedPtr<OdysseyBaseLayerNode> GetParent() const
    {
        return mParentNode.Pin();
    }

    /**
     * @return The outermost parent of this node
     */
    TSharedRef<OdysseyBaseLayerNode> GetOutermostParent()
    {
        TSharedPtr<OdysseyBaseLayerNode> Parent = mParentNode.Pin();
        return Parent.IsValid() ? Parent->GetOutermostParent() : AsShared();
    }

    /** Gets the layerStack that owns this node */
    FOdysseyLayerStackModel& GetLayerStack() const
    {
        return mParentTree.GetLayerStack();
    }

    /** Gets the parent tree that this node is in */
    FOdysseyLayerStackTree& GetParentTree() const
    {
        return mParentTree;
    }

    IOdysseyLayer* GetLayerDataPtr() const
    {
        return mLayerDataPtr;
    }

    /**
     * Set whether this node is expanded or not
     */
    void SetExpansionState(bool bInExpanded);

    /**
     * @return Whether or not this node is expanded
     */
    bool IsExpanded() const;

    /**
     * @return Whether this node is explicitly hidden from the view or not
     */
    virtual bool IsHidden() const = 0;

    /**
     * Check whether the node's tree view or track area widgets are hovered by the user's mouse.
     *
     * @return true if hovered, false otherwise. */
    bool IsHovered() const;

    /** Initialize this node with expansion states and virtual offsets */
    void Initialize(float InVirtualTop, float InVirtualBottom);


    /** @return this node's virtual offset from the top of the tree, irrespective of expansion states */
    float GetVirtualTop() const
    {
        return mVirtualTop;
    }

    /** @return this node's virtual offset plus its virtual height, irrespective of expansion states */
    float GetVirtualBottom() const
    {
        return mVirtualBottom;
    }

    /**
     * Returns whether or not this node can be dragged.
     */
    virtual bool CanDrag() const { return false; }

    /**
     * Determines if there is a valid drop zone based on the current drag drop operation and the zone the items were dragged onto.
     */
    virtual TOptional<EItemDropZone> CanDrop( FOdysseyLayerStackNodeDragDropOp& DragDropOp, EItemDropZone ItemDropZone ) const { return TOptional<EItemDropZone>(); }

    /**
     * Handles a drop of items onto this display node.
     */
    virtual void Drop( const TArray<TSharedRef<OdysseyBaseLayerNode>>& DraggedNodes, EItemDropZone DropZone ) { }

    /** Clears the parent of this node. */
    void ClearParent() { mParentNode = nullptr; }

    void MoveNodeTo( EItemDropZone ItemDropZone, TSharedRef<OdysseyBaseLayerNode> CurrentNode );

protected: //PROTECTED API

    /** Adds a child to this node, and sets it's parent to this node. */
    void AddChildAndSetParent( TSharedRef<OdysseyBaseLayerNode> InChild );


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
    /** The parent of this node*/
    TWeakPtr<OdysseyBaseLayerNode> mParentNode;

    /** List of children belonging to this node */
    TArray<TSharedRef<OdysseyBaseLayerNode>> mChildNodes;

    /** Parent tree that this node is in */
    FOdysseyLayerStackTree& mParentTree;

    /** The name identifier of this node */
    FName mNodeName;

    /** Whether or not the node is expanded */
    bool mExpanded;

    /** Event that is triggered when rename is requested */
    FRequestRenameEvent mRenameRequestedEvent;

    /** The interface ptr to the data represented by this node, only used as verification purposes */
    IOdysseyLayer* mLayerDataPtr;
};
