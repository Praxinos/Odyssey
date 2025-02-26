// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

// From module OdysseyAnimationEditor
#include "Widgets/Animation/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweening.h"
#include "Widgets/Animation/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow.h"
#include "Widgets/Animation/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
// From module OdysseyPainterEditor
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "PainterEditor/OdysseyPainterEditor.h"
// From U.E
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Math/UnitConversion.h"
// From module OdysseyStyle
#include "OdysseyStyle.h"
// From module OdysseyVector
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorTagInbetweener.h"
#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAdd.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownRemove.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility.h"
#include "OdysseyAnimationEditorTimelinePosition.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"

#include "Framework/Commands/GenericCommands.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerImageVectorTimelineInbetweening::~SOdysseyAnimationLayerImageVectorTimelineInbetweening()
{
    FOdysseyVectorEngine::OnNotifyDelegate().RemoveAll( this );
}

SOdysseyAnimationLayerImageVectorTimelineInbetweening::SOdysseyAnimationLayerImageVectorTimelineInbetweening()
    : mForwardArrowBrush( FOdysseyStyle::GetBrush( "Animation.Timeline.Inbetweening.Forward16") )
    , mBackwardArrowBrush( FOdysseyStyle::GetBrush( "Animation.Timeline.Inbetweening.Backward16") )
    , mCommandList(MakeShared<FUICommandList>())
{
    MapActionsToCommandList();
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::Construct( const FArguments& InArgs
                                                                , UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector )
{
    mAnimationLayerImageVector = iAnimationLayerImageVector;
    mTimelinePosition = InArgs._TimelinePosition;
    mEditor = InArgs._PainterEditor;

    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    FOdysseyVectorEngine::OnNotifyDelegate().AddRaw( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnVectorSceneNotify );

    SListView<TSharedPtr<FInbetweeningListViewItem>>::Construct(
        SListView<TSharedPtr<FInbetweeningListViewItem>>::FArguments()
        // for some reason, SetItemsSource does not work, so we have to use an array that we
        // call mItemsSource and that we will updates with the desired items
        .ListItemsSource(&mItemsSource)
        .OnGenerateRow( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnGenerateRow )
        //.OnGetChildren( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnGetChildren )
        .OnSelectionChanged( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnSelectionChanged )
        //.OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnContextMenuOpening )
        .SelectionMode( ESelectionMode::Multi )
        //.HeaderRow(headerRow)
    );

    Update();
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnVectorSceneNotify( FOdysseyVectorGroupPaint* iScene, uint64 iNotificationFlags )
{
    if( iNotificationFlags & FOdysseyPainterEditor::UI_UPDATE_TIMELINE )
    {
        Update();
    }
}

FOdysseyPainterEditor*
SOdysseyAnimationLayerImageVectorTimelineInbetweening::GetEditor() const
{
    return mEditor.Get();
}

TSharedPtr<FOdysseyAnimationEditorTimelinePosition>
SOdysseyAnimationLayerImageVectorTimelineInbetweening::GetTimelinePosition() const
{
    return mTimelinePosition;
}

FReply
SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnKeyDown( const FGeometry& iGeometry
                                                                , const FKeyEvent& iKeyEvent )
{
    if (mCommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return SListView<TSharedPtr<FInbetweeningListViewItem>>::OnKeyDown( iGeometry, iKeyEvent );
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::Update()
{
    mItemsSource.Reset();

    for( FOdysseyVectorTag* tag : mAnimationLayerImageVector->GetVectorLayer()->GetSharedTagList() )
    {
        if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>( tag );

            mItemsSource.Insert( MakeShareable( new FInbetweeningListViewItem(inbetweenerTag) ), 0 );
        }
    }

    SelectedItems.Empty();

    RequestListRefresh();
}

/*
void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::Private_SelectRangeFromCurrentTo ( TSharedPtr<FInbetweeningListViewItem> iItem )
{
    if( RangeSelectionStart )
    {
        FOdysseyVectorObject* fromObject = RangeSelectionStart.Get()->GetInbetweenerTag()->GetOwner();
        FOdysseyVectorObject* toObject = iItem.Get()->GetInbetweenerTag()->GetOwner();
        FOdysseyVectorObject* vectorObject = mItemsSource[0].Get()->GetInbetweenerTag()->GetOwner();
        FOdysseyVectorCell* vectorCell = fromObject->GetCell();
        bool doSelect = false;

        for( const TSharedPtr<FInbetweeningListViewItem>& rangeItem : GetItems() )
        {
            FOdysseyVectorObject* rangeItemObject = rangeItem.Get()->GetInbetweenerTag()->GetOwner();

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
SOdysseyAnimationLayerImageVectorTimelineInbetweening::Private_SetItemSelection ( TSharedPtr<FInbetweeningListViewItem> iItem
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
        vectorRoot->UnselectObject( vectorObject );
        // Keep internal array consistent for use by other methods
        SelectedItems.Remove( iItem );
    }
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::Private_ClearSelection()
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
SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnSelectionChanged( TSharedPtr<FInbetweeningListViewItem> iItem
                                                                         , ESelectInfo::Type SelectInfo )
{
    std::list<FOdysseyVectorCell*> cellList;
    uint64 retFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                    | FOdysseyPainterEditor::UI_UPDATE_HUD;

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
SOdysseyAnimationLayerImageVectorTimelineInbetweening::Private_IsItemSelected( const TSharedPtr<FInbetweeningListViewItem>& iItem )  const
{
    UOdysseyLayerStack* layerStack = mAnimationLayerImageVector->GetLayerStack();

    return iItem.Get()->GetInbetweenerTag()->GetOwner()->IsSelected() && ( layerStack->CurrentLayer == mAnimationLayerImageVector );
}

UOdysseyAnimationLayerImageVector*
SOdysseyAnimationLayerImageVectorTimelineInbetweening::GetAnimationLayerImageVector()
{
    return mAnimationLayerImageVector;
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::SetCursorPos( FVector2D iCursorPos )
{
    mCursorPos = iCursorPos;
}

TSharedRef<ITableRow>
SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                                                    , const TSharedRef<STableViewBase>& iOwnerTable )
{
   return SNew( SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow, iOwnerTable, iItem );
}

TSharedPtr<SWidget>
SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnContextMenuOpening()
{
    FMenuBuilder menu( true, nullptr );

    menu.AddMenuEntry( LOCTEXT("vector-tool.inbetweening-context-menu.add-breakdown.name", "Add Breakdown")
                     , LOCTEXT("vector-tool.inbetweening-context-menu.add-breakdown.tooltip", "Add Breakdown")
                     , FSlateIcon()
                     , FUIAction(FExecuteAction::CreateSP( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::AddBreakdown )));

    menu.AddMenuEntry( LOCTEXT("vector-tool.inbetweening-context-menu.remove-breakdown.name", "Remove Breakdown")
                     , LOCTEXT("vector-tool.inbetweening-context-menu.remove-breakdown.tooltip", "Remove Breakdown")
                     , FSlateIcon()
                     , FUIAction(FExecuteAction::CreateSP( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::RemoveBreakdown )));

    menu.AddMenuEntry( LOCTEXT("vector-tool.inbetweening-context-menu.show-hide-target.name", "Show/Hide Target")
                     , LOCTEXT("vector-tool.inbetweening-context-menu.show-hide-target.tooltip", "Show/Hide Target")
                     , FSlateIcon()
                     , FUIAction(FExecuteAction::CreateSP( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::ShowHideTarget )));

    menu.AddMenuEntry( LOCTEXT("vector-tool.inbetweening-context-menu.remove-breakdown.name", "Change direction")
                     , LOCTEXT("vector-tool.inbetweening-context-menu.remove-breakdown.tooltip", "Change direction")
                     , FSlateIcon()
                     , FUIAction(FExecuteAction::CreateSP( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::ChangeDirection )));


    return menu.MakeWidget();
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::GetSelectedInbetweenerTags( std::list<FOdysseyVectorTagInbetweener*>& oSelectedInbetweenerTagList )
{
    FOdysseyVectorLayer* vectorLayer = mAnimationLayerImageVector->GetVectorLayer();

    oSelectedInbetweenerTagList.clear();

    // I could not make it work using SListView SelectedItems methods, for some reason. So I use the SharedEnv
    for( FOdysseyVectorTag* tag : vectorLayer->GetSharedTagList() )
    {
        if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

            if( tag->GetOwner()->IsSelected() )
            {
                oSelectedInbetweenerTagList.push_back( inbetweenerTag );
            }
        }
    }
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::AddBreakdown()
{
    int breakdownFrameIndex = MousePositionToFrame( mCursorPos.X );
    UOdysseyAnimationCell* cell = mAnimationLayerImageVector->GetCellAtFrame( breakdownFrameIndex );
    if (!cell)
        return;

    int breakdownCellIndex = cell->IndexInLayer;
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;
    std::list<FOdysseyVectorEngine*> engineList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;

    GetSelectedInbetweenerTags( selectedInbetweenerTagList );

    if( selectedInbetweenerTagList.size() )
    {
        //---------- needed for undos-----------//
        GEditor->BeginTransaction(LOCTEXT("vector-timeline.transaction.add-breakdown", "Add Breakdown"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownAdd( mAnimationLayerImageVector->GetVectorLayer()
                                                                                       , selectedInbetweenerTagList
                                                                                       , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            FOdysseyPainterEditor* editor = mEditor.Get();
            if (editor)
            {
                TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
        }
        GEditor->EndTransaction();
        //--------------------------------------//

        for( FOdysseyVectorTagInbetweener* inbetweenerTag : selectedInbetweenerTagList )
        {
            uint32 tagCellIndex = inbetweenerTag->GetOwner()->GetCell()->GetCell()->GetIndex();
            int32 drawingIndex = inbetweenerTag->GetDrawingIndexFromCellIndex( breakdownCellIndex );
            FInbetweenerBreakdown* curBreakdown = inbetweenerTag->GetBreakdown( drawingIndex, true );

            //FOdysseyVectorGroupPaint* scene = inbetweenerTag->GetOwner()->GetScene();

            if( curBreakdown )
            {
                //FInbetweenerBreakdown* newbreakdown = new FInbetweenerBreakdown( inbetweenerTag );

                inbetweenerTag->AddBreakdown( nullptr/*newbreakdown*/, drawingIndex, true, true );
            }
        }

        // Updates and request redraw
        mAnimationLayerImageVector->GetVectorLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        mAnimationLayerImageVector->GetVectorLayer()->RequestRedraw( nullptr, 0 );
    }

    FOdysseyVectorEngine::Notify( nullptr, notificationFlags );
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::RemoveBreakdown()
{
    int breakdownFrameIndex = MousePositionToFrame( mCursorPos.X );
    UOdysseyAnimationCell* cell = mAnimationLayerImageVector->GetCellAtFrame( breakdownFrameIndex );
    if (!cell)
        return;

    int breakdownCellIndex = cell->IndexInLayer;
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;

    GetSelectedInbetweenerTags( selectedInbetweenerTagList );

    if( selectedInbetweenerTagList.size() )
    {
        //---------- needed for undos-----------//
        GEditor->BeginTransaction(LOCTEXT("vector-timeline.transaction.remove-breakdown", "Remove Breakdown"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownRemove( mAnimationLayerImageVector->GetVectorLayer()
                                                                                          , selectedInbetweenerTagList
                                                                                          , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            FOdysseyPainterEditor* editor = mEditor.Get();
            if (editor)
            {
                TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
        }
        GEditor->EndTransaction();
        //--------------------------------------//

        for( FOdysseyVectorTagInbetweener* inbetweenerTag : selectedInbetweenerTagList )
        {
            uint32 tagCellIndex = inbetweenerTag->GetOwner()->GetCell()->GetCell()->GetIndex();
            int32 drawingIndex = inbetweenerTag->GetDrawingIndexFromCellIndex( breakdownCellIndex );
            FInbetweenerBreakdown* breakdown = inbetweenerTag->GetBreakdown( drawingIndex, false );

            if( inbetweenerTag->GetBreakdownList().size() > 1 )
            {
                if( breakdown )
                {
                    inbetweenerTag->RemoveBreakdown( breakdown, false );
                }
            }
        }
        // Updates and request redraw
        mAnimationLayerImageVector->GetVectorLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        mAnimationLayerImageVector->GetVectorLayer()->RequestRedraw( nullptr, 0 );
    }

    // static call
    FOdysseyVectorEngine::Notify( nullptr, notificationFlags );
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::ShowHideTarget()
{
    int breakdownFrameIndex = MousePositionToFrame( mCursorPos.X );
    UOdysseyAnimationCell* cell = mAnimationLayerImageVector->GetCellAtFrame( breakdownFrameIndex );
    if (!cell)
        return;

    int breakdownCellIndex = cell->IndexInLayer;
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;
    std::list<FInbetweenerBreakdown*> breakdownList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;

    GetSelectedInbetweenerTags( selectedInbetweenerTagList );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : selectedInbetweenerTagList )
    {
        uint32 tagCellIndex = inbetweenerTag->GetOwner()->GetCell()->GetCell()->GetIndex();
        int32 drawingIndex = inbetweenerTag->GetDrawingIndexFromCellIndex( breakdownCellIndex );
        FInbetweenerBreakdown* breakdown = inbetweenerTag->GetBreakdown( drawingIndex, false );

        if( breakdown )
        {
            breakdownList.push_back( breakdown );
        }
    }

    if( breakdownList.size() )
    {
        //---------- needed for undos-----------//
        GEditor->BeginTransaction(LOCTEXT("vector-timeline.transaction.remove-breakdown", "Remove Breakdown"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility( mAnimationLayerImageVector->GetVectorLayer()
                                                                                                    , breakdownList
                                                                                                    , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            FOdysseyPainterEditor* editor = mEditor.Get();
            if (editor)
            {
                TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
        }
        GEditor->EndTransaction();
        //--------------------------------------//

        for( FInbetweenerBreakdown* breakdown : breakdownList )
        {
            breakdown->SetTargetVisibility( !breakdown->IsTargetVisible() );
            // request redraw
            //breakdown->GetTargetCell()->GetEngine()->Invalidate(0);
        }
        // Updates and request redraw
        mAnimationLayerImageVector->GetVectorLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        mAnimationLayerImageVector->GetVectorLayer()->RequestRedraw( nullptr, 0 );
    }

    // static call
    FOdysseyVectorEngine::Notify( nullptr, notificationFlags );
}

const FSlateBrush *
SOdysseyAnimationLayerImageVectorTimelineInbetweening::GetForwardArrowBrush()
{
    return mForwardArrowBrush;
}

const FSlateBrush *
SOdysseyAnimationLayerImageVectorTimelineInbetweening::GetBackwardArrowBrush()
{
    return mBackwardArrowBrush;
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::ChangeDirection()
{
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    GetSelectedInbetweenerTags( selectedInbetweenerTagList );

    //---------- needed for undos-----------//
/*
    GEditor->BeginTransaction(LOCTEXT("vector-timeline.transaction.change-diretion", "Change Direction"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownRemove( nullptr
                                                                                      , selectedInbetweenerTagList
                                                                                      , engineList
                                                                                      , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        if (mEditor)
        {
            TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
    }
    GEditor->EndTransaction();
*/
    //--------------------------------------//

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : selectedInbetweenerTagList )
    {
        switch( inbetweenerTag->GetInterpolationDirection() )
        {
            case eInbetweenerInterpolationDirection::Forward :
                inbetweenerTag->SetInterpolationDirection( eInbetweenerInterpolationDirection::Backward );
            break;

            case eInbetweenerInterpolationDirection::Backward :
                inbetweenerTag->SetInterpolationDirection( eInbetweenerInterpolationDirection::Forward );
            break;
        }
    }

    // Updates and request redraw
    mAnimationLayerImageVector->GetVectorLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    mAnimationLayerImageVector->GetVectorLayer()->RequestRedraw( nullptr, 0 );

    // static call
    FOdysseyVectorEngine::Notify( nullptr, notificationFlags );
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::MapActionsToCommandList()
{
    mCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::RemoveInbetweenerTag )
    );
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::RemoveInbetweenerTag()
{
    FOdysseyPainterEditor* editor = mEditor.Get();

    // note: editor is NULL in the Sequencer
    FOdysseyPainterEditor::RemoveInbetweenerTag( editor, mAnimationLayerImageVector->GetVectorLayer() );
}

float
SOdysseyAnimationLayerImageVectorTimelineInbetweening::MousePositionToFrame(float iX) const
{
    return iX / mTimelinePosition->GetFrameSize();
}

float
SOdysseyAnimationLayerImageVectorTimelineInbetweening::FrameToMousePosition(float iFrame) const
{
    return iFrame * mTimelinePosition->GetFrameSize();
}

#undef LOCTEXT_NAMESPACE
