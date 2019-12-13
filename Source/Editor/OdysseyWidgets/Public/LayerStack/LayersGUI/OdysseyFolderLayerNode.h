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
    friend class SOdysseyLayerStackPropertyViewTreeNode; //To link the content of the node with the content of the property view

public:
    //CONSTRUCTION/DESTRUCTION
    OdysseyFolderLayerNode( FOdysseyFolderLayer& InFolderLayer, TSharedPtr<OdysseyBaseLayerNode> InParentNode, FOdysseyLayerStackTree& InParentTree );

public:
    // ODYSSEYBASELAYERNODE IMPLEMENTATION
    virtual float GetNodeHeight() const override;
    virtual FNodePadding GetNodePadding() const override;
    virtual bool CanRenameNode() const override;

private:
    /** The brush used to draw the icon when this folder is open .*/
    const FSlateBrush* FolderOpenBrush;

    /** The brush used to draw the icon when this folder is closed. */
    const FSlateBrush* FolderClosedBrush;
};
