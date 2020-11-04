// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "LayerStack/LayersGUI/OdysseyImageLayerNode.h"

#include "EditorStyleSet.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/FOdysseyLayerStackNodeDragDropOp.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SSpinBox.h"

#include "OdysseyFolderLayer.h"

#define LOCTEXT_NAMESPACE "FOdysseyImageLayerNode"


//CONSTRUCTION/DESTRUCTION --------------------------------------

FOdysseyImageLayerNode::FOdysseyImageLayerNode( TSharedPtr<FOdysseyImageLayer> iImageLayer, FOdysseyLayerStackTree& iParentTree )
    : IOdysseyBaseLayerNode( iImageLayer->GetName(), iParentTree, iImageLayer )
{
}

// IOdysseyBaseLayerNode IMPLEMENTATION---------------------------

float FOdysseyImageLayerNode::GetNodeHeight() const
{
    return 20.0f;
}

FNodePadding FOdysseyImageLayerNode::GetNodePadding() const
{
	TSharedPtr<IOdysseyLayer> layer = mLayerDataPtr->GetParent();
	int parentCount = 0;
	while (layer)
	{
		parentCount++;
		layer = layer->GetParent();
	}

	float leftPadding = (parentCount - 1) * 10;

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

	//Refresh
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
    TSharedPtr<FOdysseyImageLayer> layer = StaticCastSharedPtr<FOdysseyImageLayer>( GetLayerDataPtr() );

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
                .ToolTipText( LOCTEXT("OdysseyLayerAlphaLockedButtonToolTip", "Toggle Layer Alpha Locked State") )
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
    TSharedPtr<IOdysseyLayer> layer = mLayerDataPtr->GetParent();

    while( layer )
    {
		if (layer->GetType() == IOdysseyLayer::eType::kFolder)
		{
			if (!StaticCastSharedPtr<FOdysseyFolderLayer>(layer)->IsOpen())
				return true;
		}
        
        layer = layer->GetParent();
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
	TSharedPtr<IOdysseyLayer> currentLayer = mParentTree.GetLayerStack().GetLayerStackData()->GetCurrentLayer();
	if (!currentLayer)
		return false;

	TSharedPtr<IOdysseyLayer> parent = currentLayer->GetParent();
	if (!parent)
		return false;

	int index = currentLayer->GetIndexInParent();
	if (index >= parent->GetNodes().Num() - 1)
		return false;

	return parent->GetNode(index + 1)->GetType() == IOdysseyLayer::eType::kImage;
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
    TSharedPtr<FOdysseyImageLayer> layer = StaticCastSharedPtr<FOdysseyImageLayer> (GetLayerDataPtr());
    
    return layer->IsAlphaLocked() ? FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaLocked16") : FOdysseyStyle::GetBrush("OdysseyLayerStack.AlphaUnlocked16");
}

FReply FOdysseyImageLayerNode::OnToggleAlphaLocked()
{
    TSharedPtr<FOdysseyImageLayer> layer = StaticCastSharedPtr<FOdysseyImageLayer> (GetLayerDataPtr());

    layer->SetIsAlphaLocked( !layer->IsAlphaLocked() );
    return FReply::Handled();
}

void FOdysseyImageLayerNode::RefreshOpacityText() const
{
    if( mOpacityText )
    {
        TSharedPtr<FOdysseyImageLayer> layer = StaticCastSharedPtr<FOdysseyImageLayer>( GetLayerDataPtr() );

        mOpacityText->DetachWidget();
        mOpacityText->AttachWidget( SNew(STextBlock).Text( FText::AsPercent( layer->GetOpacity() ) ) );
    }
}

void FOdysseyImageLayerNode::RefreshBlendingModeText() const
{
    if( mBlendingModeText )
    {
        TSharedPtr<FOdysseyImageLayer> layer = StaticCastSharedPtr<FOdysseyImageLayer>( GetLayerDataPtr() );

        mBlendingModeText->DetachWidget();
        mBlendingModeText->AttachWidget( SNew(STextBlock).Text( layer->GetBlendingModeAsText() ) );
    }
}

//---------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
