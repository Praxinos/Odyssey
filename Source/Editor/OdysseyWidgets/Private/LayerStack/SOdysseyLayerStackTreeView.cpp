// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "LayerStack/SOdysseyLayerStackTreeView.h"
#include "LayerStack/FOdysseyLayerStackNodeDragDropOp.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStackView"
#include "EditorStyleSet.h"
#include "OdysseyStyleSet.h"


//STACK VIEW

//CONSTRUCTION / DESTRUCTION---------------------------------------------

void SOdysseyLayerStackTreeView::Construct(const FArguments& InArgs)
{
    mLayerStackNodeTree = InArgs._NodeTree;
    // mLayerStackNodeTree.Get()->OnUpdated().AddRaw(this, &SOdysseyLayerStackTreeView::Refresh, -1);

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
    FMenuBuilder menuBuilder(bShouldCloseWindowAfterMenuSelection, mLayerStackNodeTree.Get()->GetLayerStack().GetCommandBindings());

    mOnGetContextMenuContent.ExecuteIfBound(menuBuilder);

    return menuBuilder.MakeWidget();
}

void SOdysseyLayerStackTreeView::OnSelectionChanged(TSharedPtr<IOdysseyBaseLayerNode> iSelectedNode, ESelectInfo::Type iSelectionInfo )
{
    this->Private_ClearSelection();

    if( iSelectedNode && mLayerStackNodeTree.Get()->GetLayerStack().GetLayerStackData())
    {
        int index = (mRootNodes.Num() - 1) - mRootNodes.Find(iSelectedNode.ToSharedRef()); //We have the nodes in the inverse order than the odysseyLayerStackData for GUI purposes
		mLayerStackNodeTree.Get()->GetLayerStack().GetLayerStackData()->SetCurrentLayer( iSelectedNode->GetLayerDataPtr() );
        mSelectedNode = iSelectedNode;
        this->Private_SetItemSelection( mSelectedNode.ToSharedRef(), true );
    }
    else if( mSelectedNode )
    {
        //Stay selected
        this->Private_SetItemSelection( mSelectedNode.ToSharedRef(), true );
    }

    mPropertyView->DetachWidget();
    if( mSelectedNode )
        mPropertyView->AttachWidget( mSelectedNode->GenerateContainerWidgetForPropertyView() );
    else
        mPropertyView->AttachWidget( SNullWidget::NullWidget );
}

TSharedRef<ITableRow> SOdysseyLayerStackTreeView::OnGenerateRow(IOdysseyBaseLayerNodeRef iDisplayNode, const TSharedRef<STableViewBase>& iOwnerTable)
{
    TSharedRef<SOdysseyLayerStackViewRow> Row =
    SNew(SOdysseyLayerStackViewRow, iOwnerTable, iDisplayNode)
    .OnGenerateWidgetForColumn(this, &SOdysseyLayerStackTreeView::GenerateWidgetForColumn);

    return Row;
}


TSharedRef<SWidget> SOdysseyLayerStackTreeView::GenerateWidgetForColumn(const IOdysseyBaseLayerNodeRef& InNode, const FName& ColumnId, const TSharedRef<SOdysseyLayerStackViewRow>& Row) const
{
    const auto* Definition = mColumns.Find(ColumnId);

    if (ensureMsgf(Definition, TEXT("Invalid column name specified")))
    {
        return Definition->mGenerator(InNode, Row);
    }

    return SNullWidget::NullWidget;
}

void SOdysseyLayerStackTreeView::OnGetChildren(IOdysseyBaseLayerNodeRef iParent, TArray<IOdysseyBaseLayerNodeRef>& oChildren) const
{
}

//PUBLIC API

void SOdysseyLayerStackTreeView::Refresh( int iOverrideNewSelectedNodeIndex /* = -1*/ )
{
    int numberOfNodes = mLayerStackNodeTree.Get()->GetRootNodes().Num();

    //We reset the copy of the root nodes
    mRootNodes.Reset( numberOfNodes );

    int indexCurrentNode = mLayerStackNodeTree.Get()->Update();
    mSelectedNode = NULL;

    //Refresh the treeView and the selected node
    if( iOverrideNewSelectedNodeIndex == -1 )
    {
        if(mLayerStackNodeTree.Get()->GetRootNodes().Num() > 0 )
        {
            this->Private_ClearSelection();
            mSelectedNode = mLayerStackNodeTree.Get()->GetRootNodes()[indexCurrentNode];
            this->Private_SetItemSelection( mSelectedNode.ToSharedRef(), true );
        }
    }
    else if( iOverrideNewSelectedNodeIndex >= 0 && iOverrideNewSelectedNodeIndex < mLayerStackNodeTree.Get()->GetRootNodes().Num() )
    {
        this->Private_ClearSelection();
        mSelectedNode = mLayerStackNodeTree.Get()->GetRootNodes()[iOverrideNewSelectedNodeIndex];
        this->Private_SetItemSelection( mSelectedNode.ToSharedRef(), true );
    }

    //Refresh the selected node related areas

    for (const auto& rootNode : mLayerStackNodeTree.Get()->GetRootNodes())
    {
        if (!rootNode->IsHidden())
        {
            mRootNodes.Add(rootNode);
        }
    }

    if( mPropertyView )
    {
        mPropertyView->DetachWidget();
        if( mSelectedNode != NULL )
            mPropertyView->AttachWidget( mSelectedNode->GenerateContainerWidgetForPropertyView() );
        else
            mPropertyView->AttachWidget( SNullWidget::NullWidget );
    }

    //STreeView Refresh
    RequestTreeRefresh();
}

