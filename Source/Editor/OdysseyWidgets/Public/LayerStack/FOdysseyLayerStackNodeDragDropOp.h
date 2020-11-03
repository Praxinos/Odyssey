// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Input/DragAndDrop.h"
#include "LayerStack/LayersGUI/OdysseyBaseLayerNode.h"
#include "GraphEditorDragDropAction.h"

class IOdysseyBaseLayerNode;

/**
 * An utility class to handle the drag n drop of nodes in the tree
 */
class FOdysseyLayerStackNodeDragDropOp : public FGraphEditorDragDropAction
{
public: //STATIC

    /** Construct a new drag/drop operation for dragging a selection of display nodes */
    static TSharedRef<FOdysseyLayerStackNodeDragDropOp> New(TArray<TSharedRef<IOdysseyBaseLayerNode>>& iDraggedNodes, FText iDefaultText, const FSlateBrush* iDefaultIcon);

public: //INTERFACE IMPLEMENTATION
    //~ FDragDropOperation interface
    virtual void Construct() override;

public: //PUBLIC API

    FText GetDecoratorText() const;

    const FSlateBrush* GetDecoratorIcon() const;

    TArray<TSharedRef<IOdysseyBaseLayerNode>>& GetDraggedNodes();

    const FSlateBrush* GetCurrentIconBrush() const;

    void SetCurrentIconBrush( const FSlateBrush* iNewIconBrush );

    void ResetToDefaultToolTip();

private:
    
    /** Current string to show as the decorator text */
    FText mCurrentHoverText;

    /** Current icon to be displayed on the decorator */
    const FSlateBrush* mCurrentIconBrush;
    
    /** The nodes currently being dragged. */
    TArray<TSharedRef<IOdysseyBaseLayerNode>> mDraggedNodes;

    /** Default string to show as hover text */
    FText mDefaultHoverText;

    /** Default icon to be displayed */
    const FSlateBrush* mDefaultHoverIcon;
};
