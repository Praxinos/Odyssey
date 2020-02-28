// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/SOdysseyLayerStackTreeView.h"
#include "LayerStack/FOdysseyLayerStackNodeDragDropOp.h"
#include "LayerStack/FOdysseyLayerStackCommands.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStackView"
#include "EditorStyleSet.h"
#include "OdysseyStyleSet.h"


//STACK VIEW

//CONSTRUCTION / DESTRUCTION---------------------------------------------

void SOdysseyLayerStackTreeView::Construct(const FArguments& InArgs, const TSharedRef<FOdysseyLayerStackTree>& InNodeTree)
{
    mLayerStackNodeTree = InNodeTree;
    mLayerStackNodeTree->OnUpdated().AddRaw(this, &SOdysseyLayerStackTreeView::Refresh, -1);

    FOdysseyLayerStackModel LayerStackModel = InNodeTree->GetLayerStack();

    HeaderRow = SNew(SHeaderRow).Visibility(EVisibility::Collapsed);
    mOnGetContextMenuContent = InArgs._OnGetContextMenuContent;

    SetupColumns(InArgs);

    STreeView::Construct
    (
     STreeView::FArguments()
     .TreeItemsSource(&mRootNodes)
     .SelectionMode(ESelectionMode::Single)
     .OnGenerateRow(this, &SOdysseyLayerStackTreeView::OnGenerateRow)
     .OnGetChildren(this, &SOdysseyLayerStackTreeView::OnGetChildren)
     .HeaderRow(HeaderRow)
     .ExternalScrollbar(InArgs._ExternalScrollbar)
     .OnSelectionChanged(this, &SOdysseyLayerStackTreeView::OnSelectionChanged)
     .AllowOverscroll(EAllowOverscroll::No)
     .OnContextMenuOpening( this, &SOdysseyLayerStackTreeView::OnContextMenuOpening )
     );

    mSelectedNode = nullptr;
}


//CALLBACKS---------------------------------------------------------------

TSharedPtr<SWidget> SOdysseyLayerStackTreeView::OnContextMenuOpening()
{
    if( mSelectedNode )
    {
        return mSelectedNode->OnSummonContextMenu();
    }

    const bool bShouldCloseWindowAfterMenuSelection = true;
    FMenuBuilder MenuBuilder(bShouldCloseWindowAfterMenuSelection, mLayerStackNodeTree->GetLayerStack().GetCommandBindings());

    mOnGetContextMenuContent.ExecuteIfBound(MenuBuilder);

    return MenuBuilder.MakeWidget();
}

void SOdysseyLayerStackTreeView::OnSelectionChanged(TSharedPtr<OdysseyBaseLayerNode> InSelectedNode, ESelectInfo::Type InSelectionInfo )
{
    this->Private_ClearSelection();

    if( InSelectedNode )
    {
        int index = (mRootNodes.Num() - 1) - mRootNodes.Find(InSelectedNode.ToSharedRef()); //We have the nodes in the inverse order than the odysseyLayerStackData for GUI purposes
        mLayerStackNodeTree->GetLayerStack().GetLayerStackData()->SetCurrentLayer( InSelectedNode->GetLayerDataPtr() );
        mSelectedNode = InSelectedNode;
        this->Private_SetItemSelection( mSelectedNode.ToSharedRef(), true );
    }
    else if( mSelectedNode )
    {
        //Stay selected
        this->Private_SetItemSelection( mSelectedNode.ToSharedRef(), true );
    }

    mPropertyView->DetachWidget();
    mPropertyView->AttachWidget( mSelectedNode->GenerateContainerWidgetForPropertyView() );
}

TSharedRef<ITableRow> SOdysseyLayerStackTreeView::OnGenerateRow(OdysseyBaseLayerNodeRef InDisplayNode, const TSharedRef<STableViewBase>& OwnerTable)
{
    TSharedRef<SOdysseyLayerStackViewRow> Row =
    SNew(SOdysseyLayerStackViewRow, OwnerTable, InDisplayNode)
    .OnGenerateWidgetForColumn(this, &SOdysseyLayerStackTreeView::GenerateWidgetForColumn);

    return Row;
}


