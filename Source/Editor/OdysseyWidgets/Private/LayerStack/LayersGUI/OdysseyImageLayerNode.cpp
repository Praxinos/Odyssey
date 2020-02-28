// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/LayersGUI/OdysseyImageLayerNode.h"

#include "EditorStyleSet.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/FOdysseyLayerStackNodeDragDropOp.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SSpinBox.h"

#define LOCTEXT_NAMESPACE "OdysseyImageLayerNode"


//CONSTRUCTION/DESTRUCTION --------------------------------------

OdysseyImageLayerNode::OdysseyImageLayerNode( FOdysseyImageLayer& InImageLayer, TSharedPtr<OdysseyBaseLayerNode> InParentNode, FOdysseyLayerStackTree& InParentTree )
    : OdysseyBaseLayerNode( InImageLayer.GetName()
    , InParentNode
    , InParentTree
    , &InImageLayer )
{
}

// ODYSSEYBASELAYERNODE IMPLEMENTATION---------------------------

float OdysseyImageLayerNode::GetNodeHeight() const
{
    return 20.0f;
}

FNodePadding OdysseyImageLayerNode::GetNodePadding() const
{
    TArray< IOdysseyLayer* > layersData = TArray<IOdysseyLayer*>();
    mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->DepthFirstSearchTree( &layersData, false );
    
    float leftPadding = (mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->FindNode( mLayerDataPtr )->GetNumberParents() - 1) * 10;
        
    return FNodePadding(leftPadding, 4, 4);
}

TOptional<EItemDropZone> OdysseyImageLayerNode::CanDrop(FOdysseyLayerStackNodeDragDropOp& DragDropOp, EItemDropZone ItemDropZone) const
{
    DragDropOp.ResetToDefaultToolTip();

    return TOptional<EItemDropZone>( ItemDropZone );
}

void OdysseyImageLayerNode::Drop(const TArray<TSharedRef<OdysseyBaseLayerNode>>& DraggedNodes, EItemDropZone ItemDropZone)
{
    TSharedPtr<OdysseyBaseLayerNode> CurrentNode = SharedThis((OdysseyBaseLayerNode*)this);

    for( TSharedRef<OdysseyBaseLayerNode> DraggedNode: DraggedNodes)
    {
        DraggedNode->MoveNodeTo( ItemDropZone, CurrentNode.ToSharedRef() );
    }
}

const FSlateBrush* OdysseyImageLayerNode::GetIconBrush() const
{
    return FOdysseyStyle::GetBrush( "OdysseyLayerStack.ImageLayer16");
}

TSharedRef<SWidget> OdysseyImageLayerNode::GenerateContainerWidgetForPropertyView()
{
    return SNew(SOdysseyImageLayerNodePropertyView, SharedThis(this) );
}

TSharedRef<SWidget> OdysseyImageLayerNode::GetCustomIconContent()
{
    return SNew(SImage)
            .Image(GetIconBrush())
            .ColorAndOpacity(GetIconColor());
}

TSharedRef<SWidget> OdysseyImageLayerNode::GetCustomOutlinerContent()
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
                .OnClicked( this, &OdysseyImageLayerNode::OnToggleVisibility )
                .ToolTipText( LOCTEXT("OdysseyLayerVisibilityButtonToolTip", "Toggle Layer Visibility") )
                .ForegroundColor( FSlateColor::UseForeground() )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .Content()
                [
                    SNew(SImage)
                    .Image(this, &OdysseyImageLayerNode::GetVisibilityBrushForLayer)
                ]
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
                .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                .OnClicked( this, &OdysseyImageLayerNode::OnToggleLocked )
                .ToolTipText( LOCTEXT("OdysseyLayerLockedButtonToolTip", "Toggle Layer Locked State") )
                .ForegroundColor( FSlateColor::UseForeground() )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .Content()
                [
                    SNew(SImage)
                    .Image(this, &OdysseyImageLayerNode::GetLockedBrushForLayer)
                ]
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
                .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                .OnClicked( this, &OdysseyImageLayerNode::OnToggleAlphaLocked )
                .ToolTipText( LOCTEXT("OdysseyLayerLockedButtonToolTip", "Toggle Layer Alpha Locked State") )
                .ForegroundColor( FSlateColor::UseForeground() )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .Content()
                [
                    SNew(SImage)
                    .Image(this, &OdysseyImageLayerNode::GetAlphaLockedBrushForLayer)
                ]
        ];
}


