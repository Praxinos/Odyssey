// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/FOdysseyLayerStackNodeDragDropOp.h"

#include "LayerStack/LayersGUI/OdysseyBaseLayerNode.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SBoxPanel.h"
#include "EditorStyleSet.h"


#define LOCTEXT_NAMESPACE "OdysseyLayerStackNodeDragDropOp"


//STATIC -------------------------------------------


TSharedRef<FOdysseyLayerStackNodeDragDropOp> FOdysseyLayerStackNodeDragDropOp::New(TArray<TSharedRef<OdysseyBaseLayerNode>>& InDraggedNodes, FText InDefaultText, const FSlateBrush* InDefaultIcon)
{
    TSharedRef<FOdysseyLayerStackNodeDragDropOp> NewOp = MakeShareable(new FOdysseyLayerStackNodeDragDropOp);

    NewOp->DraggedNodes = InDraggedNodes;
    NewOp->DefaultHoverText = NewOp->CurrentHoverText = InDefaultText;
    NewOp->DefaultHoverIcon = NewOp->CurrentIconBrush = InDefaultIcon;

    NewOp->Construct();
    return NewOp;
}

//INTERFACE IMPLEMENTATION-----------------------

    //~ FGraphEditorDragDropAction interface
void FOdysseyLayerStackNodeDragDropOp::HoverTargetChanged()
{

}

FReply FOdysseyLayerStackNodeDragDropOp::DroppedOnPanel( const TSharedRef< class SWidget >& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph)
{
    return FReply::Unhandled();
}


void FOdysseyLayerStackNodeDragDropOp::Construct()
{
    FGraphEditorDragDropAction::Construct();

    SetFeedbackMessage(
        SNew(SBorder)
        .BorderImage(FEditorStyle::GetBrush("Graph.ConnectorFeedback.Border"))
        .Content()
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.0f, 0.0f, 3.0f, 0.0f)
            .VAlign(VAlign_Center)
            [
                SNew(SImage)
                .Image(this, &FOdysseyLayerStackNodeDragDropOp::GetDecoratorIcon)
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(this, &FOdysseyLayerStackNodeDragDropOp::GetDecoratorText)
            ]
        ]
    );
}

//PUBLIC API-------------------------------------

TArray<TSharedRef<OdysseyBaseLayerNode>>& FOdysseyLayerStackNodeDragDropOp::GetDraggedNodes()
{
    return DraggedNodes;
}

void FOdysseyLayerStackNodeDragDropOp::ResetToDefaultToolTip()
{
    CurrentHoverText = DefaultHoverText;
    CurrentIconBrush = DefaultHoverIcon;
}

//-----------------------------------------------



#undef LOCTEXT_NAMESPACE
