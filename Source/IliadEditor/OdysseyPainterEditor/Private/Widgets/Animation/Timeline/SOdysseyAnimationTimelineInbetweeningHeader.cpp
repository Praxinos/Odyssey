// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineInbetweeningHeader.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayerImageVector.h"

#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Math/UnitConversion.h"
#include "OdysseyStyle.h"

// from module OdysseyPainterEditor
#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorModule.h"

// from module OdysseyVector
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "Undo/OdysseyVectorUndoTagRemove.h"
#include "Undo/OdysseyVectorUndoSelectObject.h"

#include "Framework/Commands/GenericCommands.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationTimelineInbetweeningHeader::~SOdysseyAnimationTimelineInbetweeningHeader()
{
    FOdysseyVectorEngine::OnNotifyDelegate().RemoveAll( this );
}

SOdysseyAnimationTimelineInbetweeningHeader::SOdysseyAnimationTimelineInbetweeningHeader()
    : mCommandList(MakeShared<FUICommandList>())
{
    MapActionsToCommandList();
}

void
SOdysseyAnimationTimelineInbetweeningHeader::Construct( const FArguments& InArgs
                                                      , UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector )
{
    static FTableViewStyle style = FOdysseyStyle::GetWidgetStyle<FTableViewStyle>("Inbetweening.TableView");

    mAnimationLayerImageVector = iAnimationLayerImageVector;
    mOnTransactCurrentFrame = InArgs._OnTransactCurrentFrame;

    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    FOdysseyVectorEngine::OnNotifyDelegate().AddRaw( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnVectorSceneNotify );

    SListView<TSharedPtr<FInbetweeningListViewItem>>::Construct(
        SListView<TSharedPtr<FInbetweeningListViewItem>>::FArguments()
        // for some reason, SetItemsSource does not work, so we have to use an array that we
        // call mItemsSource and that we will updates with the desired items
        .ListItemsSource(&mItemsSource)
        .OnGenerateRow( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnGenerateRow )
        //.OnGetChildren( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnGetChildren )
        .OnSelectionChanged( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnSelectionChanged )
        //.OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnContextMenuOpening )
        .SelectionMode( ESelectionMode::Multi )
        //.HeaderRow(headerRow)
    );

    SetStyle( &style );

    Update();
}

void
SOdysseyAnimationTimelineInbetweeningHeader::OnVectorSceneNotify( FOdysseyVectorGroupPaint* iScene, uint64 iNotificationFlags )
{
    if( iNotificationFlags & FOdysseyPainterEditor::UI_UPDATE_TIMELINE )
    {
        Update();
    }
}

FReply
SOdysseyAnimationTimelineInbetweeningHeader::OnKeyDown( const FGeometry& iGeometry
                                                      , const FKeyEvent& iKeyEvent )
{
    if (mCommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return SListView<TSharedPtr<FInbetweeningListViewItem>>::OnKeyDown( iGeometry, iKeyEvent );
}

FReply
SOdysseyAnimationTimelineInbetweeningHeader::OnKeyUp ( const FGeometry& MyGeometry
                                                        , const FKeyEvent& InKeyEvent )
{
    return FReply::Unhandled();
}

void
SOdysseyAnimationTimelineInbetweeningHeader::Update()
{
    mItemsSource.Reset();

    mAnimationLayerImageVector->GetVectorLayer()->GetSharedTagMutex().lock();
    for( FOdysseyVectorTag* tag : mAnimationLayerImageVector->GetVectorLayer()->GetSharedTagList() )
    {
        if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>( tag );

            mItemsSource.Insert( MakeShareable( new FInbetweeningListViewItem(inbetweenerTag) ), 0 );
        }
    }
    mAnimationLayerImageVector->GetVectorLayer()->GetSharedTagMutex().unlock();

    SelectedItems.Empty();

    RequestListRefresh();
}

