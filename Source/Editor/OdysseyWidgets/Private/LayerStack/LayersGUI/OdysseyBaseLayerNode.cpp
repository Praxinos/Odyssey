// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/LayersGUI/OdysseyBaseLayerNode.h"
#include "LayerStack/SOdysseyLayerStackOutlinerTreeNode.h"
#include "LayerStack/SOdysseyLayerStackPropertyViewTreeNode.h"
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

TSharedRef<SWidget> OdysseyBaseLayerNode::GenerateContainerWidgetForPropertyView()
{
    auto NewWidget = SNew(SOdysseyLayerStackPropertyViewTreeNode, SharedThis(this) );

    return NewWidget;
}

TSharedRef<SWidget> OdysseyBaseLayerNode::GenerateContainerWidgetForOutliner(const TSharedRef<SOdysseyLayerStackViewRow>& InRow)
{
    auto NewWidget = SNew(SOdysseyLayerStackOutlinerTreeNode, SharedThis(this), InRow)
    .IconBrush(this, &OdysseyBaseLayerNode::GetIconBrush)
    .IconColor(this, &OdysseyBaseLayerNode::GetIconColor)
    .IconOverlayBrush(this, &OdysseyBaseLayerNode::GetIconOverlayBrush)
    .IconToolTipText(this, &OdysseyBaseLayerNode::GetIconToolTipText)
    .CustomContent()
    [
        GetCustomOutlinerContent()
    ];

    return NewWidget;
}

TSharedRef<SWidget> OdysseyBaseLayerNode::GetCustomOutlinerContent()
{
    return SNew(SSpacer);
}


const FSlateBrush* OdysseyBaseLayerNode::GetIconBrush() const
{
    return nullptr;
}

const FSlateBrush* OdysseyBaseLayerNode::GetIconOverlayBrush() const
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

void OdysseyBaseLayerNode::BuildContextMenu(FMenuBuilder& MenuBuilder)
{
    TSharedRef<OdysseyBaseLayerNode> ThisNode = SharedThis(this);

    MenuBuilder.BeginSection("Edit", LOCTEXT("EditContextMenuSectionName", "Edit"));
    {
            MenuBuilder.AddMenuEntry(
            LOCTEXT("DeleteLayer", "Delete"),
            LOCTEXT("DeleteLayerTooltip", "Delete this Layer"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetActions.Delete"),
                                     FUIAction(FExecuteAction::CreateSP(&(ParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnDeleteLayer, LayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &OdysseyBaseLayerNode::HandleDeleteLayerCanExecute)));

            MenuBuilder.AddMenuEntry(
            LOCTEXT("MergeDownLayer", "Merge Down"),
            LOCTEXT("MergeDownLayerTooltip", "Merge this Layer Down"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "MergeDownIcon"),
                                     FUIAction(FExecuteAction::CreateSP(&(ParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnMergeLayerDown, LayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &OdysseyBaseLayerNode::HandleMergeLayerDownCanExecute)));
        
            MenuBuilder.AddMenuEntry(
            LOCTEXT("DuplicateLayer", "Duplicate Layer"),
            LOCTEXT("DuplicateLayerTooltip", "Duplicate this Layer"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "DuplicateLayerIcon"),
                                     FUIAction(FExecuteAction::CreateSP(&(ParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnDuplicateLayer, LayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &OdysseyBaseLayerNode::HandleDuplicateLayerCanExecute)));
    }
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

    if( ItemDropZone == EItemDropZone::OntoItem )
        return;

    //TODO: make the same thing with callbacks so we don't have to manipulate the layer stack manually here
    TArray< TSharedPtr< IOdysseyLayer > >* LayersData = ParentTree.GetLayerStack().GetLayerStackData()->GetLayers();

    int indexBase = -1;
    for( int i = 0; i < ParentTree.GetRootNodes().Num(); i++)
    {
        if( this == &(ParentTree.GetRootNodes())[i].Get())
            indexBase = i;
    }

    int indexTarget = ParentTree.GetRootNodes().Find( CurrentNode );

    if( ItemDropZone == EItemDropZone::BelowItem && indexTarget < indexBase )
    {
        indexTarget++;
    }

    if( ItemDropZone == EItemDropZone::AboveItem && indexTarget > indexBase )
    {
        indexTarget--;
    }

    //Rebase in the order of the layerStack data
    indexTarget = ParentTree.GetRootNodes().Num() - 1 - indexTarget;
    indexBase = ParentTree.GetRootNodes().Num() - 1 - indexBase;

    if( indexBase == indexTarget )
        return;

    TSharedPtr<IOdysseyLayer> Layer = (*LayersData)[indexBase];

    LayersData->Remove(Layer);
    LayersData->Insert( Layer, indexTarget );

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

bool OdysseyBaseLayerNode::HandleDeleteLayerCanExecute() const
{
    return (ParentTree.GetRootNodes().Num() > 1);
}

bool OdysseyBaseLayerNode::HandleMergeLayerDownCanExecute() const
{
    return (ParentTree.GetLayerStack().GetLayerStackData()->GetCurrentLayerIndex() != 0);
}

bool OdysseyBaseLayerNode::HandleDuplicateLayerCanExecute() const
{
    return true;
}

//--------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
