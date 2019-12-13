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
    FNodePadding(float InUniform) : Top(InUniform), Bottom(InUniform) { }
    FNodePadding(float InTop, float InBottom) : Top(InTop), Bottom(InBottom) { }

    /** @return The sum total of the separate padding values */
    float Combined() const
    {
        return Top + Bottom;
    }

    /** Padding to be applied to the top of the node */
    float Top;

    /** Padding to be applied to the bottom of the node */
    float Bottom;
};


namespace ELayerStackNode
{
    enum Type
    {
        Image,
        /* Benign spacer node */
        Spacer,
        /* Folder node */
        Folder
    };
}

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
    FRequestRenameEvent& OnRenameRequested() { return RenameRequestedEvent; }


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
    virtual bool CanRenameNode() const = 0;

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
    virtual TSharedRef<SWidget> GenerateContainerWidgetForPropertyView();

    /**
     * Generates a widget for display in the LayerStack section
     *
     * @return Generated outliner widget
     */
    virtual TSharedRef<SWidget> GenerateContainerWidgetForOutliner(const TSharedRef<SOdysseyLayerStackViewRow>& InRow);


    /**
     * Customizes an outliner widget that is to represent this node
     *
     * @return Content to display on the outliner node
     */
    virtual TSharedRef<SWidget> GetCustomOutlinerContent();


    /**
     * Gets an icon that represents this sequencer display node
     *
     * @return This node's representative icon
     */
    virtual const FSlateBrush* GetIconBrush() const;

    /**
     * Get a brush to overlay on top of the icon for this node
     *
     * @return An overlay brush, or nullptr
     */
    virtual const FSlateBrush* GetIconOverlayBrush() const;

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
    virtual void BuildContextMenu(FMenuBuilder& MenuBuilder);

    /**
     * @return The name of the node (for identification purposes)
     */
    FName GetNodeName() const
    {
        return NodeName;
    }

    /**
     * @return The number of child nodes belonging to this node
     */
    uint32 GetNumChildren() const
    {
        return ChildNodes.Num();
    }

    /**
     * @return A List of all Child nodes belonging to this node
     */
    const TArray<TSharedRef<OdysseyBaseLayerNode>>& GetChildNodes() const
    {
        return ChildNodes;
    }

    /**
     * @return The parent of this node
     */
    TSharedPtr<OdysseyBaseLayerNode> GetParent() const
    {
        return ParentNode.Pin();
    }

    /**
     * @return The outermost parent of this node
     */
    TSharedRef<OdysseyBaseLayerNode> GetOutermostParent()
    {
        TSharedPtr<OdysseyBaseLayerNode> Parent = ParentNode.Pin();
        return Parent.IsValid() ? Parent->GetOutermostParent() : AsShared();
    }

    /** Gets the layerStack that owns this node */
    FOdysseyLayerStackModel& GetLayerStack() const
    {
        return ParentTree.GetLayerStack();
    }

    /** Gets the parent tree that this node is in */
    FOdysseyLayerStackTree& GetParentTree() const
    {
        return ParentTree;
    }

    IOdysseyLayer* GetLayerDataPtr() const
    {
        return LayerDataPtr;
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
    bool IsHidden() const;

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
        return VirtualTop;
    }

    /** @return this node's virtual offset plus its virtual height, irrespective of expansion states */
    float GetVirtualBottom() const
    {
        return VirtualBottom;
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
    void ClearParent() { ParentNode = nullptr; }

    void MoveNodeTo( EItemDropZone ItemDropZone, TSharedRef<OdysseyBaseLayerNode> CurrentNode );

protected: //PROTECTED API

    /** Adds a child to this node, and sets it's parent to this node. */
    void AddChildAndSetParent( TSharedRef<OdysseyBaseLayerNode> InChild );


private: // HANDLES

    /** Callback for executing a "Rename Node" context menu action. */
    void HandleContextMenuRenameNodeExecute();

    /** Callback for determining whether a "Rename Node" context menu action can execute. */
    bool HandleContextMenuRenameNodeCanExecute() const;

    bool HandleDeleteLayerCanExecute() const;

    bool HandleMergeLayerDownCanExecute() const;
    
    bool HandleDuplicateLayerCanExecute() const;


protected:

    /** The virtual offset of this item from the top of the tree, irrespective of expansion states. */
    float VirtualTop;

    /** The virtual offset + virtual height of this item, irrespective of expansion states. */
    float VirtualBottom;


protected:
    /** The parent of this node*/
    TWeakPtr<OdysseyBaseLayerNode> ParentNode;

    /** List of children belonging to this node */
    TArray<TSharedRef<OdysseyBaseLayerNode>> ChildNodes;

    /** Parent tree that this node is in */
    FOdysseyLayerStackTree& ParentTree;

    /** The name identifier of this node */
    FName NodeName;

    /** Whether or not the node is expanded */
    bool bExpanded;

    /** Event that is triggered when rename is requested */
    FRequestRenameEvent RenameRequestedEvent;

    /** The interface ptr to the data represented by this node, only used as verification purposes */
    IOdysseyLayer* LayerDataPtr;
};
