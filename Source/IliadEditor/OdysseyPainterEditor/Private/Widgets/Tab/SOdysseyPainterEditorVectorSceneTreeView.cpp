// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeViewRow.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeViewContextMenu.h"
#include "OdysseyStyle.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyVector.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "HUD/OdysseyVectorHUD.h"
#include "OdysseyPainterEditor.h"
#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "OdysseyPainterEditorSource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"


SOdysseyPainterEditorVectorSceneTreeView::~SOdysseyPainterEditorVectorSceneTreeView()
{
}

SOdysseyPainterEditorVectorSceneTreeView::SOdysseyPainterEditorVectorSceneTreeView()
    : mCommandList(MakeShared<FUICommandList>())
{
    MapActionsToCommandList();
}

void
SOdysseyPainterEditorVectorSceneTreeView::Construct( const FArguments& InArgs )
{
    TSharedPtr<SHeaderRow> headerRow = SNew(SHeaderRow)
                                      .Visibility( EVisibility::Collapsed )
                                      + SHeaderRow::Column("Visible")
                                      .FixedWidth( 24.0f )
                                      [
                                          SNew(SBorder)
                                          .Padding(0,0)
                                          .BorderBackgroundColor( FSlateColor( FLinearColor( 0, 0, 0, 0 ) ) )
                                          .Content()
                                          [
                                              SNew(SImage)
                                              .Image( FOdysseyStyle::GetBrush("Level.VisibleIcon16x") )
                                          ]
                                      ]
                                      + SHeaderRow::Column("HUD Color")
                                      .FixedWidth( 3.0f )
                                      [
                                          SNullWidget::NullWidget
                                      ]
                                      + SHeaderRow::Column("Name");

    mEditor = InArgs._Editor;

    STreeView<TSharedPtr<FVectorSceneTreeViewItem>>::Construct(
        STreeView<TSharedPtr<FVectorSceneTreeViewItem>>::FArguments()
        // for some reason, SetTreeItemsSource does not work, so we have to use an array that we
        // call mItemsSource and that we will updates with the desired items
        .TreeItemsSource(&mItemsSource)
        .OnGenerateRow( this, &SOdysseyPainterEditorVectorSceneTreeView::OnGenerateRow )
        .OnGetChildren( this, &SOdysseyPainterEditorVectorSceneTreeView::OnGetChildren )
        .OnExpansionChanged( this, &SOdysseyPainterEditorVectorSceneTreeView::OnExpansionChanged )
        .OnSelectionChanged( this, &SOdysseyPainterEditorVectorSceneTreeView::OnSelectionChanged )
        //.OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SOdysseyPainterEditorVectorSceneTreeView::OnContextMenuOpening )
        //.SelectionMode( ESelectionMode::Multi )
        .HeaderRow(headerRow)
    );
}

void
SOdysseyPainterEditorVectorSceneTreeView::RenameSelectedItem()
{
    TArray<TSharedPtr<FVectorSceneTreeViewItem>> selectedItems = GetSelectedItems();

    // no need to create an undo record or do anything if the selection is empty
    if( selectedItems.Num() )
    {
        TSharedPtr<ITableRow> tableRow = WidgetFromItem( selectedItems[0] );
        TSharedPtr<SOdysseyPainterEditorVectorSceneTreeViewRow> itemWidget = StaticCastSharedPtr<SOdysseyPainterEditorVectorSceneTreeViewRow>(tableRow);

        itemWidget->Rename();
    }
}

TSharedPtr<SWidget>
SOdysseyPainterEditorVectorSceneTreeView::OnContextMenuOpening()
{
    return SOdysseyPainterEditorVectorSceneTreeViewContextMenu::CreateWidget( this );
}

FReply
SOdysseyPainterEditorVectorSceneTreeView::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
    if (mCommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return STreeView<TSharedPtr<FVectorSceneTreeViewItem>>::OnKeyDown( iGeometry, iKeyEvent );
}

TSharedPtr<FVectorSceneTreeViewItem>
SOdysseyPainterEditorVectorSceneTreeView::GetRootItem()
{
    return mRootItem;
}

FOdysseyPainterEditor*
SOdysseyPainterEditorVectorSceneTreeView::GetEditor() const
{
    return mEditor;
}