TSharedRef<SWidget> SOdysseyLayerStackTreeView::GenerateWidgetForColumn(const OdysseyBaseLayerNodeRef& InNode, const FName& ColumnId, const TSharedRef<SOdysseyLayerStackViewRow>& Row) const
{
    const auto* Definition = mColumns.Find(ColumnId);

    if (ensureMsgf(Definition, TEXT("Invalid column name specified")))
    {
        return Definition->mGenerator(InNode, Row);
    }

    return SNullWidget::NullWidget;
}

void SOdysseyLayerStackTreeView::OnGetChildren(OdysseyBaseLayerNodeRef InParent, TArray<OdysseyBaseLayerNodeRef>& OutChildren) const
{
    for (const auto& Node : InParent->GetChildNodes())
    {
        if (!Node->IsHidden())
        {
            OutChildren.Add(Node);
        }
    }
}

//PUBLIC API

void SOdysseyLayerStackTreeView::Refresh( int OverrideNewSelectedNodeIndex /* = -1*/ )
{
    int NumberOfNodes = mLayerStackNodeTree->GetRootNodes().Num();

    //We reset the copy of the root nodes
    mRootNodes.Reset( NumberOfNodes );

    int indexCurrentNode = mLayerStackNodeTree->Update();

    //Refresh the treeView and the selected node
    if( OverrideNewSelectedNodeIndex == -1 )
    {
        if( mLayerStackNodeTree->GetRootNodes().Num() > 0 )
        {
            this->Private_ClearSelection();
            mSelectedNode = mLayerStackNodeTree->GetRootNodes()[indexCurrentNode];
            this->Private_SetItemSelection( mSelectedNode.ToSharedRef(), true );
        }
    }
    else if( OverrideNewSelectedNodeIndex >= 0 && OverrideNewSelectedNodeIndex < mLayerStackNodeTree->GetRootNodes().Num() )
    {
        this->Private_ClearSelection();
        mSelectedNode = mLayerStackNodeTree->GetRootNodes()[OverrideNewSelectedNodeIndex];
        this->Private_SetItemSelection( mSelectedNode.ToSharedRef(), true );
    }

    //Refresh the selected node related areas

    for (const auto& RootNode : mLayerStackNodeTree->GetRootNodes())
    {
        if (RootNode->IsExpanded())
        {
            SetItemExpansion(RootNode, true);
        }

        if (!RootNode->IsHidden())
        {
            mRootNodes.Add(RootNode);
        }
    }

    if( mPropertyView )
    {
        mPropertyView->DetachWidget();
        mPropertyView->AttachWidget( mSelectedNode->GenerateContainerWidgetForPropertyView() );
    }

    //STreeView Refresh
    RequestTreeRefresh();
}

TSharedPtr<OdysseyBaseLayerNode>
SOdysseyLayerStackTreeView::GetSelectedNode()
{
    return mSelectedNode;
}

SVerticalBox::FSlot*& SOdysseyLayerStackTreeView::GetPropertyView()
{
    return mPropertyView;
}



//PROTECTED API----------------------------------------------------------

void SOdysseyLayerStackTreeView::SetupColumns(const FArguments& InArgs)
{
    FOdysseyLayerStackModel& LayerStackModel = mLayerStackNodeTree->GetLayerStack();

    // Define a column for the Outliner
    auto GenerateOutliner = [=](const OdysseyBaseLayerNodeRef& InNode, const TSharedRef<SOdysseyLayerStackViewRow>& InRow)
    {
        return InNode->GenerateContainerWidgetForOutliner(InRow);
    };

    mColumns.Add("Outliner", FLayerStackTreeViewColumn(GenerateOutliner, 1.f));

    // Now populate the header row with the columns
    for (auto& Pair : mColumns)
    {
            HeaderRow->AddColumn(
                SHeaderRow::Column(Pair.Key)
                .FillWidth(Pair.Value.mWidth));
    }
}


//-----------------------------------------------------------------------




//VIEW ROW

//CONSTRUCTION / DESTRUCTION---------------------------------------------

SOdysseyLayerStackViewRow::~SOdysseyLayerStackViewRow()
{
}

