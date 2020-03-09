// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/LayersGUI/OdysseyImageLayerNode.h"

#include "EditorStyleSet.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/FOdysseyLayerStackNodeDragDropOp.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SSpinBox.h"

#define LOCTEXT_NAMESPACE "FOdysseyImageLayerNode"


//CONSTRUCTION/DESTRUCTION --------------------------------------

FOdysseyImageLayerNode::FOdysseyImageLayerNode( FOdysseyImageLayer& iImageLayer, FOdysseyLayerStackTree& iParentTree )
    : IOdysseyBaseLayerNode( iImageLayer.GetName(), iParentTree, &iImageLayer )
{
}

// IOdysseyBaseLayerNode IMPLEMENTATION---------------------------

float FOdysseyImageLayerNode::GetNodeHeight() const
{
    return 20.0f;
}

FNodePadding FOdysseyImageLayerNode::GetNodePadding() const
{
    TArray< IOdysseyLayer* > layersData = TArray<IOdysseyLayer*>();
    mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->DepthFirstSearchTree( &layersData, false );
    
    float leftPadding = (mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->FindNode( mLayerDataPtr )->GetNumberParents() - 1) * 10;
        
    return FNodePadding(leftPadding, 4, 4);
}

TOptional<EItemDropZone> FOdysseyImageLayerNode::CanDrop(FOdysseyLayerStackNodeDragDropOp& iDragDropOp, EItemDropZone iItemDropZone) const
{
    iDragDropOp.ResetToDefaultToolTip();

    return TOptional<EItemDropZone>( iItemDropZone );
}

void FOdysseyImageLayerNode::Drop(const TArray<TSharedRef<IOdysseyBaseLayerNode>>& iDraggedNodes, EItemDropZone iItemDropZone)
{
    TSharedPtr<IOdysseyBaseLayerNode> currentNode = SharedThis((IOdysseyBaseLayerNode*)this);

    for( TSharedRef<IOdysseyBaseLayerNode> draggedNode: iDraggedNodes)
    {
        draggedNode->MoveNodeTo( iItemDropZone, currentNode.ToSharedRef() );
    }
}

const FSlateBrush* FOdysseyImageLayerNode::GetIconBrush() const
{
    return FOdysseyStyle::GetBrush( "OdysseyLayerStack.ImageLayer16");
}

TSharedRef<SWidget> FOdysseyImageLayerNode::GenerateContainerWidgetForPropertyView()
{
    return SNew(SOdysseyImageLayerNodePropertyView, SharedThis(this) );
}

TSharedRef<SWidget> FOdysseyImageLayerNode::GetCustomIconContent()
{
    return SNew(SImage)
            .Image(GetIconBrush())
            .ColorAndOpacity(GetIconColor());
}

TSharedRef<SWidget> FOdysseyImageLayerNode::GetCustomOutlinerContent()
{
    FOdysseyImageLayer* layer = static_cast<FOdysseyImageLayer*>( GetLayerDataPtr() );

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
                .OnClicked( this, &FOdysseyImageLayerNode::OnToggleVisibility )
                .ToolTipText( LOCTEXT("OdysseyLayerVisibilityButtonToolTip", "Toggle Layer Visibility") )
                .ForegroundColor( FSlateColor::UseForeground() )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .Content()
                [
                    SNew(SImage)
                    .Image(this, &FOdysseyImageLayerNode::GetVisibilityBrushForLayer)
                ]
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
                .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                .OnClicked( this, &FOdysseyImageLayerNode::OnToggleLocked )
                .ToolTipText( LOCTEXT("OdysseyLayerLockedButtonToolTip", "Toggle Layer Locked State") )
                .ForegroundColor( FSlateColor::UseForeground() )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .Content()
                [
                    SNew(SImage)
                    .Image(this, &FOdysseyImageLayerNode::GetLockedBrushForLayer)
                ]
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
                .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                .OnClicked( this, &FOdysseyImageLayerNode::OnToggleAlphaLocked )
                .ToolTipText( LOCTEXT("OdysseyLayerLockedButtonToolTip", "Toggle Layer Alpha Locked State") )
                .ForegroundColor( FSlateColor::UseForeground() )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .Content()
                [
                    SNew(SImage)
                    .Image(this, &FOdysseyImageLayerNode::GetAlphaLockedBrushForLayer)
                ]
        ];
}


