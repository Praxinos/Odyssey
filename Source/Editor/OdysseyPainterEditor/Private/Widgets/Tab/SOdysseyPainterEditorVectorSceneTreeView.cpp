// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"
#include "OdysseyStyleSet.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyVector.h"

#define LOCTEXT_NAMESPACE "SOdysseyPainterEditorVectorSceneTreeView"


FVectorSceneTreeViewItem::~FVectorSceneTreeViewItem()
{
}

FVectorSceneTreeViewItem::FVectorSceneTreeViewItem(FOdysseyVectorObject* iVectorObject)
{
    mVectorObject = iVectorObject;
}

FOdysseyVectorObject*
FVectorSceneTreeViewItem::GetVectorObject()
{
    return mVectorObject;
}

SOdysseyPainterEditorVectorSceneTreeView::~SOdysseyPainterEditorVectorSceneTreeView()
{
}

SOdysseyPainterEditorVectorSceneTreeView::SOdysseyPainterEditorVectorSceneTreeView()
    : mCommandList(MakeShared<FUICommandList>())
{
    MapActionsToCommandList();
}

void SOdysseyPainterEditorVectorSceneTreeView::Construct( const FArguments& InArgs )
{
    STreeView<TSharedPtr<FVectorSceneTreeViewItem>>::Construct(
        STreeView<TSharedPtr<FVectorSceneTreeViewItem>>::FArguments()
        .TreeItemsSource(&mItemsSource)
        .OnGenerateRow( this, &SOdysseyPainterEditorVectorSceneTreeView::OnGenerateRow ) 
        .OnGetChildren( this, &SOdysseyPainterEditorVectorSceneTreeView::OnGetChildren )
        //.OnExpansionChanged( this, &SOdysseyLayerStackTreeView::OnExpansionChanged )
        .OnSelectionChanged( this, &SOdysseyPainterEditorVectorSceneTreeView::OnSelectionChanged )
        //.OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        //.OnContextMenuOpening( this, &SOdysseyLayerStackTreeView::OnContextMenuOpening )
        .SelectionMode( ESelectionMode::Multi )
        //.HeaderRow(headerRow)
    );
}

FReply
SOdysseyPainterEditorVectorSceneTreeView::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
	if (mCommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return STreeView<TSharedPtr<FVectorSceneTreeViewItem>>::OnKeyDown( iGeometry, iKeyEvent );
}

FReply
SOdysseyPainterEditorVectorSceneTreeView::OnDragOver( const FGeometry& MyGeometry
                                                    , const FDragDropEvent& DragDropEvent )
{
	return FReply::Handled();
}

void
SOdysseyPainterEditorVectorSceneTreeView::BuildTree( const TSharedPtr<FVectorSceneTreeViewItem> iParent )
{
    std::list<FOdysseyVectorObject*>& childrenList = iParent.Get()->GetVectorObject()->GetChildrenList();

    iParent.Get()->mChildren.Empty();

    for( FOdysseyVectorObject* child : childrenList )
    {
        if( child->GetClass() != FOdysseyVectorPathBuilder::StaticClass() )
        {
            TSharedPtr<FVectorSceneTreeViewItem> childItem = MakeShareable(new FVectorSceneTreeViewItem(child));

            SetItemExpansion( childItem, true );

    //SetItemSelection( childItem, true, ESelectInfo::Type::Direct);

            iParent.Get()->mChildren.Add( childItem );

            BuildTree( childItem );
        }
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::Update( FOdysseyVectorScene* iScene )
{
    mItemsSource.Empty();
    mItemsSource.Add( MakeShareable(new FVectorSceneTreeViewItem(iScene)) );

    //SetTreeItemsSource( &mItemsSource );
    BuildTree( mItemsSource[0] );

    RequestTreeRefresh();
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
    return SNew( STableRow<TSharedPtr<FVectorSceneTreeViewItem>>, iOwnerTable )
    [
        SNew(STextBlock)
        .Text( FText::FromString( iItem.Get()->GetVectorObject()->GetName() ) )
        //.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
        //.ColorAndOpacity(FLinearColor(1,0,1,1))
        //.ShadowColorAndOpacity(FLinearColor::Black)
        //.ShadowOffset(FIntPoint(-2, 2))
    ];
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnSelectionChanged( TSharedPtr<FVectorSceneTreeViewItem> iItem, ESelectInfo::Type SelectInfo )
{
    if( iItem == nullptr ) return;

    FOdysseyVectorObject* iSelectedObject = iItem.Get()->GetVectorObject();
    FOdysseyVectorScene* scene = iSelectedObject->GetScene();

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorSceneTreeView","Selection Changed"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelect( scene );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    // deselect all if control key is not pressed
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
    {
        scene->ClearSelection();
    }

    scene->Select( iSelectedObject );

    scene->GetEngine()->ResetHUD();

    scene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                              | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
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

void
SOdysseyPainterEditorVectorSceneTreeView::DeleteObjectSelection()
{
    if( mItemsSource.Num() )
    {
        TSharedPtr<FVectorSceneTreeViewItem> rootItem = mItemsSource[0];
        FOdysseyVectorScene* scene = static_cast<FOdysseyVectorScene*>(rootItem.Get()->GetVectorObject());

        FOdysseyPainterEditor::DeleteObjectSelection( scene->GetEngine(), scene );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::CopyObjectSelection()
{
    if( mItemsSource.Num() )
    {
        TSharedPtr<FVectorSceneTreeViewItem> rootItem = mItemsSource[0];
        FOdysseyVectorScene* scene = static_cast<FOdysseyVectorScene*>(rootItem.Get()->GetVectorObject());

        FOdysseyPainterEditor::CopyObjectSelection( scene->GetEngine(), scene );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::PasteObjectSelection()
{
    if( mItemsSource.Num() )
    {
        TSharedPtr<FVectorSceneTreeViewItem> rootItem = mItemsSource[0];
        FOdysseyVectorScene* scene = static_cast<FOdysseyVectorScene*>(rootItem.Get()->GetVectorObject());

        FOdysseyPainterEditor::PasteObjectSelection( scene->GetEngine(), scene );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::MapActionsToCommandList()
{
    mCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw(this, &SOdysseyPainterEditorVectorSceneTreeView::SelectAll)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(this, &SOdysseyPainterEditorVectorSceneTreeView::DeleteObjectSelection)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw(this, &SOdysseyPainterEditorVectorSceneTreeView::CopyObjectSelection)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw(this, &SOdysseyPainterEditorVectorSceneTreeView::PasteObjectSelection)
    );
/*
    mCommandList->MapAction(
        FGenericCommands::Get().Rename,
        FExecuteAction::CreateRaw(this, &SOdysseyPainterEditorVectorSceneTreeView::RenameCurrentLayer)
    );
*/
}
