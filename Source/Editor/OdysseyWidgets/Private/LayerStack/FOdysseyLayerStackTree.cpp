// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/FOdysseyLayerStackTree.h"

#include "IOdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "LayerStack/LayersGUI/OdysseyFolderLayerNode.h"
#include "LayerStack/LayersGUI/OdysseyImageLayerNode.h"
#include "OdysseyTree.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStackTree"

//CONSTRUCTION-------------------------------------

FOdysseyLayerStackTree::FOdysseyLayerStackTree( FOdysseyLayerStackModel& iLayerStack )
    : mLayerStack(iLayerStack)
{}

//PUBLIC API-------------------------------------

void FOdysseyLayerStackTree::Empty()
{
    mRootNodes.Empty();
}


int FOdysseyLayerStackTree::Update()
{
    TArray< IOdysseyLayer* > layersData = TArray<IOdysseyLayer*>();
    mLayerStack.GetLayerStackData()->GetLayers()->DepthFirstSearchTree( &layersData, false );
    
    Empty();

    for( int i = 0; i < layersData.Num(); i++ )
    {
        if( layersData[i]->GetType() == IOdysseyLayer::eType::kImage )
            mRootNodes.Add( MakeShareable(new FOdysseyImageLayerNode( *(static_cast<FOdysseyImageLayer*> (layersData[i])), *this )) );
        else if( layersData[i]->GetType() == IOdysseyLayer::eType::kFolder )
            mRootNodes.Add( MakeShareable(new FOdysseyFolderLayerNode( *(static_cast<FOdysseyFolderLayer*> (layersData[i])), *this )) );
    }
    
    return mLayerStack.GetLayerStackData()->GetCurrentLayerAsIndex();
}

FOdysseyLayerStackModel& FOdysseyLayerStackTree::GetLayerStack()
{
    return mLayerStack;
}

const TArray< TSharedRef<IOdysseyBaseLayerNode> >& FOdysseyLayerStackTree::GetRootNodes() const
{
    return mRootNodes;
}

//---------------------------------------------


#undef LOCTEXT_NAMESPACE