void OdysseyImageLayerNode::BuildContextMenu(FMenuBuilder& MenuBuilder)
{
    MenuBuilder.BeginSection("Edit", LOCTEXT("EditContextMenuSectionName", "Edit"));
    {
            MenuBuilder.AddMenuEntry(
            LOCTEXT("DeleteLayer", "Delete"),
            LOCTEXT("DeleteLayerTooltip", "Delete this Layer"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetActions.Delete"),
                                     FUIAction(FExecuteAction::CreateSP(&(mParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnDeleteLayer, mLayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &OdysseyImageLayerNode::HandleDeleteLayerCanExecute)));

            MenuBuilder.AddMenuEntry(
            LOCTEXT("MergeDownLayer", "Merge Down"),
            LOCTEXT("MergeDownLayerTooltip", "Merge this Layer Down"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "MergeDownIcon"),
                                     FUIAction(FExecuteAction::CreateSP(&(mParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnMergeLayerDown, mLayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &OdysseyImageLayerNode::HandleMergeLayerDownCanExecute)));
        
            MenuBuilder.AddMenuEntry(
            LOCTEXT("DuplicateLayer", "Duplicate Layer"),
            LOCTEXT("DuplicateLayerTooltip", "Duplicate this Layer"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "DuplicateLayerIcon"),
                                     FUIAction(FExecuteAction::CreateSP(&(mParentTree.GetLayerStack()), &FOdysseyLayerStackModel::OnDuplicateLayer, mLayerDataPtr),
                                     FCanExecuteAction::CreateSP(this, &OdysseyImageLayerNode::HandleDuplicateLayerCanExecute)));
    }
}

bool OdysseyImageLayerNode::IsHidden() const
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

bool OdysseyImageLayerNode::HandleDeleteLayerCanExecute() const
{
    return true;
}

bool OdysseyImageLayerNode::HandleMergeLayerDownCanExecute() const
{
    //We can only merge down to another image layer
    
    int currentIndex = mParentTree.GetLayerStack().GetLayerStackData()->GetCurrentLayerAsIndex();
    
    if( currentIndex == (mParentTree.GetRootNodes().Num() - 1) )
        return false;
        
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mParentTree.GetLayerStack().GetLayerStackData()->GetLayers()->DepthFirstSearchTree( &layers, false );
    
    return (layers[currentIndex + 1]->GetType() == IOdysseyLayer::eType::kImage);
    
}

bool OdysseyImageLayerNode::HandleDuplicateLayerCanExecute() const
{
    return true;
}


//--------------------------------------------------PROTECTED API


const FSlateBrush* OdysseyImageLayerNode::GetVisibilityBrushForLayer() const
{
    return GetLayerDataPtr()->IsVisible() ? FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16") : FOdysseyStyle::GetBrush("OdysseyLayerStack.NotVisible16");
}

FReply OdysseyImageLayerNode::OnToggleVisibility()
{
    GetLayerDataPtr()->SetIsVisible( !GetLayerDataPtr()->IsVisible() );
    GetLayerStack().GetLayerStackData()->ComputeResultBlock();
    return FReply::Handled();
}

const FSlateBrush* OdysseyImageLayerNode::GetLockedBrushForLayer() const
{
    return GetLayerDataPtr()->IsLocked() ? FOdysseyStyle::GetBrush("OdysseyLayerStack.Locked16") : FOdysseyStyle::GetBrush("OdysseyLayerStack.Unlocked16");
}

FReply OdysseyImageLayerNode::OnToggleLocked()
{
    GetLayerDataPtr()->SetIsLocked( !GetLayerDataPtr()->IsLocked() );
    return FReply::Handled();
}

const FSlateBrush* OdysseyImageLayerNode::GetAlphaLockedBrushForLayer() const
{
    FOdysseyImageLayer* layer = static_cast<FOdysseyImageLayer*> (GetLayerDataPtr());
    
    return layer->IsAlphaLocked() ? FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaLocked16") : FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaUnlocked16");
}

FReply OdysseyImageLayerNode::OnToggleAlphaLocked()
{
    FOdysseyImageLayer* layer = static_cast<FOdysseyImageLayer*> (GetLayerDataPtr());

    layer->SetIsAlphaLocked( !layer->IsAlphaLocked() );
    return FReply::Handled();
}

void OdysseyImageLayerNode::RefreshOpacityText() const
{
    if( mOpacityText )
    {
        FOdysseyImageLayer* layer = static_cast<FOdysseyImageLayer*>( GetLayerDataPtr() );

        mOpacityText->DetachWidget();
        mOpacityText->AttachWidget( SNew(STextBlock).Text( FText::AsPercent( layer->GetOpacity() ) ) );
    }
}

void OdysseyImageLayerNode::RefreshBlendingModeText() const
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
