// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/LayersGUI/OdysseyFolderLayerNode.h"

#include "EditorStyleSet.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "FOdysseyFolderLayerNode"


//CONSTRUCTION/DESTRUCTION --------------------------------------

FOdysseyFolderLayerNode::FOdysseyFolderLayerNode( FOdysseyFolderLayer& iFolderLayer, FOdysseyLayerStackTree& iParentTree )
    : IOdysseyBaseLayerNode( iFolderLayer.GetName(), iParentTree, &iFolderLayer )
{
    mFolderOpenBrush = FEditorStyle::GetBrush( "ContentBrowser.AssetTreeFolderOpen" );
    mFolderClosedBrush = FEditorStyle::GetBrush( "ContentBrowser.AssetTreeFolderClosed" );
}

// IOdysseyBaseLayerNode IMPLEMENTATION---------------------------

float FOdysseyFolderLayerNode::GetNodeHeight() const
{
    return 20.0f;
}

FNodePadding FOdysseyFolderLayerNode::GetNodePadding() const
{
    TArray< IOdysseyLayer* > layersData = TArray<IOdysseyLayer*>();
    mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->DepthFirstSearchTree( &layersData, false );
    
    float leftPadding = (mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->FindNode( mLayerDataPtr )->GetNumberParents() - 1) * 10;
        
    return FNodePadding(leftPadding, 4, 4);
}

TOptional<EItemDropZone> FOdysseyFolderLayerNode::CanDrop(FOdysseyLayerStackNodeDragDropOp& iDragDropOp, EItemDropZone iItemDropZone) const
{
    iDragDropOp.ResetToDefaultToolTip();

    return TOptional<EItemDropZone>( iItemDropZone );
}

void FOdysseyFolderLayerNode::Drop(const TArray<TSharedRef<IOdysseyBaseLayerNode>>& iDraggedNodes, EItemDropZone iItemDropZone)
{
    TSharedPtr<IOdysseyBaseLayerNode> CurrentNode = SharedThis((IOdysseyBaseLayerNode*)this);

    for( TSharedRef<IOdysseyBaseLayerNode> draggedNode: iDraggedNodes)
    {
        draggedNode->MoveNodeTo( iItemDropZone, CurrentNode.ToSharedRef() );
    }
}

const FSlateBrush* FOdysseyFolderLayerNode::GetIconBrush() const
{
    FOdysseyFolderLayer* layer = static_cast<FOdysseyFolderLayer*>( GetLayerDataPtr() );

    if( layer->IsOpen() )
        return mFolderOpenBrush;
    else
        return mFolderClosedBrush;
}

TSharedRef<SWidget> FOdysseyFolderLayerNode::GenerateContainerWidgetForPropertyView()
{
    return SNew(SOdysseyFolderLayerNodePropertyView, SharedThis(this) );
}

TSharedRef<SWidget> FOdysseyFolderLayerNode::GetCustomIconContent()
{
    return SNew(SButton)
           .ButtonStyle( &FOdysseyStyle::GetWidgetStyle<FButtonStyle>("OdysseyLayerStack.Motionless") )
           .VAlign(VAlign_Center)
           .HAlign(HAlign_Center)
           .OnClicked(this, &FOdysseyFolderLayerNode::HandleExpand)
           .ContentPadding(0.f)
           .ForegroundColor(FSlateColor::UseForeground())
           .IsFocusable(false)
           [
               SNew(SImage)
               .Image(this, &FOdysseyFolderLayerNode::GetIconBrush)
               .ColorAndOpacity(FSlateColor::UseForeground())
           ];
}

TSharedRef<SWidget> FOdysseyFolderLayerNode::GetCustomOutlinerContent()
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
                .OnClicked( this, &FOdysseyFolderLayerNode::OnToggleVisibility )
                .ToolTipText( LOCTEXT("OdysseyLayerVisibilityButtonToolTip", "Toggle Layer Visibility") )
                .ForegroundColor( FSlateColor::UseForeground() )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .Content()
                [
                    SNew(SImage)
                    .Image(this, &FOdysseyFolderLayerNode::GetVisibilityBrushForLayer)
                ]
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
                .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                .OnClicked( this, &FOdysseyFolderLayerNode::OnToggleLocked )
                .ToolTipText( LOCTEXT("OdysseyLayerLockedButtonToolTip", "Toggle Layer Locked State") )
                .ForegroundColor( FSlateColor::UseForeground() )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .Content()
                [
                    SNew(SImage)
                    .Image(this, &FOdysseyFolderLayerNode::GetLockedBrushForLayer)
                ]
         ];
}

