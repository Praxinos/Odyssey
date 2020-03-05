// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/LayersGUI/OdysseyBaseLayerNode.h"
#include "LayerStack/LayersGUI/SOdysseyImageLayerNodePropertyView.h"
#include "LayerStack/LayersGUI/SOdysseyFolderLayerNodePropertyView.h"

#include "LayerStack/SOdysseyLayerStackOutlinerTreeNode.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Views/STableRow.h" // For EItemZone
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "IOdysseyBaseLayerNode"


//CONSTRUCTION/DESTRUCTION --------------------------------------

IOdysseyBaseLayerNode::IOdysseyBaseLayerNode( FName iNodeName, TSharedPtr<IOdysseyBaseLayerNode> iParentNode, FOdysseyLayerStackTree& iParentTree, IOdysseyLayer* iLayerDataPtr )
    : mVirtualTop( 0.f )
    , mVirtualBottom( 0.f )
    , mParentNode( iParentNode )
    , mParentTree( iParentTree )
    , mNodeName( iNodeName )
    , mExpanded( false )
    , mLayerDataPtr( iLayerDataPtr )
{
}

//PUBLIC API-----------------------------------------------------

bool IOdysseyBaseLayerNode::CanRenameNode() const
{
    return true;
}

FText IOdysseyBaseLayerNode::GetDisplayName() const
{
    return FText::FromName( mLayerDataPtr->GetName() );
}

void IOdysseyBaseLayerNode::SetDisplayName(const FText& iNewDisplayName)
{
    mLayerDataPtr->SetName( FName( *iNewDisplayName.ToString() ) );
}

FLinearColor IOdysseyBaseLayerNode::GetDisplayNameColor() const
{
    return FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

FText IOdysseyBaseLayerNode::GetDisplayNameToolTipText() const
{
    return FText();
}

TSharedRef<SWidget> IOdysseyBaseLayerNode::GenerateContainerWidgetForOutliner(const TSharedRef<SOdysseyLayerStackViewRow>& iRow)
{
    auto newWidget = 
    SNew(SOdysseyLayerStackOutlinerTreeNode, SharedThis(this), iRow)
    .IconToolTipText(this, &IOdysseyBaseLayerNode::GetIconToolTipText)
    .IconContent()
    [
        GetCustomIconContent()
    ]
    .CustomContent()
    [
        GetCustomOutlinerContent()
    ];

    return newWidget;
}

const FSlateBrush* IOdysseyBaseLayerNode::GetIconBrush() const
{
    return nullptr;
}

FSlateColor IOdysseyBaseLayerNode::GetIconColor() const
{
    return FSlateColor( FLinearColor::White );
}

FText IOdysseyBaseLayerNode::GetIconToolTipText() const
{
    return FText();
}

FString IOdysseyBaseLayerNode::GetPathName() const
{
    // First get our parent's path
    FString pathName;

    if (mParentNode.IsValid())
    {
        ensure(mParentNode != SharedThis(this));
        pathName = mParentNode.Pin()->GetPathName() + TEXT(".");
    }

    //then append our path
    pathName += GetNodeName().ToString();

    return pathName;
}

TSharedPtr<SWidget> IOdysseyBaseLayerNode::OnSummonContextMenu()
{
    const bool bShouldCloseWindowAfterMenuSelection = true;
    FMenuBuilder menuBuilder(bShouldCloseWindowAfterMenuSelection, mParentTree.GetLayerStack().GetCommandBindings());
    BuildContextMenu(menuBuilder);

    return menuBuilder.MakeWidget();
}

void IOdysseyBaseLayerNode::SetExpansionState(bool bInExpanded)
{
    mExpanded = bInExpanded;
}


bool IOdysseyBaseLayerNode::IsExpanded() const
{
    return mExpanded;
}


bool IOdysseyBaseLayerNode::IsHidden() const
{
    return false;
}


bool IOdysseyBaseLayerNode::IsHovered() const
{
    return false;
}

void IOdysseyBaseLayerNode::Initialize(float iVirtualTop, float iVirtualBottom)
{
    mVirtualTop = iVirtualTop;
    mVirtualBottom = iVirtualBottom;
}


void IOdysseyBaseLayerNode::MoveNodeTo( EItemDropZone iItemDropZone, TSharedRef<IOdysseyBaseLayerNode> iCurrentNode )
{
    //TODO: make the same thing with callbacks so we don't have to manipulate the layer stack manually here
    TArray< IOdysseyLayer* > layersData = TArray<IOdysseyLayer*>();
    mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->DepthFirstSearchTree( &layersData, false );
        
    int indexBase = -1;
    for( int i = 0; i < mParentTree.GetRootNodes().Num(); i++)
    {
        if( this == &(mParentTree.GetRootNodes())[i].Get())
            indexBase = i;
    }

    int indexTarget = mParentTree.GetRootNodes().Find( iCurrentNode );

    if( indexBase == indexTarget )
        return;
        
    FOdysseyNTree<IOdysseyLayer*>* layerBase = mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->FindNode( layersData[indexBase] );
    FOdysseyNTree<IOdysseyLayer*>* layerTarget = mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->FindNode( layersData[indexTarget] );
    
    //We can't move a folder inside itself, this would make the tree invalid, so, we prevent it.
    if( layersData[indexBase]->GetType() == IOdysseyLayer::eType::kFolder && layerTarget->HasForParent(layerBase) )
        return;
    
    if( iItemDropZone == EItemDropZone::BelowItem )
    {
        layerBase->MoveNodeTo( layerTarget, ePosition::kAfter );
    }
    else if( iItemDropZone == EItemDropZone::AboveItem )
    {
        layerBase->MoveNodeTo( layerTarget, ePosition::kBefore );
    }
    else if( iItemDropZone == EItemDropZone::OntoItem && layersData[indexTarget]->GetType() == IOdysseyLayer::eType::kFolder )
    {
        layerBase->MoveNodeTo( layerTarget, ePosition::kIn );
    }
    else
    {
        return;
    }


    mParentTree.OnUpdated().Broadcast();
    mParentTree.GetLayerStack().GetLayerStackData()->ComputeResultBlock();
}


//PROTECTED API------------------------------------------

void IOdysseyBaseLayerNode::AddChildAndSetParent( TSharedRef<IOdysseyBaseLayerNode> iChild )
{
    mChildNodes.Add( iChild );
    iChild->mParentNode = SharedThis( this );
}

//HANDLES-------------------------------------------------------

void IOdysseyBaseLayerNode::HandleContextMenuRenameNodeExecute()
{
    mRenameRequestedEvent.Broadcast();
}


bool IOdysseyBaseLayerNode::HandleContextMenuRenameNodeCanExecute() const
{
    return CanRenameNode();
}

//--------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
