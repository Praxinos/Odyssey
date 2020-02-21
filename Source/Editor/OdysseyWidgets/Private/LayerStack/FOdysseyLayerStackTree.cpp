// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/FOdysseyLayerStackTree.h"

#include "OdysseyFolderLayer.h"
#include "OdysseyImageLayer.h"
#include "OdysseyLayerStack.h"
#include "LayerStack/LayersGUI/OdysseyFolderLayerNode.h"
#include "LayerStack/LayersGUI/OdysseyImageLayerNode.h"
#include "OdysseyTree.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStackTree"


//PUBLIC API-------------------------------------

void FOdysseyLayerStackTree::Empty()
{
    RootNodes.Empty();
    HoveredNode = nullptr;
}


int FOdysseyLayerStackTree::Update()
{
    TArray< IOdysseyLayer* > layersData = TArray<IOdysseyLayer*>();
    LayerStack.GetLayerStackData()->GetLayers()->DepthFirstSearchTree( &layersData, false );
    
    TArray< TSharedRef<OdysseyBaseLayerNode> > rootNodesCopy = RootNodes;
    Empty();

    for( int i = 0; i < layersData.Num(); i++ )
    {
        if( layersData[i]->GetType() == IOdysseyLayer::eType::kImage )
            RootNodes.Add( MakeShareable(new OdysseyImageLayerNode( *(static_cast<FOdysseyImageLayer*> (layersData[i])), nullptr, *this )) );
        else if( layersData[i]->GetType() == IOdysseyLayer::eType::kFolder )
            RootNodes.Add( MakeShareable(new OdysseyFolderLayerNode( *(static_cast<FOdysseyFolderLayer*> (layersData[i])), nullptr, *this )) );
    }
    
    return LayerStack.GetLayerStackData()->GetCurrentLayerAsIndex();
}

const TArray< TSharedRef<OdysseyBaseLayerNode> >& FOdysseyLayerStackTree::GetRootNodes() const
{
    return RootNodes;
}

void FOdysseyLayerStackTree::SetHoveredNode(const TSharedPtr<OdysseyBaseLayerNode>& InHoveredNode)
{
    if (InHoveredNode != HoveredNode)
    {
        HoveredNode = InHoveredNode;
    }
}

const TSharedPtr<OdysseyBaseLayerNode>& FOdysseyLayerStackTree::GetHoveredNode() const
{
    return HoveredNode;
}


//---------------------------------------------


#undef LOCTEXT_NAMESPACE
