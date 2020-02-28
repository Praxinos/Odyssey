// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/LayersGUI/OdysseyFolderLayerNode.h"

#include "EditorStyleSet.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "OdysseyFolderLayerNode"


//CONSTRUCTION/DESTRUCTION --------------------------------------

OdysseyFolderLayerNode::OdysseyFolderLayerNode( FOdysseyFolderLayer& InFolderLayer, TSharedPtr<OdysseyBaseLayerNode> InParentNode, FOdysseyLayerStackTree& InParentTree )
    : OdysseyBaseLayerNode( InFolderLayer.GetName()
    , InParentNode
    , InParentTree
    , &InFolderLayer )
{
    mFolderOpenBrush = FEditorStyle::GetBrush( "ContentBrowser.AssetTreeFolderOpen" );
    mFolderClosedBrush = FEditorStyle::GetBrush( "ContentBrowser.AssetTreeFolderClosed" );
}

// ODYSSEYBASELAYERNODE IMPLEMENTATION---------------------------

float OdysseyFolderLayerNode::GetNodeHeight() const
{
    return 20.0f;
}

FNodePadding OdysseyFolderLayerNode::GetNodePadding() const
{
    TArray< IOdysseyLayer* > layersData = TArray<IOdysseyLayer*>();
    mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->DepthFirstSearchTree( &layersData, false );
    
    float leftPadding = (mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->FindNode( mLayerDataPtr )->GetNumberParents() - 1) * 10;
        
    return FNodePadding(leftPadding, 4, 4);
}

TOptional<EItemDropZone> OdysseyFolderLayerNode::CanDrop(FOdysseyLayerStackNodeDragDropOp& DragDropOp, EItemDropZone ItemDropZone) const
{
    DragDropOp.ResetToDefaultToolTip();

    return TOptional<EItemDropZone>( ItemDropZone );
}

void OdysseyFolderLayerNode::Drop(const TArray<TSharedRef<OdysseyBaseLayerNode>>& DraggedNodes, EItemDropZone ItemDropZone)
{
    TSharedPtr<OdysseyBaseLayerNode> CurrentNode = SharedThis((OdysseyBaseLayerNode*)this);

    for( TSharedRef<OdysseyBaseLayerNode> DraggedNode: DraggedNodes)
    {
        DraggedNode->MoveNodeTo( ItemDropZone, CurrentNode.ToSharedRef() );
    }
}

const FSlateBrush* OdysseyFolderLayerNode::GetIconBrush() const
{
    FOdysseyFolderLayer* layer = static_cast<FOdysseyFolderLayer*>( GetLayerDataPtr() );

    if( layer->IsOpen() )
        return mFolderOpenBrush;
    else
        return mFolderClosedBrush;
}

TSharedRef<SWidget> OdysseyFolderLayerNode::GenerateContainerWidgetForPropertyView()
{
    return SNew(SOdysseyFolderLayerNodePropertyView, SharedThis(this) );
}

TSharedRef<SWidget> OdysseyFolderLayerNode::GetCustomIconContent()
{
    return SNew(SButton)
           .ButtonStyle( &FOdysseyStyle::GetWidgetStyle<FButtonStyle>("OdysseyLayerStack.Motionless") )
           .VAlign(VAlign_Center)
           .HAlign(HAlign_Center)
           .OnClicked(this, &OdysseyFolderLayerNode::HandleExpand)
           .ContentPadding(0.f)
           .ForegroundColor(FSlateColor::UseForeground())
           .IsFocusable(false)
           [
               SNew(SImage)
               .Image(this, &OdysseyFolderLayerNode::GetIconBrush)
               .ColorAndOpacity(FSlateColor::UseForeground())
           ];
}

TSharedRef<SWidget> OdysseyFolderLayerNode::GetCustomOutlinerContent()
{
    FOdysseyFolderLayer* layer = static_cast<FOdysseyFolderLayer*>( GetLayerDataPtr() );

    return SNew(SHorizontalBox)

        +SHorizontalBox::Slot()
        .HAlign( HAlign_Left )
        .VAlign( VAlign_Center )
        .Expose( mBlendingModeText )
        [
            SNew(STextBlock).Text( layer->GetBlendingModeAsText() )
        ]

        +SHorizontalBox::Slot()
        .HAlign( HAlign_Left )
        .VAlign( VAlign_Center )
        .Expose( mOpacityText )
        [
            SNew(STextBlock).Text( FText::AsPercent( layer->GetOpacity() ) )
        ]

        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
                .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                .OnClicked( this, &OdysseyFolderLayerNode::OnToggleVisibility )
                .ToolTipText( LOCTEXT("OdysseyLayerVisibilityButtonToolTip", "Toggle Layer Visibility") )
                .ForegroundColor( FSlateColor::UseForeground() )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .Content()
                [
                    SNew(SImage)
                    .Image(this, &OdysseyFolderLayerNode::GetVisibilityBrushForLayer)
                ]
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
                .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                .OnClicked( this, &OdysseyFolderLayerNode::OnToggleLocked )
                .ToolTipText( LOCTEXT("OdysseyLayerLockedButtonToolTip", "Toggle Layer Locked State") )
                .ForegroundColor( FSlateColor::UseForeground() )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .Content()
                [
                    SNew(SImage)
                    .Image(this, &OdysseyFolderLayerNode::GetLockedBrushForLayer)
                ]
         ];
}