TSharedPtr<IOdysseyBaseLayerNode>
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
    // Define a column for the Outliner
    auto GenerateOutliner = [=](const IOdysseyBaseLayerNodeRef& iNode, const TSharedRef<SOdysseyLayerStackViewRow>& iRow)
    {
        return iNode->GenerateContainerWidgetForOutliner(iRow);
    };

    mColumns.Add("Outliner", FLayerStackTreeViewColumn(GenerateOutliner, 1.f));

    // Now populate the header row with the columns
    for (auto& pair : mColumns)
    {
            HeaderRow->AddColumn(
                SHeaderRow::Column(pair.Key)
                .FillWidth(pair.Value.mWidth));
    }
}


//-----------------------------------------------------------------------




//VIEW ROW

//CONSTRUCTION / DESTRUCTION---------------------------------------------

SOdysseyLayerStackViewRow::~SOdysseyLayerStackViewRow()
{
}

/** Construct function for this widget */
void SOdysseyLayerStackViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& iOwnerTableView, const IOdysseyBaseLayerNodeRef& iNode)
{
    //static_cast is dangerous, but we're sure that our owner table is of SOdysseyLayerStackTreeView type
    mTreeView = static_cast<SOdysseyLayerStackTreeView*>(&iOwnerTableView.Get());
    mNode = iNode;
    mOnGenerateWidgetForColumn = InArgs._OnGenerateWidgetForColumn;
    bool bIsSelectable = iNode->IsSelectable();

    SMultiColumnTableRow::Construct(
                                    SMultiColumnTableRow::FArguments()
                                    .Style(&FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("OdysseyLayerStack.AlternatedRows"))
                                    .OnDragDetected(this, &SOdysseyLayerStackViewRow::OnDragDetected)
                                    .OnCanAcceptDrop(this, &SOdysseyLayerStackViewRow::OnCanAcceptDrop)
                                    .OnAcceptDrop(this, &SOdysseyLayerStackViewRow::OnAcceptDrop)
                                    .ShowSelection(bIsSelectable),
                                    iOwnerTableView);
}

TSharedRef<SWidget> SOdysseyLayerStackViewRow::GenerateWidgetForColumn(const FName& iColumnId)
{
    auto pinnedNode = mNode.Pin();
    if (pinnedNode.IsValid())
    {
        return mOnGenerateWidgetForColumn.Execute(pinnedNode.ToSharedRef(), iColumnId, SharedThis(this));
    }

    return SNullWidget::NullWidget;
}


//DELEGATES-----------------------------------------------------------

FReply SOdysseyLayerStackViewRow::OnDragDetected( const FGeometry& iGeometry, const FPointerEvent& iPointerEvent )
{
    TSharedPtr<IOdysseyBaseLayerNode> displayNode = mNode.Pin();
    mTreeView->OnSelectionChanged( displayNode );

    if ( displayNode.IsValid() )
    {
        TArray<TSharedRef<IOdysseyBaseLayerNode> > draggableNodes;
        draggableNodes.Add(displayNode.ToSharedRef());

        FText defaultText = FText::Format( NSLOCTEXT( "OdysseyLayerStackTreeViewRow", "DefaultDragDropFormat", "Move {0} item(s)" ), FText::AsNumber( draggableNodes.Num() ) );

        TSharedRef<FOdysseyLayerStackNodeDragDropOp> dragDropOp = FOdysseyLayerStackNodeDragDropOp::New( draggableNodes, defaultText, nullptr );

        return FReply::Handled().BeginDragDrop( dragDropOp );
    }
    return FReply::Unhandled();
}

TOptional<EItemDropZone> SOdysseyLayerStackViewRow::OnCanAcceptDrop( const FDragDropEvent& iDragDropEvent, EItemDropZone iItemDropZone, IOdysseyBaseLayerNodeRef iDisplayNode )
{
    TSharedPtr<FOdysseyLayerStackNodeDragDropOp> dragDropOp = iDragDropEvent.GetOperationAs<FOdysseyLayerStackNodeDragDropOp>();

    if( dragDropOp.IsValid() )
    {
        dragDropOp->ResetToDefaultToolTip();
        TOptional<EItemDropZone> allowedDropZone = iDisplayNode->CanDrop( *dragDropOp, iItemDropZone );
        if ( allowedDropZone.IsSet() == false )
        {
            dragDropOp->SetCurrentIconBrush( FEditorStyle::GetBrush( TEXT( "Graph.ConnectorFeedback.Error" ) ) );
        }
        return allowedDropZone;
    }
    return TOptional<EItemDropZone>();
}

FReply SOdysseyLayerStackViewRow::OnAcceptDrop( const FDragDropEvent& iDragDropEvent, EItemDropZone iItemDropZone, IOdysseyBaseLayerNodeRef iDisplayNode )
{
    TSharedPtr<FOdysseyLayerStackNodeDragDropOp> dragDropOp = iDragDropEvent.GetOperationAs<FOdysseyLayerStackNodeDragDropOp>();
    if ( dragDropOp.IsValid())
    {
        iDisplayNode->Drop( dragDropOp->GetDraggedNodes(), iItemDropZone );
        mTreeView->OnSelectionChanged( dragDropOp->GetDraggedNodes()[0] );
        mTreeView->Refresh();

        return FReply::Handled();
    }
    return FReply::Unhandled();
}


#undef LOCTEXT_NAMESPACE