void
SOdysseyPainterEditorVectorSceneTreeView::BuildTree( const TSharedPtr<FVectorSceneTreeViewItem> iItem )
{
    std::list<FOdysseyVectorObject*>& childrenList = iItem.Get()->GetVectorObject()->GetChildrenList();

    iItem.Get()->mChildren.Empty();

    for( FOdysseyVectorObject* child : childrenList )
    {
        TSharedPtr<FVectorSceneTreeViewItem> childItem = MakeShareable(new FVectorSceneTreeViewItem(child, true ));

        //iItem.Get()->mChildren.Add( childItem );
        // reverse order in order to get the most forward objet on top of the hierarchy
        iItem.Get()->mChildren.Insert( childItem, 0 );

        BuildTree( childItem );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::ExpandTree( const TSharedPtr<FVectorSceneTreeViewItem> iItem )
{
    FOdysseyVectorObject* itemObject = iItem.Get()->GetVectorObject();

    if( itemObject->IsExpanded() )
    {
        SetItemExpansion( iItem, true );

        for( int i = 0; i < iItem.Get()->mChildren.Num(); i++ )
        {
            ExpandTree( iItem.Get()->mChildren[i] );
        }
    }
}

/*
void
SOdysseyPainterEditorVectorSceneTreeView::Private_SelectRangeFromCurrentTo ( TSharedPtr<FVectorSceneTreeViewItem> iItem )
{
    if( RangeSelectionStart )
    {
        FOdysseyVectorObject* fromObject = RangeSelectionStart.Get()->GetVectorObject();
        FOdysseyVectorObject* toObject = iItem.Get()->GetVectorObject();
        FOdysseyVectorObject* vectorObject = mRootItem.Get()->GetVectorObject();
        FOdysseyVectorCell* vectorCell = vectorObject->GetCell();
        bool doSelect = false;

        for( const TSharedPtr<FVectorSceneTreeViewItem>& rangeItem : GetItems() )
        {
            FOdysseyVectorObject* rangeItemObject = rangeItem.Get()->GetVectorObject();

            if( ( rangeItemObject == fromObject ) || ( rangeItemObject == toObject ) )
            {
                if( rangeItemObject->IsSelected() == false )
                {
                    vectorCell->SelectObject( rangeItemObject );
                    // Keep internal array consistent for use by other methods
                    SelectedItems.Add( rangeItem );
                }

                doSelect = !doSelect;
            }
            else
            {
                if( doSelect )
                {
                    if( rangeItemObject->IsSelected() == false )
                    {
                        vectorCell->SelectObject( rangeItemObject );
                        // Keep internal array consistent for use by other methods
                        SelectedItems.Add( rangeItem );
                    }
                }
            }
        }
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::Private_SetItemSelection ( TSharedPtr<FVectorSceneTreeViewItem> iItem
                                                                   , bool bShouldBeSelected
                                                                   , bool bWasUserDirected )
{
    FOdysseyVectorObject* vectorObject = iItem.Get()->GetVectorObject();
    FOdysseyVectorCell* vectorCell = vectorObject->GetCell();

    if( bShouldBeSelected )
    {
        vectorCell->SelectObject( vectorObject );
        // Keep internal array consistent for use by other methods
        SelectedItems.Add( iItem );

        RangeSelectionStart = iItem;
        SelectorItem = iItem;
    }
    else
    {
        vectorCell->UnselectObject( vectorObject );
        // Keep internal array consistent for use by other methods
        SelectedItems.Remove( iItem );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::Private_ClearSelection()
{
    if( mRootItem )
    {
        // the scene
        FOdysseyVectorObject* vectorObject = mRootItem.Get()->GetVectorObject();
        FOdysseyVectorCell* vectorCell = vectorObject->GetCell();

        vectorCell->ClearObjectSelection();
    }

    // Keep internal array consistent for use by other methods
    SelectedItems.Empty();
}
*/

bool
SOdysseyPainterEditorVectorSceneTreeView::Private_IsItemSelected( const TSharedPtr<FVectorSceneTreeViewItem>& iItem )  const
{
    return iItem.Get()->GetVectorObject()->IsSelected();
}

void
SOdysseyPainterEditorVectorSceneTreeView::Update( FOdysseyVectorGroupPaint* iScene )
{
    uint64 hudFlags = mEditor->GetVectorHUDFlags();

    mItemsSource.Empty();

    RequestTreeRefresh();

    if( iScene )
    {
        //if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
        {
            int32 sceneCellIndex = iScene->GetCell()->GetIndex();
            FOdysseyVectorLayer* sharedEnv = iScene->GetLayer();
            if (!sharedEnv)
                return;

            for( FOdysseyVectorTag* tag : sharedEnv->GetSharedTagList() )
            {
                if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                    int32 sourceCellIndex = inbetweenerTag->GetSourceCellIndex();
                    int32 targetCellIndex = inbetweenerTag->GetTargetCellIndex();

                    if ((sceneCellIndex > sourceCellIndex) && ( sceneCellIndex <= targetCellIndex ) )
                    {
                        FOdysseyVectorObject* owner = inbetweenerTag->GetOwner();

                        mItemsSource.Add( MakeShareable(new FVectorSceneTreeViewItem( owner, false ) ) );
                    }
                }
            }
        }

        mRootItem = MakeShareable(new FVectorSceneTreeViewItem(iScene, true ));

        BuildTree( mRootItem );

        mItemsSource.Add( mRootItem );

        RequestTreeRefresh();

        // Expand items if need
        ExpandTree( mRootItem );
        // Select items if needed
        SelectedItems.Empty();
        //SelectTree( mRootItem );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnGetChildren( TSharedPtr<FVectorSceneTreeViewItem> iParent
                                                       , TArray<TSharedPtr<FVectorSceneTreeViewItem>>& oChildren) const
{
    oChildren = iParent.Get()->mChildren;
}

TSharedRef<ITableRow>
SOdysseyPainterEditorVectorSceneTreeView::OnGenerateRow( TSharedPtr<FVectorSceneTreeViewItem> iItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    return SNew( SOdysseyPainterEditorVectorSceneTreeViewRow, iOwnerTable, iItem );
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnExpansionChanged( TSharedPtr<FVectorSceneTreeViewItem> iItem, bool mExpanded )
{
    FOdysseyVectorObject* expandedObject = iItem.Get()->GetVectorObject();

    expandedObject->SetExpanded( mExpanded );

    // Reselect
    //SelectedItems.Empty();
    //SelectTree( mRootItem );
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnSelectionChanged( TSharedPtr<FVectorSceneTreeViewItem> iItem
                                                            , ESelectInfo::Type SelectInfo )
{
    uint64 retFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                    | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                    | FOdysseyPainterEditor::UI_UPDATE_HUD;

    if( mRootItem && ( SelectInfo != ESelectInfo::Type::Direct ) )
    {
        FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.selection-changed","Selection Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( scene->GetLayer()
                                                                         , scene->GetCell()
                                                                         , retFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        scene->GetCell()->ClearObjectSelection();

        // iTtem is null when selection is empty
        if( iItem )
        {
            TArray<TSharedPtr<FVectorSceneTreeViewItem>> selectedItems = GetSelectedItems();

            for( int i = 0; i < selectedItems.Num(); i++ )
            {
                FOdysseyVectorObject* selectedObject = selectedItems[i].Get()->GetVectorObject();

                scene->GetCell()->SelectObject( selectedObject );
            }
        }

        scene->GetCell()->ResetHUD();

        scene->GetLayer()->RequestRedraw( scene->GetCell(), 0 );

        FOdysseyVectorEngine::Notify( scene, retFlags );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::SelectAll()
{
    if( mRootItem )
    {
        FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

        FOdysseyPainterEditor::SelectAllObjects( mEditor, scene );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::DeleteObjects()
{
    if( mRootItem )
    {
        FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

        FOdysseyPainterEditor::DeleteObjects( mEditor, scene );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::CutObjects()
{
    SOdysseyPainterEditorVectorSceneTreeView::CopyObjects();
    SOdysseyPainterEditorVectorSceneTreeView::DeleteObjects();
}

void
SOdysseyPainterEditorVectorSceneTreeView::CopyObjects()
{
    if( mRootItem )
    {
        FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

        FOdysseyPainterEditor::CopyObjects( scene  );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::PasteObjects()
{
    if( mRootItem )
    {
        FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

        FOdysseyPainterEditor::PasteObjects( mEditor, scene );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::MapActionsToCommandList()
{
    mCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw( this, &SOdysseyPainterEditorVectorSceneTreeView::SelectAll )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw( this, &SOdysseyPainterEditorVectorSceneTreeView::DeleteObjects )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateRaw( this, &SOdysseyPainterEditorVectorSceneTreeView::CutObjects )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw( this, &SOdysseyPainterEditorVectorSceneTreeView::CopyObjects )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw( this, &SOdysseyPainterEditorVectorSceneTreeView::PasteObjects )
    );
}

#undef LOCTEXT_NAMESPACE