/** Construct function for this widget */
void SOdysseyLayerStackViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, const OdysseyBaseLayerNodeRef& InNode)
{
    //static_cast is dangerous, but we're sure that our owner table is of SOdysseyLayerStackTreeView type
    mTreeView = static_cast<SOdysseyLayerStackTreeView*>(&OwnerTableView.Get());
    mNode = InNode;
    mOnGenerateWidgetForColumn = InArgs._OnGenerateWidgetForColumn;
    bool bIsSelectable = InNode->IsSelectable();

    SMultiColumnTableRow::Construct(
                                    SMultiColumnTableRow::FArguments()
                                    .Style(&FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("OdysseyLayerStack.AlternatedRows"))
                                    .OnDragDetected(this, &SOdysseyLayerStackViewRow::OnDragDetected)
                                    .OnCanAcceptDrop(this, &SOdysseyLayerStackViewRow::OnCanAcceptDrop)
                                    .OnAcceptDrop(this, &SOdysseyLayerStackViewRow::OnAcceptDrop)
                                    .ShowSelection(bIsSelectable),
                                    OwnerTableView);
}

TSharedRef<SWidget> SOdysseyLayerStackViewRow::GenerateWidgetForColumn(const FName& ColumnId)
{
    auto PinnedNode = mNode.Pin();
    if (PinnedNode.IsValid())
    {
        return mOnGenerateWidgetForColumn.Execute(PinnedNode.ToSharedRef(), ColumnId, SharedThis(this));
    }

    return SNullWidget::NullWidget;
}


//DELEGATES-----------------------------------------------------------

FReply SOdysseyLayerStackViewRow::OnDragDetected( const FGeometry& InGeometry, const FPointerEvent& InPointerEvent )
{
    TSharedPtr<OdysseyBaseLayerNode> DisplayNode = mNode.Pin();
    mTreeView->OnSelectionChanged( DisplayNode );

    if ( DisplayNode.IsValid() )
    {
        TArray<TSharedRef<OdysseyBaseLayerNode> > DraggableNodes;
        DraggableNodes.Add(DisplayNode.ToSharedRef());

        FText DefaultText = FText::Format( NSLOCTEXT( "OdysseyLayerStackTreeViewRow", "DefaultDragDropFormat", "Move {0} item(s)" ), FText::AsNumber( DraggableNodes.Num() ) );

        TSharedRef<FOdysseyLayerStackNodeDragDropOp> DragDropOp = FOdysseyLayerStackNodeDragDropOp::New( DraggableNodes, DefaultText, nullptr );

        return FReply::Handled().BeginDragDrop( DragDropOp );
    }
    return FReply::Unhandled();
}

TOptional<EItemDropZone> SOdysseyLayerStackViewRow::OnCanAcceptDrop( const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, OdysseyBaseLayerNodeRef DisplayNode )
{
    TSharedPtr<FOdysseyLayerStackNodeDragDropOp> DragDropOp = DragDropEvent.GetOperationAs<FOdysseyLayerStackNodeDragDropOp>();

    if( DragDropOp.IsValid() )
    {
        DragDropOp->ResetToDefaultToolTip();
        TOptional<EItemDropZone> AllowedDropZone = DisplayNode->CanDrop( *DragDropOp, InItemDropZone );
        if ( AllowedDropZone.IsSet() == false )
        {
            DragDropOp->SetCurrentIconBrush( FEditorStyle::GetBrush( TEXT( "Graph.ConnectorFeedback.Error" ) ) );
        }
        return AllowedDropZone;
    }
    return TOptional<EItemDropZone>();
}

FReply SOdysseyLayerStackViewRow::OnAcceptDrop( const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, OdysseyBaseLayerNodeRef DisplayNode )
{
    TSharedPtr<FOdysseyLayerStackNodeDragDropOp> DragDropOp = DragDropEvent.GetOperationAs<FOdysseyLayerStackNodeDragDropOp>();
    if ( DragDropOp.IsValid())
    {
        DisplayNode->Drop( DragDropOp->GetDraggedNodes(), InItemDropZone );
        mTreeView->OnSelectionChanged( DragDropOp->GetDraggedNodes()[0] );
        mTreeView->Refresh();

        return FReply::Handled();
    }
    return FReply::Unhandled();
}


#undef LOCTEXT_NAMESPACE
