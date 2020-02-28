// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/LayersGUI/OdysseyBaseLayerNode.h"
#include "OdysseyFolderLayer.h"

/**
 * Base node GUI for a layer folder in the layerStack
 */
class OdysseyFolderLayerNode : public OdysseyBaseLayerNode
{
    friend class SOdysseyFolderLayerNodePropertyView; //To link the content of the node with the content of the property view

public:
    //CONSTRUCTION/DESTRUCTION
    OdysseyFolderLayerNode( FOdysseyFolderLayer& InFolderLayer, TSharedPtr<OdysseyBaseLayerNode> InParentNode, FOdysseyLayerStackTree& InParentTree );

public:
    // ODYSSEYBASELAYERNODE IMPLEMENTATION
    virtual float GetNodeHeight() const override;
    virtual FNodePadding GetNodePadding() const override;
    virtual TOptional<EItemDropZone> CanDrop(FOdysseyLayerStackNodeDragDropOp& DragDropOp, EItemDropZone ItemDropZone) const override;
    virtual void Drop(const TArray<TSharedRef<OdysseyBaseLayerNode>>& DraggedNodes, EItemDropZone ItemDropZone) override;
    virtual TSharedRef<SWidget> GenerateContainerWidgetForPropertyView() override;
    virtual const FSlateBrush* GetIconBrush() const override;
    virtual TSharedRef<SWidget> GetCustomIconContent() override;
    virtual TSharedRef<SWidget> GetCustomOutlinerContent() override;
    virtual void BuildContextMenu(FMenuBuilder& MenuBuilder) override;
    virtual bool IsHidden() const override;

private: // HANDLES
    bool HandleDeleteLayerCanExecute() const;
    bool HandleFlattenLayerCanExecute() const;
    bool HandleDuplicateLayerCanExecute() const;
    FReply HandleExpand();


protected:
    //PROTECTED API
    const FSlateBrush* GetVisibilityBrushForLayer() const;
    FReply OnToggleVisibility();
    const FSlateBrush* GetLockedBrushForLayer() const;
    FReply OnToggleLocked();
    void RefreshOpacityText() const;
    void RefreshBlendingModeText() const;
    
private:
    /** The brush used to draw the icon when this folder is open .*/
    const FSlateBrush* mFolderOpenBrush;

    /** The brush used to draw the icon when this folder is closed. */
    const FSlateBrush* mFolderClosedBrush;
    
private:
    SHorizontalBox::FSlot* mOpacityText;
    SHorizontalBox::FSlot* mBlendingModeText;
};
