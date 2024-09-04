// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeViewRow.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeViewContextMenu.h"
#include "OdysseyStyleSet.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyVector.h"
#include "OdysseyPainterEditor.h"
#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"

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
SOdysseyPainterEditorVectorSceneTreeView::Construct( const FArguments& InArgs, FOdysseyPainterEditor* iEditor)
{
    mEditor = iEditor;
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
        .SelectionMode( ESelectionMode::Multi )
        //.HeaderRow(headerRow)
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
        TSharedPtr<FVectorSceneTreeViewItem> childItem = MakeShareable(new FVectorSceneTreeViewItem(child));

        //iItem.Get()->mChildren.Add( childItem );
        // reverse order in order to get the most forward objet on top of the hierarchy 
        iItem.Get()->mChildren.Insert( childItem, 0 );

        BuildTree( childItem );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::SelectTree( const TSharedPtr<FVectorSceneTreeViewItem> iItem )
{
    FOdysseyVectorObject* itemObject = iItem.Get()->GetVectorObject();

    if( itemObject->IsSelected() )
    {
        if( IsItemSelected( iItem ) == false )
        {
            SelectedItems.Add( iItem );
        }
    }

    for( int i = 0; i < iItem.Get()->mChildren.Num(); i++ )
    {
        SelectTree( iItem.Get()->mChildren[i] );
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

void
SOdysseyPainterEditorVectorSceneTreeView::Update( FOdysseyVectorGroupPaint* iScene )
{
    mItemsSource.Empty();

    RequestTreeRefresh();

    if( iScene )
    {
        mRootItem = MakeShareable(new FVectorSceneTreeViewItem(iScene));

        BuildTree( mRootItem );

        mItemsSource.Add( mRootItem );

        RequestTreeRefresh();

        // Expand items if need
        ExpandTree( mRootItem );
        // Select items if needed
        SelectedItems.Empty();
        SelectTree( mRootItem );
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
    SelectedItems.Empty();
    SelectTree( mRootItem );
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnSelectionChanged( TSharedPtr<FVectorSceneTreeViewItem> iItem
                                                            , ESelectInfo::Type SelectInfo )
{
    uint64 retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                    | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                    | FOdysseyPainterEditor::UI_UPDATE_TIMELINE;

    if( mRootItem && ( SelectInfo != ESelectInfo::Type::Direct ) )
    {
        FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.selection-changed","Selection Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( scene, retFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
            TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        scene->GetEngine()->ClearObjectSelection();

        // iTtem is null when selection is empty
        if( iItem )
        {
            TArray<TSharedPtr<FVectorSceneTreeViewItem>> selectedItems = GetSelectedItems();

            // no need to create an undo record or do anything if the selection is empty
            if( selectedItems.Num() )
            {
                for( int i = 0; i < selectedItems.Num(); i++ )
                {
                    FOdysseyVectorObject* selectedObject = selectedItems[i].Get()->GetVectorObject();

                    scene->GetEngine()->SelectObject( selectedObject );
                }
            }
        }

        scene->GetEngine()->ResetHUD();

        scene->GetEngine()->Signal( retFlags );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::SelectAll()
{
    TArray<TSharedPtr<FVectorSceneTreeViewItem>> selectedItems = GetSelectedItems();
/*
    for( int i = 0; selectedItems.Num(); i++ )
    {
    }
*/
}

//FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

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
/*
    mCommandList->MapAction(
        FGenericCommands::Get().Rename,
        FExecuteAction::CreateRaw(this, &SOdysseyPainterEditorVectorSceneTreeView::RenameCurrentLayer)
    );
*/
}

#undef LOCTEXT_NAMESPACE