/*
void
SOdysseyAnimationTimelineInbetweeningHeader::Private_SelectRangeFromCurrentTo ( TSharedPtr<FInbetweeningListViewItem> iItem )
{
    if( RangeSelectionStart )
    {
        FOdysseyVectorObject* fromObject = RangeSelectionStart.Get()->GetInbetweenerTag()->GetOwner();
        FOdysseyVectorObject* toObject = iItem.Get()->GetInbetweenerTag()->GetOwner();
        FOdysseyVectorObject* vectorObject = mItemsSource[0].Get()->GetInbetweenerTag()->GetOwner();
        FOdysseyVectorCell* vectorRoot = fromObject->GetCell();
        bool doSelect = false;

        for( const TSharedPtr<FInbetweeningListViewItem>& rangeItem : GetItems() )
        {
            FOdysseyVectorObject* rangeItemObject = rangeItem.Get()->GetInbetweenerTag()->GetOwner();

            if( ( rangeItemObject == fromObject ) || ( rangeItemObject == toObject ) )
            {
                if( rangeItemObject->IsSelected() == false )
                {
                    vectorRoot->SelectObject( rangeItemObject );
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
                        vectorRoot->SelectObject( rangeItemObject );
                        // Keep internal array consistent for use by other methods
                        SelectedItems.Add( rangeItem );
                    }
                }
            }
        }
    }
}

void
SOdysseyAnimationTimelineInbetweeningHeader::Private_SetItemSelection ( TSharedPtr<FInbetweeningListViewItem> iItem
                                                                      , bool bShouldBeSelected
                                                                      , bool bWasUserDirected )
{
    FOdysseyVectorObject* vectorObject = iItem.Get()->GetInbetweenerTag()->GetOwner();
    FOdysseyVectorCell* vectorRoot = vectorObject->GetCell();

    if( bShouldBeSelected )
    {
        vectorRoot->SelectObject( vectorObject );
        // Keep internal array consistent for use by other methods
        SelectedItems.Add( iItem );

        RangeSelectionStart = iItem;
    }
    else
    {
        // Keep internal array consistent for use by other methods
        SelectedItems.Remove( iItem );

        vectorRoot->UnselectObject( vectorObject );
    }
}

void
SOdysseyAnimationTimelineInbetweeningHeader::Private_ClearSelection()
{
    if( mItemsSource.Num() )
    {
        // the scene
        FOdysseyVectorObject* vectorObject = mItemsSource[0].Get()->GetInbetweenerTag()->GetOwner();
        FOdysseyVectorCell* vectorRoot = vectorObject->GetCell();

        vectorRoot->ClearObjectSelection();
    }

    // Keep internal array consistent for use by other methods
    SelectedItems.Empty();
}
*/

void
SOdysseyAnimationTimelineInbetweeningHeader::OnSelectionChanged( TSharedPtr<FInbetweeningListViewItem> iItem
                                                               , ESelectInfo::Type SelectInfo )
{
    std::list<FOdysseyVectorCell*> cellList;
    uint64 retFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                    | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    if( SelectInfo != ESelectInfo::Type::Direct )
    {
        for( TSharedPtr<FInbetweeningListViewItem> item : mItemsSource )
        {
            FOdysseyVectorCell* cell = item->GetInbetweenerTag()->GetOwner()->GetCell();

            if( std::find( cellList.begin(), cellList.end(), cell ) == cellList.end() )
            {
                cellList.push_back( cell );
            }
        }

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.selection-changed","Selection Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( mAnimationLayerImageVector->GetVectorLayer()
                                                                         , cellList
                                                                         , retFlags );

            GUndo->StoreUndo( mAnimationLayerImageVector, TUniquePtr<FOdysseyVectorUndo>(undo) );

            //TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
            //if (source)
                //source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        for( FOdysseyVectorCell* cell : cellList )
        {
            cell->ClearObjectSelection();

            // mark for redraw (to force-redraw the HUD)
            mAnimationLayerImageVector->GetVectorLayer()->InvalidateCell( cell );
        }

        for( TSharedPtr<FInbetweeningListViewItem> item : mItemsSource )
        {
            TArray<TSharedPtr<FInbetweeningListViewItem>> selectedItems = GetSelectedItems();

            for( int i = 0; i < selectedItems.Num(); i++ )
            {
                FOdysseyVectorObject* selectedObject = selectedItems[i].Get()->GetInbetweenerTag()->GetOwner();

                selectedObject->GetCell()->SelectObject( selectedObject );
            }
        }

        mAnimationLayerImageVector->GetVectorLayer()->RequestRedraw( nullptr, 0 );

        FOdysseyVectorEngine::Notify( nullptr, retFlags );
    }
}