void OdysseyFolderLayerNode::BuildContextMenu(FMenuBuilder& MenuBuilder)
{
    MenuBuilder.BeginSection("Edit", LOCTEXT("EditContextMenuSectionName", "Edit"));
    {
            MenuBuilder.AddMenuEntry(
            LOCTEXT("DeleteLayer", "Delete"),
            LOCTEXT("DeleteLayerTooltip", "Delete this Layer"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetActions.Delete"),
                                     FUIAction(FExecuteAction::CreateSP(&(mParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnDeleteLayer, mLayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &OdysseyFolderLayerNode::HandleDeleteLayerCanExecute)));

            MenuBuilder.AddMenuEntry(
            LOCTEXT("FlattenLayer", "Flatten Layer"),
            LOCTEXT("FlattenTooltip", "Flatten the folder in one image layer"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "Flatten"),
                                     FUIAction(FExecuteAction::CreateSP(&(mParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnFlattenLayer, mLayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &OdysseyFolderLayerNode::HandleFlattenLayerCanExecute)));
        
            MenuBuilder.AddMenuEntry(
            LOCTEXT("DuplicateLayer", "Duplicate Layer"),
            LOCTEXT("DuplicateLayerTooltip", "Duplicate this Layer"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "DuplicateLayerIcon"),
                                     FUIAction(FExecuteAction::CreateSP(&(mParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnDuplicateLayer, mLayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &OdysseyFolderLayerNode::HandleDuplicateLayerCanExecute)));
    }
}


bool OdysseyFolderLayerNode::IsHidden() const
{
    FOdysseyNTree<IOdysseyLayer*>* layerNode = mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->FindNode( mLayerDataPtr );

    while( layerNode->GetParent()->GetNodeContent() != NULL )
    {
        if( layerNode->GetParent()->GetNodeContent()->GetType() == IOdysseyLayer::eType::kFolder )
            if( !static_cast<FOdysseyFolderLayer*>( layerNode->GetParent()->GetNodeContent() )->IsOpen() )
                return true;
        
        layerNode = layerNode->GetParent();
    }
    
    return false;
}

//-----------------------------------------------------Handles

bool OdysseyFolderLayerNode::HandleDeleteLayerCanExecute() const
{
    return true;
}

bool OdysseyFolderLayerNode::HandleFlattenLayerCanExecute() const
{
    //We won't be able to flatten it everytime when new layer types will be created
    return true;
}

bool OdysseyFolderLayerNode::HandleDuplicateLayerCanExecute() const
{
    return true;
}

FReply OdysseyFolderLayerNode::HandleExpand()
{
    FOdysseyFolderLayer* layer = static_cast<FOdysseyFolderLayer*>( GetLayerDataPtr() );
    layer->SetIsOpen( !layer->IsOpen() );
    
    mParentTree.GetLayerStack().GetLayerStackView()->RefreshView();
    
    return FReply::Handled();
}

//--------------------------------------------------PROTECTED API


const FSlateBrush* OdysseyFolderLayerNode::GetVisibilityBrushForLayer() const
{
    return GetLayerDataPtr()->IsVisible() ? FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16") : FOdysseyStyle::GetBrush("OdysseyLayerStack.NotVisible16");
}

FReply OdysseyFolderLayerNode::OnToggleVisibility()
{
    GetLayerDataPtr()->SetIsVisible( !GetLayerDataPtr()->IsVisible() );
    GetLayerStack().GetLayerStackData()->ComputeResultBlock();
    return FReply::Handled();
}

const FSlateBrush* OdysseyFolderLayerNode::GetLockedBrushForLayer() const
{
    return GetLayerDataPtr()->IsLocked() ? FOdysseyStyle::GetBrush("OdysseyLayerStack.Locked16") : FOdysseyStyle::GetBrush("OdysseyLayerStack.Unlocked16");
}

FReply OdysseyFolderLayerNode::OnToggleLocked()
{
    GetLayerDataPtr()->SetIsLocked( !GetLayerDataPtr()->IsLocked() );
    mExpanded = false;
    return FReply::Handled();
}

void OdysseyFolderLayerNode::RefreshOpacityText() const
{
    if( mOpacityText )
    {
        FOdysseyFolderLayer* layer = static_cast<FOdysseyFolderLayer*>( GetLayerDataPtr() );

        mOpacityText->DetachWidget();
        mOpacityText->AttachWidget( SNew(STextBlock).Text( FText::AsPercent( layer->GetOpacity() ) ) );
    }
}

void OdysseyFolderLayerNode::RefreshBlendingModeText() const
{
    if( mBlendingModeText )
    {
        FOdysseyFolderLayer* layer = static_cast<FOdysseyFolderLayer*>( GetLayerDataPtr() );

        mBlendingModeText->DetachWidget();
        mBlendingModeText->AttachWidget( SNew(STextBlock).Text( layer->GetBlendingModeAsText() ) );
    }
}
//---------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
