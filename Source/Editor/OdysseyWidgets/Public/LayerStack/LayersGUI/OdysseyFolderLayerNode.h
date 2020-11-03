// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/LayersGUI/OdysseyBaseLayerNode.h"
#include "OdysseyFolderLayer.h"

/**
 * Base node GUI for a layer folder in the layerStack
 */
class FOdysseyFolderLayerNode : public IOdysseyBaseLayerNode
{
    friend class SOdysseyFolderLayerNodePropertyView; //To link the content of the node with the content of the property view

public:
    //CONSTRUCTION/DESTRUCTION
    FOdysseyFolderLayerNode( TSharedPtr<FOdysseyFolderLayer> iFolderLayer, FOdysseyLayerStackTree& iParentTree );

public:
    // IOdysseyBaseLayerNode IMPLEMENTATION
    virtual float GetNodeHeight() const override;
    virtual FNodePadding GetNodePadding() const override;
    virtual TOptional<EItemDropZone> CanDrop(FOdysseyLayerStackNodeDragDropOp& iDragDropOp, EItemDropZone iItemDropZone) const override;
    virtual void Drop(const TArray<TSharedRef<IOdysseyBaseLayerNode>>& iDraggedNodes, EItemDropZone iItemDropZone) override;
    virtual TSharedRef<SWidget> GenerateContainerWidgetForPropertyView() override;
    virtual const FSlateBrush* GetIconBrush() const override;
    virtual TSharedRef<SWidget> GetCustomIconContent() override;
    virtual TSharedRef<SWidget> GetCustomOutlinerContent() override;
    virtual void BuildContextMenu(FMenuBuilder& iMenuBuilder) override;
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