bool
SOdysseyAnimationTimelineInbetweeningHeader::Private_IsItemSelected( const TSharedPtr<FInbetweeningListViewItem>& iItem )  const
{
    UOdysseyLayerStack* layerStack = mAnimationLayerImageVector->GetLayerStack();

    return iItem.Get()->GetInbetweenerTag()->GetOwner()->IsSelected() && ( layerStack->GetCurrentLayer() == mAnimationLayerImageVector );
}

TSharedRef<ITableRow>
SOdysseyAnimationTimelineInbetweeningHeader::OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                                          , const TSharedRef<STableViewBase>& iOwnerTable )
{
    return SNew( SOdysseyAnimationTimelineInbetweeningHeaderRow, iOwnerTable, iItem )
        .OnTransactCurrentFrame(mOnTransactCurrentFrame);
}

TSharedPtr<SWidget>
SOdysseyAnimationTimelineInbetweeningHeader::OnContextMenuOpening()
{
    FMenuBuilder menu( true, nullptr );

    menu.AddMenuEntry( LOCTEXT("vector-tool.inbetweening-context-menu.remove-tag.name", "Remove Tag")
                     , LOCTEXT("vector-tool.inbetweening-context-menu.remove-tag.tooltip", "Remove Tag")
                     , FSlateIcon()
                     , FUIAction(FExecuteAction::CreateSP( this, &SOdysseyAnimationTimelineInbetweeningHeader::RemoveInbetweenerTag )));

    menu.AddMenuEntry( LOCTEXT("vector-tool.inbetweening-context-menu.add-breakdown.name", "Commit")
                     , LOCTEXT("vector-tool.inbetweening-context-menu.add-breakdown.tooltip", "Commit")
                     , FSlateIcon()
                     , FUIAction(FExecuteAction::CreateSP( this, &SOdysseyAnimationTimelineInbetweeningHeader::Commit )));

    menu.AddMenuEntry( LOCTEXT("vector-tool.inbetweening-context-menu.reset-spacing-charts.name", "Reset spacing charts")
                     , LOCTEXT("vector-tool.inbetweening-context-menu.reset-spacing-charts.tooltip", "Reset spacing charts")
                     , FSlateIcon()
                     , FUIAction(FExecuteAction::CreateSP( this, &SOdysseyAnimationTimelineInbetweeningHeader::ResetSpacingCharts )));

    return menu.MakeWidget();
}

void
SOdysseyAnimationTimelineInbetweeningHeader::Commit()
{
    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(mAnimationLayerImageVector->GetAnimation());

    // note: editor is NULL in the Sequencer
    FOdysseyPainterEditor::CommitSelectedInbetweenerTag( editor, mAnimationLayerImageVector->GetVectorLayer() );
}

void
SOdysseyAnimationTimelineInbetweeningHeader::ResetSpacingCharts()
{
    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(mAnimationLayerImageVector->GetAnimation());

    // note: editor is NULL in the Sequencer
    FOdysseyPainterEditor::ResetInbetweenerTagSpacingChart( editor, mAnimationLayerImageVector->GetVectorLayer() );
}

void
SOdysseyAnimationTimelineInbetweeningHeader::RemoveInbetweenerTag()
{
    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(mAnimationLayerImageVector->GetAnimation());

    // note: editor is NULL in the Sequencer
    FOdysseyPainterEditor::RemoveInbetweenerTag( editor, mAnimationLayerImageVector->GetVectorLayer() );
}

UOdysseyAnimationLayerImageVector*
SOdysseyAnimationTimelineInbetweeningHeader::GetAnimationLayerImageVector()
{
    return mAnimationLayerImageVector;
}

void
SOdysseyAnimationTimelineInbetweeningHeader::MapActionsToCommandList()
{
    mCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw( this, &SOdysseyAnimationTimelineInbetweeningHeader::RemoveInbetweenerTag )
    );
}

#undef LOCTEXT_NAMESPACE
