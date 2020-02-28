// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/SOdysseyLayerStackOutlinerTreeNode.h"

#include "LayerStack/SOdysseyLayerStackTreeView.h"
#include "LayerStack/LayersGUI/OdysseyBaseLayerNode.h"
#include "ScopedTransaction.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Views/SExpanderArrow.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Fonts/SlateFontInfo.h"
#include "Types/SlateStructs.h"
#include "EditorStyleSet.h"
#include "OdysseyStyleSet.h"
#include "Styling/SlateTypes.h"



#define LOCTEXT_NAMESPACE "OdysseyLayerStackOutlinerTreeNode"


//SODYSSEYLAYERSTACKOUTLINERTREENODE----------------------------

//CONSTRUCTION/DESTRUCTION--------------------------------------
SOdysseyLayerStackOutlinerTreeNode::~SOdysseyLayerStackOutlinerTreeNode()
{
    mLayerNode->OnRenameRequested().RemoveAll(this);
}



void SOdysseyLayerStackOutlinerTreeNode::Construct( const FArguments& InArgs, TSharedRef<OdysseyBaseLayerNode> Node, const TSharedRef<SOdysseyLayerStackViewRow>& InTableRow )
{
    mLayerNode = Node;
    mIsOuterTopLevelNode = !Node->GetParent().IsValid();

    auto NodeHeight = [=]() -> FOptionalSize { return Node->GetNodeHeight(); };

    FMargin InnerNodePadding;
    if ( mIsInnerTopLevelNode )
    {
        mInnerBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Expanded" );
        InnerNodePadding = FMargin(0.f, 1.f);
    }
    else
    {
        mInnerBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TransparentBorder" );
        InnerNodePadding = FMargin(0.f);
    }

    FSlateFontInfo NodeFont = FEditorStyle::GetFontStyle("Sequencer.AnimationOutliner.RegularFont");

    mEditableLabel = SNew( SInlineEditableTextBlock )
    .IsReadOnly(this, &SOdysseyLayerStackOutlinerTreeNode::HandleNodeLabelIsReadOnly )
    .Font(NodeFont)
    .ColorAndOpacity(this, &SOdysseyLayerStackOutlinerTreeNode::GetDisplayNameColor)
    .OnTextCommitted(this, &SOdysseyLayerStackOutlinerTreeNode::HandleNodeLabelTextChanged)
    .Text(this, &SOdysseyLayerStackOutlinerTreeNode::GetDisplayName)
    .ToolTipText(this, &SOdysseyLayerStackOutlinerTreeNode::GetDisplayNameToolTipText)
    .Clipping(EWidgetClipping::ClipToBounds)
	.IsSelected(FIsSelected::CreateSP(InTableRow, &SOdysseyLayerStackViewRow::IsSelectedExclusively));

    Node->OnRenameRequested().AddRaw( this, &SOdysseyLayerStackOutlinerTreeNode::EnterRenameMode );

    TSharedRef<SWidget>    FinalWidget =
        SNew( SBorder )
        .VAlign( VAlign_Center )
        .BorderImage( this, &SOdysseyLayerStackOutlinerTreeNode::GetNodeBorderImage )
        .BorderBackgroundColor( this, &SOdysseyLayerStackOutlinerTreeNode::GetNodeBackgroundTint )
        .Padding(FMargin(Node->GetNodePadding().mLeft, Node->GetNodePadding().mTop, 0, Node->GetNodePadding().mBottom ))
        [
            SNew( SHorizontalBox )

            + SHorizontalBox::Slot()
            [
                SNew(SBox)
                .HeightOverride_Lambda(NodeHeight)
                .Padding(FMargin(5.0f, 0.0f))
                [
                    SNew( SHorizontalBox )

                     + SHorizontalBox::Slot()
                        .Padding( InnerNodePadding )
                        [
                            SNew( SBorder )
                            .BorderImage( FEditorStyle::GetBrush( "LayerStack.NodeOutliner.TopLevelBorder_Collapsed" ) )
                            .BorderBackgroundColor( this, &SOdysseyLayerStackOutlinerTreeNode::GetNodeInnerBackgroundTint )
                            .Padding( FMargin(0) )
                            [
                                SNew( SHorizontalBox )

                                // Icon
                                + SHorizontalBox::Slot()
                                .Padding(FMargin(0.f, 0.f, 4.f, 0.f))
                                .VAlign(VAlign_Center)
                                .AutoWidth()
                                [
                                    SNew(SOverlay)

                                    + SOverlay::Slot()
                                    [
                                        //This can be an image or something else, depending of the type of layer ---
                                        InArgs._IconContent.Widget
                                    ]
                                ]

                                // Label Slot
                                + SHorizontalBox::Slot()
                                .VAlign(VAlign_Center)
                                .AutoWidth()
                                .Padding(FMargin(0.f, 0.f, 20.f, 0.f))
                                [
                                    mEditableLabel.ToSharedRef()
                                ]
                                // Arbitrary customization slot
                                + SHorizontalBox::Slot()
                                .HAlign(HAlign_Fill)
                                [
                                    InArgs._CustomContent.Widget
                                ]
                             ]
                         ]
                    ]
                ]
            ];

    ChildSlot
    [
        FinalWidget
    ];
}

