// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/FOdysseyLayerStackTree.h"

#include "OdysseyFolderLayer.h"
#include "OdysseyImageLayer.h"
#include "OdysseyLayerStack.h"
#include "LayerStack/LayersGUI/OdysseyFolderLayerNode.h"
#include "LayerStack/LayersGUI/OdysseyTrackLayerNode.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStackTree"


//PUBLIC API-------------------------------------

void FOdysseyLayerStackTree::Empty()
{
    RootNodes.Empty();
    HoveredNode = nullptr;
}


int FOdysseyLayerStackTree::Update()
{
    TArray< TSharedPtr< IOdysseyLayer > >* layersData = LayerStack.GetLayerStackData()->GetLayers();
    TArray< TSharedRef<OdysseyBaseLayerNode> > rootNodesCopy = RootNodes;
    bool found;
    int newIndex = -1;

    Empty();

    for( int i = layersData->Num() - 1; i >= 0; i-- )
    {
        found = false;
        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( (*layersData)[i].Get() );

        for( int j = 0; j < rootNodesCopy.Num(); j++)
        {
            if( rootNodesCopy[j]->GetLayerDataPtr() == imageLayer )
            {
                RootNodes.Add( rootNodesCopy[j] );
                found = true;
                break; //We found the node linked to this data, we don't have anything more to do
            }
        }

        if (!found)
        {
            RootNodes.Add( MakeShareable(new OdysseyTrackLayerNode( *imageLayer, nullptr, *this )) );
            newIndex = i;
        }
    }
    if( newIndex != -1 )
    {
        LayerStack.GetLayerStackData()->SetCurrentLayerIndex( newIndex );
        return newIndex;
    }


    //UE_LOG(LogTemp, Display, TEXT("LayerSelected: %d"), LayerStack.GetLayerStackData()->GetCurrentLayerIndex());
    return LayerStack.GetLayerStackData()->GetCurrentLayerIndex();
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

TSharedRef<OdysseyBaseLayerNode> CreateFolderNode( FOdysseyFolderLayer& FolderLayer, FOdysseyLayerStackTree& NodeTree )
{
    TSharedRef<OdysseyFolderLayerNode> FolderNode( new OdysseyFolderLayerNode( FolderLayer, TSharedPtr<OdysseyBaseLayerNode>(), NodeTree ) );

    return FolderNode;
}


#undef LOCTEXT_NAMESPACE
