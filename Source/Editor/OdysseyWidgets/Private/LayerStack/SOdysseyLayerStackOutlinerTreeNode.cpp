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
#include "Styling/SlateTypes.h"



#define LOCTEXT_NAMESPACE "OdysseyLayerStackOutlinerTreeNode"

/*
class SLayerColorPicker : public SCompoundWidget
{
public:
    static void OnOpen()
    {

    }

    static void OnClose()
    {

    }

    SLATE_BEGIN_ARGS(SLayerColorPicker){}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs)
    {
    }

    FLinearColor GetTrackColor() const
    {
    }

    void SetTrackColor(FLinearColor NewColor)
    {

    }

private:
    static TUniquePtr<FScopedTransaction> Transaction;
    static bool bMadeChanges;
};

TUniquePtr<FScopedTransaction> SLayerColorPicker::Transaction;
bool SLayerColorPicker::bMadeChanges = false;

*/



//SODYSSEYLAYERSTACKOUTLINERTREENODE----------------------------

//CONSTRUCTION/DESTRUCTION--------------------------------------
SOdysseyLayerStackOutlinerTreeNode::~SOdysseyLayerStackOutlinerTreeNode()
{
    LayerNode->OnRenameRequested().RemoveAll(this);
}



void SOdysseyLayerStackOutlinerTreeNode::Construct( const FArguments& InArgs, TSharedRef<OdysseyBaseLayerNode> Node, const TSharedRef<SOdysseyLayerStackViewRow>& InTableRow )
{
    LayerNode = Node;
    bIsOuterTopLevelNode = !Node->GetParent().IsValid();

    auto NodeHeight = [=]() -> FOptionalSize { return Node->GetNodeHeight(); };

    FMargin InnerNodePadding;
    if ( bIsInnerTopLevelNode )
    {
        InnerBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Expanded" );
        InnerNodePadding = FMargin(0.f, 1.f);
    }
    else
    {
        InnerBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TransparentBorder" );
        InnerNodePadding = FMargin(0.f);
    }

    FSlateFontInfo NodeFont = FEditorStyle::GetFontStyle("Sequencer.AnimationOutliner.RegularFont");

    EditableLabel = SNew( SInlineEditableTextBlock )
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
        .Padding(FMargin(0, Node->GetNodePadding().Combined() / 2))
        [
            SNew( SHorizontalBox )

            + SHorizontalBox::Slot()
            [
                SNew(SBox)
                .HeightOverride_Lambda(NodeHeight)
                .Padding(FMargin(5.0f, 0.0f))
                [
                    SNew( SHorizontalBox )

                    // Expand track lanes button
                    + SHorizontalBox::Slot()
                    .Padding(FMargin(2.f, 0.f, 2.f, 0.f))
                    .VAlign( VAlign_Center )
                    .AutoWidth()
                    /*[
                        SNew(SExpanderArrow, InTableRow).IndentAmount(10)
                    ]*/
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
                                    SNew(SImage)
                                    .Image(InArgs._IconBrush)
                                    .ColorAndOpacity(InArgs._IconColor)
                                ]

                                + SOverlay::Slot()
                                .VAlign(VAlign_Top)
                                .HAlign(HAlign_Right)
                                [
                                    SNew(SImage)
                                    .Image(InArgs._IconOverlayBrush)
                                ]

                                + SOverlay::Slot()
                                [
                                    SNew(SSpacer)
                                    .Visibility(EVisibility::Visible)
                                    .ToolTipText(InArgs._IconToolTipText)
                                ]
                            ]

                            // Label Slot
                            + SHorizontalBox::Slot()
                            .VAlign(VAlign_Center)
                            .AutoWidth()
                            .Padding(FMargin(0.f, 0.f, 20.f, 0.f))
                            [
                                EditableLabel.ToSharedRef()
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
    EditableLabel->EnterEditingMode();
}


void SOdysseyLayerStackOutlinerTreeNode::GetAllDescendantNodes(TSharedPtr<OdysseyBaseLayerNode> RootNode, TArray<TSharedRef<OdysseyBaseLayerNode> >& AllNodes)
{

}

void SOdysseyLayerStackOutlinerTreeNode::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{

}

void SOdysseyLayerStackOutlinerTreeNode::OnMouseLeave(const FPointerEvent& MouseEvent)
{

}

const FSlateBrush* SOdysseyLayerStackOutlinerTreeNode::GetNodeBorderImage() const
{
    return LayerNode->IsExpanded() ? ExpandedBackgroundBrush : CollapsedBackgroundBrush;
}

FSlateColor SOdysseyLayerStackOutlinerTreeNode::GetNodeBackgroundTint() const
{
    return bIsOuterTopLevelNode ? FLinearColor(FColor(48, 48, 48, 255)) : FLinearColor(FColor(62, 62, 62, 255));
}

FSlateColor SOdysseyLayerStackOutlinerTreeNode::GetNodeInnerBackgroundTint() const
{
    return FLinearColor( 0.f, 0.f, 0.f, 0.f );
}

TSharedRef<SWidget> SOdysseyLayerStackOutlinerTreeNode::OnGetColorPicker() const
{
    return SNullWidget::NullWidget;
}

FSlateColor SOdysseyLayerStackOutlinerTreeNode::GetTrackColorTint() const
{
    return FLinearColor::Transparent;
}

FSlateColor SOdysseyLayerStackOutlinerTreeNode::GetForegroundBasedOnSelection() const
{
    return TableRowStyle->SelectedTextColor;
}


EVisibility SOdysseyLayerStackOutlinerTreeNode::GetExpanderVisibility() const
{
    return LayerNode->GetNumChildren() > 0 ? EVisibility::Visible : EVisibility::Hidden;
}


FSlateColor SOdysseyLayerStackOutlinerTreeNode::GetDisplayNameColor() const
{
    return LayerNode->GetDisplayNameColor();
}


FText SOdysseyLayerStackOutlinerTreeNode::GetDisplayNameToolTipText() const
{
    return LayerNode->GetDisplayNameToolTipText();
}


FText SOdysseyLayerStackOutlinerTreeNode::GetDisplayName() const
{
    return LayerNode->GetDisplayName();
}


bool SOdysseyLayerStackOutlinerTreeNode::HandleNodeLabelIsReadOnly() const
{
    return !LayerNode->CanRenameNode();
}


void SOdysseyLayerStackOutlinerTreeNode::HandleNodeLabelTextChanged(const FText& NewLabel, ETextCommit::Type iType)
{
    LayerNode->SetDisplayName(NewLabel);
}


#undef LOCTEXT_NAMESPACE