void FOdysseyFolderLayerNode::BuildContextMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.BeginSection("Edit", LOCTEXT("EditContextMenuSectionName", "Edit"));
    {
            iMenuBuilder.AddMenuEntry(
            LOCTEXT("DeleteLayer", "Delete"),
            LOCTEXT("DeleteLayerTooltip", "Delete this Layer"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetActions.Delete"),
                                     FUIAction(FExecuteAction::CreateSP(&(mParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnDeleteLayer, mLayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &FOdysseyFolderLayerNode::HandleDeleteLayerCanExecute)));

            iMenuBuilder.AddMenuEntry(
            LOCTEXT("FlattenLayer", "Flatten Layer"),
            LOCTEXT("FlattenTooltip", "Flatten the folder in one image layer"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "Flatten"),
                                     FUIAction(FExecuteAction::CreateSP(&(mParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnFlattenLayer, mLayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &FOdysseyFolderLayerNode::HandleFlattenLayerCanExecute)));
        
            iMenuBuilder.AddMenuEntry(
            LOCTEXT("DuplicateLayer", "Duplicate Layer"),
            LOCTEXT("DuplicateLayerTooltip", "Duplicate this Layer"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "DuplicateLayerIcon"),
                                     FUIAction(FExecuteAction::CreateSP(&(mParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnDuplicateLayer, mLayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &FOdysseyFolderLayerNode::HandleDuplicateLayerCanExecute)));
    }
}


bool FOdysseyFolderLayerNode::IsHidden() const
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

bool FOdysseyFolderLayerNode::HandleDeleteLayerCanExecute() const
{
    return true;
}

bool FOdysseyFolderLayerNode::HandleFlattenLayerCanExecute() const
{
    //We won't be able to flatten it everytime when new layer types will be created
    return true;
}

bool FOdysseyFolderLayerNode::HandleDuplicateLayerCanExecute() const
{
    return true;
}

FReply FOdysseyFolderLayerNode::HandleExpand()
{
    FOdysseyFolderLayer* layer = static_cast<FOdysseyFolderLayer*>( GetLayerDataPtr() );
    layer->SetIsOpen( !layer->IsOpen() );
    
    mParentTree.GetLayerStack().GetLayerStackView()->RefreshView();
    
    return FReply::Handled();
}

//--------------------------------------------------PROTECTED API


const FSlateBrush* FOdysseyFolderLayerNode::GetVisibilityBrushForLayer() const
{
    return GetLayerDataPtr()->IsVisible() ? FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16") : FOdysseyStyle::GetBrush("OdysseyLayerStack.NotVisible16");
}

FReply FOdysseyFolderLayerNode::OnToggleVisibility()
{
    GetLayerDataPtr()->SetIsVisible( !GetLayerDataPtr()->IsVisible() );
    GetLayerStack().GetLayerStackData()->ComputeResultBlock();
    return FReply::Handled();
}

const FSlateBrush* FOdysseyFolderLayerNode::GetLockedBrushForLayer() const
{
    return GetLayerDataPtr()->IsLocked() ? FOdysseyStyle::GetBrush("OdysseyLayerStack.Locked16") : FOdysseyStyle::GetBrush("OdysseyLayerStack.Unlocked16");
}

FReply FOdysseyFolderLayerNode::OnToggleLocked()
{
    GetLayerDataPtr()->SetIsLocked( !GetLayerDataPtr()->IsLocked() );
    return FReply::Handled();
}

void FOdysseyFolderLayerNode::RefreshOpacityText() const
{
    if( mOpacityText )
    {
        FOdysseyFolderLayer* layer = static_cast<FOdysseyFolderLayer*>( GetLayerDataPtr() );

        mOpacityText->DetachWidget();
        mOpacityText->AttachWidget( SNew(STextBlock).Text( FText::AsPercent( layer->GetOpacity() ) ) );
    }
}

void FOdysseyFolderLayerNode::RefreshBlendingModeText() const
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
