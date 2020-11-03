// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "LayerStack/FOdysseyLayerStackNodeDragDropOp.h"

#include "LayerStack/LayersGUI/OdysseyBaseLayerNode.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SBoxPanel.h"
#include "EditorStyleSet.h"


#define LOCTEXT_NAMESPACE "OdysseyLayerStackNodeDragDropOp"


//STATIC -------------------------------------------


TSharedRef<FOdysseyLayerStackNodeDragDropOp> FOdysseyLayerStackNodeDragDropOp::New(TArray<TSharedRef<IOdysseyBaseLayerNode>>& iDraggedNodes, FText iDefaultText, const FSlateBrush* iDefaultIcon)
{
    TSharedRef<FOdysseyLayerStackNodeDragDropOp> newOp = MakeShareable(new FOdysseyLayerStackNodeDragDropOp);

    newOp->mDraggedNodes = iDraggedNodes;
    newOp->mDefaultHoverText = newOp->mCurrentHoverText = iDefaultText;
    newOp->mDefaultHoverIcon = newOp->mCurrentIconBrush = iDefaultIcon;

    newOp->Construct();
    return newOp;
}

//INTERFACE IMPLEMENTATION-----------------------

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

FText FOdysseyLayerStackNodeDragDropOp::GetDecoratorText() const
{
    return mCurrentHoverText;
}

const FSlateBrush* FOdysseyLayerStackNodeDragDropOp::GetDecoratorIcon() const
{
    return mCurrentIconBrush;
}

TArray<TSharedRef<IOdysseyBaseLayerNode>>& FOdysseyLayerStackNodeDragDropOp::GetDraggedNodes()
{
    return mDraggedNodes;
}

const FSlateBrush* FOdysseyLayerStackNodeDragDropOp::GetCurrentIconBrush() const
{
    return mCurrentIconBrush;
}


void FOdysseyLayerStackNodeDragDropOp::SetCurrentIconBrush( const FSlateBrush* iNewIconBrush )
{
    mCurrentIconBrush = iNewIconBrush;
}

void FOdysseyLayerStackNodeDragDropOp::ResetToDefaultToolTip()
{
    mCurrentHoverText = mDefaultHoverText;
    mCurrentIconBrush = mDefaultHoverIcon;
}


//-----------------------------------------------



#undef LOCTEXT_NAMESPACE
