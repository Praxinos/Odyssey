// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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

IOdysseyBaseLayerNode::IOdysseyBaseLayerNode( FName iNodeName, FOdysseyLayerStackTree& iParentTree, TSharedPtr<IOdysseyLayer> iLayerDataPtr )
    : mVirtualTop( 0.f )
    , mVirtualBottom( 0.f )
    , mParentTree( iParentTree )
    , mNodeName( iNodeName )
    , mLayerDataPtr( iLayerDataPtr )
{
}

//PUBLIC API-----------------------------------------------------

bool IOdysseyBaseLayerNode::IsSelectable() const
{
    return true;
}

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

TSharedPtr<SWidget> IOdysseyBaseLayerNode::OnSummonContextMenu()
{
    const bool bShouldCloseWindowAfterMenuSelection = true;
    FMenuBuilder menuBuilder(bShouldCloseWindowAfterMenuSelection, mParentTree.GetLayerStack().GetCommandBindings());
    BuildContextMenu(menuBuilder);

    return menuBuilder.MakeWidget();
}

FName IOdysseyBaseLayerNode::GetNodeName() const
{
    return mNodeName;
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

float IOdysseyBaseLayerNode::GetVirtualTop() const
{
    return mVirtualTop;
}

float IOdysseyBaseLayerNode::GetVirtualBottom() const
{
    return mVirtualBottom;
}

void IOdysseyBaseLayerNode::MoveNodeTo( EItemDropZone iItemDropZone, TSharedRef<IOdysseyBaseLayerNode> iCurrentNode )
{
    //TODO: make the same thing with callbacks so we don't have to manipulate the layer stack manually here
    

	TArray< TSharedPtr<IOdysseyLayer> > layersData;
	mParentTree.GetLayerStack().GetLayerStackData()->GetLayerRoot()->DepthFirstSearchTree(&layersData, false);
        
    int indexBase = -1;
    for( int i = 0; i < mParentTree.GetRootNodes().Num(); i++)
    {
        if( this == &(mParentTree.GetRootNodes())[i].Get())
            indexBase = i;
    }

    int indexTarget = mParentTree.GetRootNodes().Find( iCurrentNode );

    if( indexBase == indexTarget )
        return;
        
    TSharedPtr<IOdysseyLayer> layerBase = layersData[indexBase];
    TSharedPtr<IOdysseyLayer> layerTarget = layersData[indexTarget];
    
    //We can't move a folder inside itself, this would make the tree invalid, so, we prevent it.
    if( layerBase->GetType() == IOdysseyLayer::eType::kFolder && layerTarget->HasForParent(layerBase) )
        return;
    
    if( iItemDropZone == EItemDropZone::BelowItem )
    {
		layerBase->MoveNodeTo(layerTarget, ePosition::kAfter);
    }
    else if( iItemDropZone == EItemDropZone::AboveItem )
    {
		layerBase->MoveNodeTo(layerTarget, ePosition::kBefore);
    }
    else if( iItemDropZone == EItemDropZone::OntoItem && layersData[indexTarget]->GetType() == IOdysseyLayer::eType::kFolder )
    {
		layerBase->MoveNodeTo(layerTarget, ePosition::kIn);
    }
    else
    {
        return;
    }

	mParentTree.GetLayerStack().OnUpdated().Broadcast();
}

FOdysseyLayerStackModel& IOdysseyBaseLayerNode::GetLayerStack() const
{
    return mParentTree.GetLayerStack();
}

FOdysseyLayerStackTree& IOdysseyBaseLayerNode::GetParentTree() const
{
    return mParentTree;
}

TSharedPtr<IOdysseyLayer> IOdysseyBaseLayerNode::GetLayerDataPtr() const
{
    return mLayerDataPtr;
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
