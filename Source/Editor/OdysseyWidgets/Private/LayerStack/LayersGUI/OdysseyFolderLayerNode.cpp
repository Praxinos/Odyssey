// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/LayersGUI/OdysseyFolderLayerNode.h"

#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "OdysseyFolderLayerNode"


//CONSTRUCTION/DESTRUCTION --------------------------------------

OdysseyFolderLayerNode::OdysseyFolderLayerNode( FOdysseyFolderLayer& InFolderLayer, TSharedPtr<OdysseyBaseLayerNode> InParentNode, FOdysseyLayerStackTree& InParentTree )
    : OdysseyBaseLayerNode( InFolderLayer.GetName(), InParentNode, InParentTree, &InFolderLayer )
{
    FolderOpenBrush = FEditorStyle::GetBrush( "ContentBrowser.AssetTreeFolderOpen" );
    FolderClosedBrush = FEditorStyle::GetBrush( "ContentBrowser.AssetTreeFolderClosed" );
}

// ODYSSEYBASELAYERNODE IMPLEMENTATION---------------------------

float OdysseyFolderLayerNode::GetNodeHeight() const
{
    return 20.0f;
}

FNodePadding OdysseyFolderLayerNode::GetNodePadding() const
{
    return FNodePadding(4, 4);
}

bool OdysseyFolderLayerNode::CanRenameNode() const
{
    return true;
}

//---------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