void FOdysseyImageLayerNode::BuildContextMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.BeginSection("Edit", LOCTEXT("EditContextMenuSectionName", "Edit"));
    {
            iMenuBuilder.AddMenuEntry(
            LOCTEXT("DeleteLayer", "Delete"),
            LOCTEXT("DeleteLayerTooltip", "Delete this Layer"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetActions.Delete"),
                                     FUIAction(FExecuteAction::CreateSP(&(mParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnDeleteLayer, mLayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &FOdysseyImageLayerNode::HandleDeleteLayerCanExecute)));

            iMenuBuilder.AddMenuEntry(
            LOCTEXT("MergeDownLayer", "Merge Down"),
            LOCTEXT("MergeDownLayerTooltip", "Merge this Layer Down"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "MergeDownIcon"),
                                     FUIAction(FExecuteAction::CreateSP(&(mParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnMergeLayerDown, mLayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &FOdysseyImageLayerNode::HandleMergeLayerDownCanExecute)));
        
            iMenuBuilder.AddMenuEntry(
            LOCTEXT("DuplicateLayer", "Duplicate Layer"),
            LOCTEXT("DuplicateLayerTooltip", "Duplicate this Layer"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "DuplicateLayerIcon"),
                                     FUIAction(FExecuteAction::CreateSP(&(mParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnDuplicateLayer, mLayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &FOdysseyImageLayerNode::HandleDuplicateLayerCanExecute)));
    }
}

bool FOdysseyImageLayerNode::IsHidden() const
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

bool FOdysseyImageLayerNode::HandleDeleteLayerCanExecute() const
{
    return true;
}

bool FOdysseyImageLayerNode::HandleMergeLayerDownCanExecute() const
{
    //We can only merge down to another image layer
    
    int currentIndex = mParentTree.GetLayerStack().GetLayerStackData()->GetCurrentLayerAsIndex();
    
    if( currentIndex == (mParentTree.GetRootNodes().Num() - 1) )
        return false;
        
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->DepthFirstSearchTree( &layers, false );
    
    return (layers[currentIndex + 1]->GetType() == IOdysseyLayer::eType::kImage);
    
}

bool FOdysseyImageLayerNode::HandleDuplicateLayerCanExecute() const
{
    return true;
}


//--------------------------------------------------PROTECTED API


const FSlateBrush* FOdysseyImageLayerNode::GetVisibilityBrushForLayer() const
{
    return GetLayerDataPtr()->IsVisible() ? FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16") : FOdysseyStyle::GetBrush("OdysseyLayerStack.NotVisible16");
}

FReply FOdysseyImageLayerNode::OnToggleVisibility()
{
    GetLayerDataPtr()->SetIsVisible( !GetLayerDataPtr()->IsVisible() );
    GetLayerStack().GetLayerStackData()->ComputeResultBlock();
    return FReply::Handled();
}

const FSlateBrush* FOdysseyImageLayerNode::GetLockedBrushForLayer() const
{
    return GetLayerDataPtr()->IsLocked() ? FOdysseyStyle::GetBrush("OdysseyLayerStack.Locked16") : FOdysseyStyle::GetBrush("OdysseyLayerStack.Unlocked16");
}

FReply FOdysseyImageLayerNode::OnToggleLocked()
{
    GetLayerDataPtr()->SetIsLocked( !GetLayerDataPtr()->IsLocked() );
    return FReply::Handled();
}

const FSlateBrush* FOdysseyImageLayerNode::GetAlphaLockedBrushForLayer() const
{
    FOdysseyImageLayer* layer = static_cast<FOdysseyImageLayer*> (GetLayerDataPtr());
    
    return layer->IsAlphaLocked() ? FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaLocked16") : FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaUnlocked16");
}

FReply FOdysseyImageLayerNode::OnToggleAlphaLocked()
{
    FOdysseyImageLayer* layer = static_cast<FOdysseyImageLayer*> (GetLayerDataPtr());

    layer->SetIsAlphaLocked( !layer->IsAlphaLocked() );
    return FReply::Handled();
}

void FOdysseyImageLayerNode::RefreshOpacityText() const
{
    if( mOpacityText )
    {
        FOdysseyImageLayer* layer = static_cast<FOdysseyImageLayer*>( GetLayerDataPtr() );

        mOpacityText->DetachWidget();
        mOpacityText->AttachWidget( SNew(STextBlock).Text( FText::AsPercent( layer->GetOpacity() ) ) );
    }
}

void FOdysseyImageLayerNode::RefreshBlendingModeText() const
{
    if( mBlendingModeText )
    {
        FOdysseyImageLayer* layer = static_cast<FOdysseyImageLayer*>( GetLayerDataPtr() );

        mBlendingModeText->DetachWidget();
        mBlendingModeText->AttachWidget( SNew(STextBlock).Text( layer->GetBlendingModeAsText() ) );
    }
}

//---------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
