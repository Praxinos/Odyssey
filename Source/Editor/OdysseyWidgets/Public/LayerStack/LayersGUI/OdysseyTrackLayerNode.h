// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/LayersGUI/OdysseyBaseLayerNode.h"
#include "OdysseyImageLayer.h"
#include "Widgets/SBoxPanel.h"


DECLARE_DELEGATE( FOnRefreshOpacityText );

/**
 * Base node GUI for a layer trackable in time in the layerStack
 */
class OdysseyTrackLayerNode : public OdysseyBaseLayerNode
{
    friend class SOdysseyLayerStackPropertyViewTreeNode; //To link the content of the node with the content of the property view

public:
    //CONSTRUCTION/DESTRUCTION
    OdysseyTrackLayerNode( FOdysseyImageLayer& InImageLayer, TSharedPtr<OdysseyBaseLayerNode> InParentNode, FOdysseyLayerStackTree& InParentTree );

public:
    // ODYSSEYBASELAYERNODE IMPLEMENTATION
    virtual float GetNodeHeight() const override;
    virtual FNodePadding GetNodePadding() const override;
    virtual bool CanRenameNode() const override;
    virtual TOptional<EItemDropZone> CanDrop(FOdysseyLayerStackNodeDragDropOp& DragDropOp, EItemDropZone ItemDropZone) const override;
    virtual void Drop(const TArray<TSharedRef<OdysseyBaseLayerNode>>& DraggedNodes, EItemDropZone ItemDropZone) override;
    virtual const FSlateBrush* GetIconBrush() const override;
    virtual TSharedRef<SWidget> GetCustomOutlinerContent() override;

protected:
    //PROTECTED API
    const FSlateBrush* GetVisibilityBrushForLayer() const;
    FReply OnToggleVisibility();
    const FSlateBrush* GetLockedBrushForLayer() const;
    FReply OnToggleLocked();
    void RefreshOpacityText() const;
    void RefreshBlendingModeText() const;


private:
    SHorizontalBox::FSlot* OpacityText;
    SHorizontalBox::FSlot* BlendingModeText;


};