//PUBLIC API ----------------------------------------------------

void SOdysseyLayerStackOutlinerTreeNode::EnterRenameMode()
{
    mEditableLabel->EnterEditingMode();
}

const TSharedPtr<OdysseyBaseLayerNode> SOdysseyLayerStackOutlinerTreeNode::GetLayerNode() const
{
    return mLayerNode;
}

void SOdysseyLayerStackOutlinerTreeNode::GetAllDescendantNodes(TSharedPtr<OdysseyBaseLayerNode> RootNode, TArray<TSharedRef<OdysseyBaseLayerNode> >& AllNodes)
{

}

//PRIVATE API ----------------------------------------------------

const FSlateBrush* SOdysseyLayerStackOutlinerTreeNode::GetNodeBorderImage() const
{
    return mLayerNode->IsExpanded() ? mExpandedBackgroundBrush : mCollapsedBackgroundBrush;
}

FSlateColor SOdysseyLayerStackOutlinerTreeNode::GetNodeBackgroundTint() const
{
    return mIsOuterTopLevelNode ? FLinearColor(FColor(48, 48, 48, 255)) : FLinearColor(FColor(62, 62, 62, 255));
}

FSlateColor SOdysseyLayerStackOutlinerTreeNode::GetNodeInnerBackgroundTint() const
{
    return FLinearColor( 0.f, 0.f, 0.f, 0.f );
}

FSlateColor SOdysseyLayerStackOutlinerTreeNode::GetTrackColorTint() const
{
    return FLinearColor::Transparent;
}

FSlateColor SOdysseyLayerStackOutlinerTreeNode::GetForegroundBasedOnSelection() const
{
    return mTableRowStyle->SelectedTextColor;
}


EVisibility SOdysseyLayerStackOutlinerTreeNode::GetExpanderVisibility() const
{
    return mLayerNode->GetNumChildren() > 0 ? EVisibility::Visible : EVisibility::Hidden;
}


FSlateColor SOdysseyLayerStackOutlinerTreeNode::GetDisplayNameColor() const
{
    return mLayerNode->GetDisplayNameColor();
}


FText SOdysseyLayerStackOutlinerTreeNode::GetDisplayNameToolTipText() const
{
    return mLayerNode->GetDisplayNameToolTipText();
}


FText SOdysseyLayerStackOutlinerTreeNode::GetDisplayName() const
{
    return mLayerNode->GetDisplayName();
}


bool SOdysseyLayerStackOutlinerTreeNode::HandleNodeLabelIsReadOnly() const
{
    return !mLayerNode->CanRenameNode();
}


void SOdysseyLayerStackOutlinerTreeNode::HandleNodeLabelTextChanged(const FText& NewLabel, ETextCommit::Type iType)
{
    mLayerNode->SetDisplayName(NewLabel);
}


#undef LOCTEXT_NAMESPACE
