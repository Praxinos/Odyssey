// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "LayerStack/FOdysseyLayerStackTree.h"

#include "IOdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "LayerStack/LayersGUI/OdysseyFolderLayerNode.h"
#include "LayerStack/LayersGUI/OdysseyImageLayerNode.h"
#include "OdysseyTreeShared.h"

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
	Empty();

	if (!mLayerStack.GetLayerStackData())
	{
		return -1;
	}

	TArray<TSharedPtr<IOdysseyLayer>> layersData;
	mLayerStack.GetLayerStackData()->GetLayerRoot()->DepthFirstSearchTree(&layersData, false);
    for( int i = 0; i < layersData.Num(); i++ )
    {
        if( layersData[i]->GetType() == IOdysseyLayer::eType::kImage )
            mRootNodes.Add( MakeShareable(new FOdysseyImageLayerNode( StaticCastSharedPtr<FOdysseyImageLayer>(layersData[i]), *this )) );
        else if( layersData[i]->GetType() == IOdysseyLayer::eType::kFolder )
            mRootNodes.Add( MakeShareable(new FOdysseyFolderLayerNode( StaticCastSharedPtr<FOdysseyFolderLayer>(layersData[i]), *this )) );
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
