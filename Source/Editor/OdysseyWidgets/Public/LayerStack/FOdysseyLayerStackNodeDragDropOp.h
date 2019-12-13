// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Input/DragAndDrop.h"
#include "LayerStack/LayersGUI/OdysseyBaseLayerNode.h"
#include "GraphEditorDragDropAction.h"

class OdysseyBaseLayerNode;

/**
 * An utility class to handle the drag n drop of nodes in the tree
 */
class FOdysseyLayerStackNodeDragDropOp : public FGraphEditorDragDropAction
{
public: //STATIC

    /**
     * Construct a new drag/drop operation for dragging a selection of display nodes
     */
    static TSharedRef<FOdysseyLayerStackNodeDragDropOp> New(TArray<TSharedRef<OdysseyBaseLayerNode>>& InDraggedNodes, FText InDefaultText, const FSlateBrush* InDefaultIcon);

public: //INTERFACE IMPLEMENTATION

    //~ FGraphEditorDragDropAction interface
    virtual void HoverTargetChanged() override;
    virtual FReply DroppedOnPanel( const TSharedRef< class SWidget >& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph) override;

    //~ FDragDropOperation interface
    virtual void Construct() override;

public: //PUBLIC API

    /**
     * Get the current decorator text
     */
    FText GetDecoratorText() const
    {
        return CurrentHoverText;
    }

    /**
     * Get the current decorator icon
     */
    const FSlateBrush* GetDecoratorIcon() const
    {
        return CurrentIconBrush;
    }

    /**
     * Reset the tooltip decorator back to its original state
     */
    void ResetToDefaultToolTip();

    TArray<TSharedRef<OdysseyBaseLayerNode>>& GetDraggedNodes();

public: //PUBLIC MEMBERS

    /**
     * Current string to show as the decorator text
     */
    FText CurrentHoverText;

    /**
     * Current icon to be displayed on the decorator
     */
    const FSlateBrush* CurrentIconBrush;

private:

    /** The nodes currently being dragged. */
    TArray<TSharedRef<OdysseyBaseLayerNode>> DraggedNodes;

    /** Default string to show as hover text */
    FText DefaultHoverText;

    /** Default icon to be displayed */
    const FSlateBrush* DefaultHoverIcon;
};
