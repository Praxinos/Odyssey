// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "LayerStack/LayersGUI/OdysseyTrackLayerNode.h"

#include "EditorStyleSet.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/FOdysseyLayerStackNodeDragDropOp.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SSpinBox.h"

#define LOCTEXT_NAMESPACE "OdysseyTrackLayerNode"


//CONSTRUCTION/DESTRUCTION --------------------------------------

OdysseyTrackLayerNode::OdysseyTrackLayerNode( FOdysseyImageLayer& InImageLayer, TSharedPtr<OdysseyBaseLayerNode> InParentNode, FOdysseyLayerStackTree& InParentTree )
    : OdysseyBaseLayerNode( InImageLayer.GetName(), InParentNode, InParentTree, &InImageLayer )
{
}

// ODYSSEYBASELAYERNODE IMPLEMENTATION---------------------------

float OdysseyTrackLayerNode::GetNodeHeight() const
{
    return 20.0f;
}

FNodePadding OdysseyTrackLayerNode::GetNodePadding() const
{
    return FNodePadding(4, 4);
}

bool OdysseyTrackLayerNode::CanRenameNode() const
{
    return true;
}

TOptional<EItemDropZone> OdysseyTrackLayerNode::CanDrop(FOdysseyLayerStackNodeDragDropOp& DragDropOp, EItemDropZone ItemDropZone) const
{
    DragDropOp.ResetToDefaultToolTip();

    return TOptional<EItemDropZone>( ItemDropZone );
}

void OdysseyTrackLayerNode::Drop(const TArray<TSharedRef<OdysseyBaseLayerNode>>& DraggedNodes, EItemDropZone ItemDropZone)
{
    TSharedPtr<OdysseyBaseLayerNode> CurrentNode = SharedThis((OdysseyBaseLayerNode*)this);

    for( TSharedRef<OdysseyBaseLayerNode> DraggedNode: DraggedNodes)
    {
        DraggedNode->MoveNodeTo( ItemDropZone, CurrentNode.ToSharedRef() );
    }
}

const FSlateBrush* OdysseyTrackLayerNode::GetIconBrush() const
{
    return FOdysseyStyle::Get().GetBrush( "OdysseyLayerStack.ImageLayer16");
}

TSharedRef<SWidget> OdysseyTrackLayerNode::GetCustomOutlinerContent()
{
    FOdysseyImageLayer* layer = static_cast<FOdysseyImageLayer*>( GetLayerDataPtr() );

    return SNew(SHorizontalBox)

        +SHorizontalBox::Slot()
        .HAlign( HAlign_Left )
        .VAlign( VAlign_Center )
        .Expose( BlendingModeText )
        [
            SNew(STextBlock).Text( layer->GetBlendingModeAsText() )
        ]

        +SHorizontalBox::Slot()
        .HAlign( HAlign_Left )
        .VAlign( VAlign_Center )
        .Expose( OpacityText )
        [
            SNew(STextBlock).Text( FText::AsPercent( layer->GetOpacity() ) )
        ]

        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
                .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                .OnClicked( this, &OdysseyTrackLayerNode::OnToggleVisibility )
                .ToolTipText( LOCTEXT("OdysseyLayerVisibilityButtonToolTip", "Toggle Layer Visibility") )
                .ForegroundColor( FSlateColor::UseForeground() )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .Content()
                [
                    SNew(SImage)
                    .Image(this, &OdysseyTrackLayerNode::GetVisibilityBrushForLayer)
                ]
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
                .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                .OnClicked( this, &OdysseyTrackLayerNode::OnToggleLocked )
                .ToolTipText( LOCTEXT("OdysseyLayerLockedButtonToolTip", "Toggle Layer Locked State") )
                .ForegroundColor( FSlateColor::UseForeground() )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .Content()
                [
                    SNew(SImage)
                    .Image(this, &OdysseyTrackLayerNode::GetLockedBrushForLayer)
                ]
        ];
}


//--------------------------------------------------PROTECTED API


const FSlateBrush* OdysseyTrackLayerNode::GetVisibilityBrushForLayer() const
{
    return GetLayerDataPtr()->IsVisible() ? FOdysseyStyle::Get().GetBrush("OdysseyLayerStack.Visible16") : FOdysseyStyle::Get().GetBrush("OdysseyLayerStack.NotVisible16");
}

FReply OdysseyTrackLayerNode::OnToggleVisibility()
{
    GetLayerDataPtr()->SetIsVisible( !GetLayerDataPtr()->IsVisible() );
    GetLayerStack().GetLayerStackData()->ComputeResultBlock();
    return FReply::Handled();
}

const FSlateBrush* OdysseyTrackLayerNode::GetLockedBrushForLayer() const
{
    return GetLayerDataPtr()->IsLocked() ? FOdysseyStyle::Get().GetBrush("OdysseyLayerStack.Locked16") : FOdysseyStyle::Get().GetBrush("OdysseyLayerStack.Unlocked16");
}

FReply OdysseyTrackLayerNode::OnToggleLocked()
{
    GetLayerDataPtr()->SetIsLocked( !GetLayerDataPtr()->IsLocked() );
    return FReply::Handled();
}

void OdysseyTrackLayerNode::RefreshOpacityText() const
{
    if( OpacityText )
    {
        FOdysseyImageLayer* layer = static_cast<FOdysseyImageLayer*>( GetLayerDataPtr() );

        OpacityText->DetachWidget();
        OpacityText->AttachWidget( SNew(STextBlock).Text( FText::AsPercent( layer->GetOpacity() ) ) );
    }
}

void OdysseyTrackLayerNode::RefreshBlendingModeText() const
{
    if( BlendingModeText )
    {
        FOdysseyImageLayer* layer = static_cast<FOdysseyImageLayer*>( GetLayerDataPtr() );

        BlendingModeText->DetachWidget();
        BlendingModeText->AttachWidget( SNew(STextBlock).Text( layer->GetBlendingModeAsText() ) );
    }
}

//---------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
