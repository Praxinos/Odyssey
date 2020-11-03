// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/LayersGUI/OdysseyBaseLayerNode.h"
#include "LayerStack/LayersGUI/SOdysseyImageLayerNodePropertyView.h"
#include "OdysseyImageLayer.h"
#include "Widgets/SBoxPanel.h"


DECLARE_DELEGATE( FOnRefreshOpacityText );

/**
 * Base node GUI for a layer trackable in time in the layerStack
 */
class FOdysseyImageLayerNode : public IOdysseyBaseLayerNode
{
    friend class SOdysseyImageLayerNodePropertyView; //To link the content of the node with the content of the property view

public:
    //CONSTRUCTION/DESTRUCTION
    FOdysseyImageLayerNode( TSharedPtr<FOdysseyImageLayer> iImageLayer, FOdysseyLayerStackTree& iParentTree );

public:
    // ODYSSEYBASELAYERNODE IMPLEMENTATION
    virtual float GetNodeHeight() const override;
    virtual FNodePadding GetNodePadding() const override;
    virtual TOptional<EItemDropZone> CanDrop(FOdysseyLayerStackNodeDragDropOp& iDragDropOp, EItemDropZone iItemDropZone) const override;
    virtual void Drop(const TArray<TSharedRef<IOdysseyBaseLayerNode>>& iDraggedNodes, EItemDropZone iItemDropZone) override;
    virtual const FSlateBrush* GetIconBrush() const override;
    virtual TSharedRef<SWidget> GenerateContainerWidgetForPropertyView() override;
    virtual TSharedRef<SWidget> GetCustomIconContent() override;
    virtual TSharedRef<SWidget> GetCustomOutlinerContent() override;
    virtual void BuildContextMenu(FMenuBuilder& MenuBuilder) override;
    virtual bool IsHidden() const override;

private: // HANDLES
    bool HandleDeleteLayerCanExecute() const;
    bool HandleMergeLayerDownCanExecute() const;
    bool HandleDuplicateLayerCanExecute() const;
    
protected:
    //PROTECTED API
    const FSlateBrush* GetVisibilityBrushForLayer() const;
    FReply OnToggleVisibility();
    const FSlateBrush* GetLockedBrushForLayer() const;
    FReply OnToggleLocked();
    const FSlateBrush* GetAlphaLockedBrushForLayer() const;
    FReply OnToggleAlphaLocked();
    void RefreshOpacityText() const;
    void RefreshBlendingModeText() const;



private:
    SHorizontalBox::FSlot* mOpacityText;
    SHorizontalBox::FSlot* mBlendingModeText;


};
