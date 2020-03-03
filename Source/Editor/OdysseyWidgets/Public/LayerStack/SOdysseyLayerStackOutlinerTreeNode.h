// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateColor.h"
#include "Layout/Visibility.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "LayerStack/LayersGUI/OdysseyBaseLayerNode.h"

class SOdysseyLayerStackViewRow;
class SInlineEditableTextBlock;
struct FSlateBrush;
struct FTableRowStyle;

/**
 * The base container GUI for a tree node
 */
class SOdysseyLayerStackOutlinerTreeNode : public SCompoundWidget
{
public:
    SOdysseyLayerStackOutlinerTreeNode()
        : mExpandedBackgroundBrush( NULL )
        , mCollapsedBackgroundBrush( NULL )
        , mInnerBackgroundBrush( NULL )
    {}

    ~SOdysseyLayerStackOutlinerTreeNode();

    SLATE_BEGIN_ARGS(SOdysseyLayerStackOutlinerTreeNode){}
        SLATE_ATTRIBUTE(FText, IconToolTipText)
        SLATE_NAMED_SLOT(FArguments, IconContent)
        SLATE_NAMED_SLOT(FArguments, CustomContent)
    SLATE_END_ARGS()


    void Construct( const FArguments& InArgs, TSharedRef<OdysseyBaseLayerNode> iNode, const TSharedRef<SOdysseyLayerStackViewRow>& iTableRow );


public:
    //PUBLIC API
    /** Change the node's label text to edit mode. */
    void EnterRenameMode();

    /** @return The display node used by this widget. */
    const TSharedPtr<OdysseyBaseLayerNode> GetLayerNode() const;

private:
    //PRIVATE API
    FSlateColor GetForegroundBasedOnSelection() const;

    /** Get the tint to apply to the color indicator based on this node's track */
    FSlateColor GetTrackColorTint() const;

    /**
     * @return The border image to show in the tree node.
     */
    const FSlateBrush* GetNodeBorderImage() const;

    /**
     * @return The tint to apply to the border image
     */
    FSlateColor GetNodeBackgroundTint() const;

    /**
    * @return The tint to apply to the border image for the inner portion of the node.
    */
    FSlateColor GetNodeInnerBackgroundTint() const;

    /**
     * @return The expander visibility of this node.
     */
    EVisibility GetExpanderVisibility() const;

    /**
     * @return The color used to draw the display name.
     */
    FSlateColor GetDisplayNameColor() const;

    /**
     * @return The text displayed for the tool tip for the diplay name label.
     */
    FText GetDisplayNameToolTipText() const;

    /**
     * @return The display name for this node.
     */
    FText GetDisplayName() const;

    /** Callback for checking whether the node label is read only. */
    bool HandleNodeLabelIsReadOnly() const;

    /** Callback for when the node label text has changed. */
    void HandleNodeLabelTextChanged(const FText& iNewLabel, ETextCommit::Type iType);

    /** Get all descendant nodes from the given root node. */
    void GetAllDescendantNodes(TSharedPtr<OdysseyBaseLayerNode> iRootNode, TArray<TSharedRef<OdysseyBaseLayerNode> >& iAllNodes);

private:

    /** Layout node the widget is visualizing. */
    TSharedPtr<OdysseyBaseLayerNode> mLayerNode;

    /** Holds the editable text label widget. */
    TSharedPtr<SInlineEditableTextBlock> mEditableLabel;

    /** True if this node is a top level node, at the root of the tree, false otherwise */
    bool mIsOuterTopLevelNode;

    /** True if this is a top level node inside or a folder, otherwise false. */
    bool mIsInnerTopLevelNode;

    /** Default background brush for this node when expanded */
    const FSlateBrush* mExpandedBackgroundBrush;

    /** Default background brush for this node when collapsed */
    const FSlateBrush* mCollapsedBackgroundBrush;

    /** The brush to use when drawing the background for the inner portion of the node. */
    const FSlateBrush* mInnerBackgroundBrush;

    /** The table row style used for nodes in the tree. This is required as we don't actually use the tree for selection. */
    const FTableRowStyle* mTableRowStyle;
};
