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

#define LOCTEXT_NAMESPACE "OdysseyBaseLayerNode"


//CONSTRUCTION/DESTRUCTION --------------------------------------

OdysseyBaseLayerNode::OdysseyBaseLayerNode( FName InNodeName, TSharedPtr<OdysseyBaseLayerNode> InParentNode, FOdysseyLayerStackTree& InParentTree, IOdysseyLayer* InLayerDataPtr )
    : VirtualTop( 0.f )
    , VirtualBottom( 0.f )
    , ParentNode( InParentNode )
    , ParentTree( InParentTree )
    , NodeName( InNodeName )
    , bExpanded( false )
    , LayerDataPtr( InLayerDataPtr )
{
}

//PUBLIC API-----------------------------------------------------

bool OdysseyBaseLayerNode::CanRenameNode() const
{
    return true;
}

FText OdysseyBaseLayerNode::GetDisplayName() const
{
    return FText::FromName( LayerDataPtr->GetName() );
}

void OdysseyBaseLayerNode::SetDisplayName(const FText& NewDisplayName)
{
    LayerDataPtr->SetName( FName( *NewDisplayName.ToString() ) );
}

FLinearColor OdysseyBaseLayerNode::GetDisplayNameColor() const
{
    return FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

FText OdysseyBaseLayerNode::GetDisplayNameToolTipText() const
{
    return FText();
}

TSharedRef<SWidget> OdysseyBaseLayerNode::GenerateContainerWidgetForOutliner(const TSharedRef<SOdysseyLayerStackViewRow>& InRow)
{
    auto NewWidget = SNew(SOdysseyLayerStackOutlinerTreeNode, SharedThis(this), InRow)
    .IconToolTipText(this, &OdysseyBaseLayerNode::GetIconToolTipText)
    .IconContent()
    [
        GetCustomIconContent()
    ]
    .CustomContent()
    [
        GetCustomOutlinerContent()
    ];

    return NewWidget;
}

const FSlateBrush* OdysseyBaseLayerNode::GetIconBrush() const
{
    return nullptr;
}

FSlateColor OdysseyBaseLayerNode::GetIconColor() const
{
    return FSlateColor( FLinearColor::White );
}

FText OdysseyBaseLayerNode::GetIconToolTipText() const
{
    return FText();
}

FString OdysseyBaseLayerNode::GetPathName() const
{
    // First get our parent's path
    FString PathName;

    if (ParentNode.IsValid())
    {
        ensure(ParentNode != SharedThis(this));
        PathName = ParentNode.Pin()->GetPathName() + TEXT(".");
    }

    //then append our path
    PathName += GetNodeName().ToString();

    return PathName;
}

TSharedPtr<SWidget> OdysseyBaseLayerNode::OnSummonContextMenu()
{
    const bool bShouldCloseWindowAfterMenuSelection = true;
    FMenuBuilder MenuBuilder(bShouldCloseWindowAfterMenuSelection, ParentTree.GetLayerStack().GetCommandBindings());
    BuildContextMenu(MenuBuilder);

    return MenuBuilder.MakeWidget();
}

void OdysseyBaseLayerNode::SetExpansionState(bool bInExpanded)
{
    bExpanded = bInExpanded;
}


bool OdysseyBaseLayerNode::IsExpanded() const
{
    return bExpanded;
}


bool OdysseyBaseLayerNode::IsHidden() const
{
    return false;
}


bool OdysseyBaseLayerNode::IsHovered() const
{
    return false;
}

void OdysseyBaseLayerNode::Initialize(float InVirtualTop, float InVirtualBottom)
{
    VirtualTop = InVirtualTop;
    VirtualBottom = InVirtualBottom;
}


void OdysseyBaseLayerNode::MoveNodeTo( EItemDropZone ItemDropZone, TSharedRef<OdysseyBaseLayerNode> CurrentNode )
{
    //TODO: make the same thing with callbacks so we don't have to manipulate the layer stack manually here
    TArray< IOdysseyLayer* > layersData = TArray<IOdysseyLayer*>();
    ParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->DepthFirstSearchTree( &layersData, false );
        
    int indexBase = -1;
    for( int i = 0; i < ParentTree.GetRootNodes().Num(); i++)
    {
        if( this == &(ParentTree.GetRootNodes())[i].Get())
            indexBase = i;
    }

    int indexTarget = ParentTree.GetRootNodes().Find( CurrentNode );

    if( indexBase == indexTarget )
        return;
        
    FOdysseyNTree<IOdysseyLayer*>* layerBase = ParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->FindNode( layersData[indexBase] );
    FOdysseyNTree<IOdysseyLayer*>* layerTarget = ParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->FindNode( layersData[indexTarget] );
    
    //We can't move a folder inside itself, this would make the tree invalid, so, we prevent it.
    if( layersData[indexBase]->GetType() == IOdysseyLayer::eType::kFolder && layerTarget->HasForParent(layerBase) )
        return;
    
    if( ItemDropZone == EItemDropZone::BelowItem )
    {
        layerBase->MoveNodeTo( layerTarget, ePosition::kAfter );
    }
    else if( ItemDropZone == EItemDropZone::AboveItem )
    {
        layerBase->MoveNodeTo( layerTarget, ePosition::kBefore );
    }
    else if( ItemDropZone == EItemDropZone::OntoItem && layersData[indexTarget]->GetType() == IOdysseyLayer::eType::kFolder )
    {
        layerBase->MoveNodeTo( layerTarget, ePosition::kIn );
    }
    else
    {
        return;
    }


    ParentTree.OnUpdated().Broadcast();
    ParentTree.GetLayerStack().GetLayerStackData()->ComputeResultBlock();
}


//PROTECTED API------------------------------------------

void OdysseyBaseLayerNode::AddChildAndSetParent( TSharedRef<OdysseyBaseLayerNode> InChild )
{
    ChildNodes.Add( InChild );
    InChild->ParentNode = SharedThis( this );
}

//HANDLES-------------------------------------------------------

void OdysseyBaseLayerNode::HandleContextMenuRenameNodeExecute()
{
    RenameRequestedEvent.Broadcast();
}


bool OdysseyBaseLayerNode::HandleContextMenuRenameNodeCanExecute() const
{
    return CanRenameNode();
}

//--------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
