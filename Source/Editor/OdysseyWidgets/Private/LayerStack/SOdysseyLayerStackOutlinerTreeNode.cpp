// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "LayerStack/SOdysseyLayerStackOutlinerTreeNode.h"

#include "LayerStack/SOdysseyLayerStackTreeView.h"
#include "LayerStack/LayersGUI/OdysseyBaseLayerNode.h"
#include "ScopedTransaction.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SComboButton.h"
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



void SOdysseyLayerStackOutlinerTreeNode::Construct( const FArguments& InArgs, TSharedRef<IOdysseyBaseLayerNode> iNode, const TSharedRef<SOdysseyLayerStackViewRow>& iTableRow )
{
    mLayerNode = iNode;

    auto nodeHeight = [=]() -> FOptionalSize { return iNode->GetNodeHeight(); };

    FSlateFontInfo NodeFont = FEditorStyle::GetFontStyle("Sequencer.AnimationOutliner.RegularFont");

    mEditableLabel = SNew( SInlineEditableTextBlock )
    .IsReadOnly(this, &SOdysseyLayerStackOutlinerTreeNode::HandleNodeLabelIsReadOnly )
    .Font(NodeFont)
    .ColorAndOpacity(this, &SOdysseyLayerStackOutlinerTreeNode::GetDisplayNameColor)
    .OnTextCommitted(this, &SOdysseyLayerStackOutlinerTreeNode::HandleNodeLabelTextChanged)
    .Text(this, &SOdysseyLayerStackOutlinerTreeNode::GetDisplayName)
    .ToolTipText(this, &SOdysseyLayerStackOutlinerTreeNode::GetDisplayNameToolTipText)
    .Clipping(EWidgetClipping::ClipToBounds)
	.IsSelected(FIsSelected::CreateSP(iTableRow, &SOdysseyLayerStackViewRow::IsSelectedExclusively));

    iNode->OnRenameRequested().AddRaw( this, &SOdysseyLayerStackOutlinerTreeNode::EnterRenameMode );

    TSharedRef<SWidget>    finalWidget =
        SNew( SBorder )
        .VAlign( VAlign_Center )
        .BorderBackgroundColor( this, &SOdysseyLayerStackOutlinerTreeNode::GetNodeBackgroundTint )
        .Padding(FMargin(iNode->GetNodePadding().mLeft, iNode->GetNodePadding().mTop, 0, iNode->GetNodePadding().mBottom ))
        [
            SNew( SHorizontalBox )

            + SHorizontalBox::Slot()
            [
                SNew(SBox)
                .HeightOverride_Lambda(nodeHeight)
                .Padding(FMargin(5.0f, 0.0f))
                [
                    SNew( SHorizontalBox )

                     + SHorizontalBox::Slot()
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
        finalWidget
    ];
}

//PUBLIC API ----------------------------------------------------

void SOdysseyLayerStackOutlinerTreeNode::EnterRenameMode()
{
    mEditableLabel->EnterEditingMode();
}

const TSharedPtr<IOdysseyBaseLayerNode> SOdysseyLayerStackOutlinerTreeNode::GetLayerNode() const
{
    return mLayerNode;
}

void SOdysseyLayerStackOutlinerTreeNode::GetAllDescendantNodes(TSharedPtr<IOdysseyBaseLayerNode> iRootNode, TArray<TSharedRef<IOdysseyBaseLayerNode> >& iAllNodes)
{

}

//PRIVATE API ----------------------------------------------------

FSlateColor SOdysseyLayerStackOutlinerTreeNode::GetNodeBackgroundTint() const
{
    return FLinearColor(FColor(62, 62, 62, 255));
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


void SOdysseyLayerStackOutlinerTreeNode::HandleNodeLabelTextChanged(const FText& iNewLabel, ETextCommit::Type iType)
{
    mLayerNode->SetDisplayName(iNewLabel);
}


#undef LOCTEXT_NAMESPACE
