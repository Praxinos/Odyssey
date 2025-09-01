// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Animation/Timeline/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweening.h"
#include "Widgets/Animation/Timeline/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayerImageVector.h"
// From module OdysseyPainterEditor
#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorModule.h"
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
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyAnimationCell.h"

#include "Framework/Commands/GenericCommands.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerImageVectorTimelineInbetweening::~SOdysseyAnimationLayerImageVectorTimelineInbetweening()
{
    UnbindLayerDelegates( );
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
    static FTableViewStyle style = FOdysseyStyle::GetWidgetStyle<FTableViewStyle>("Inbetweening.TableView");

    mAnimationLayerImageVector = iAnimationLayerImageVector;
    mTimelinePosition = InArgs._TimelinePosition;

    BindLayerDelegates( );

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

    SetStyle( &style );

    Update();
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnVectorSceneNotify( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                                                          , uint32 iUpdateFlags )
{
    if( ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_LIST] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_LIST] ) )
    {
        Update();
    }
}

TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>
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

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnSelectionChanged( TSharedPtr<FInbetweeningListViewItem> iItem
                                                                         , ESelectInfo::Type SelectInfo )
{
    std::list<FOdysseyVectorCell*> cellList;

    UnbindLayerDelegates();

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
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( mAnimationLayerImageVector->GetVectorLayer().Get()
                                                                         , cellList );

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

        mAnimationLayerImageVector->GetVectorLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

        mAnimationLayerImageVector->GetVectorLayer()->RequestRedraw( nullptr, 0 );
    }

    BindLayerDelegates();
}

bool
SOdysseyAnimationLayerImageVectorTimelineInbetweening::Private_IsItemSelected( const TSharedPtr<FInbetweeningListViewItem>& iItem )  const
{
    UOdysseyLayerStack* layerStack = mAnimationLayerImageVector->GetLayerStack();

    return iItem.Get()->GetInbetweenerTag()->GetOwner()->IsSelected() && ( layerStack->GetCurrentLayer() == mAnimationLayerImageVector );
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

    menu.AddMenuEntry( LOCTEXT("vector-tool.inbetweening-context-menu.change-direction.name", "Change direction")
                     , LOCTEXT("vector-tool.inbetweening-context-menu.change-direction.tooltip", "Change direction")
                     , FSlateIcon()
                     , FUIAction(FExecuteAction::CreateSP( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::ChangeDirection )));


    return menu.MakeWidget();
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::GetSelectedInbetweenerTags( std::list<FOdysseyVectorTagInbetweener*>& oSelectedInbetweenerTagList )
{
    FOdysseyVectorLayer* vectorLayer = mAnimationLayerImageVector->GetVectorLayer().Get();

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
    UOdysseyLayerCell* cell = mAnimationLayerImageVector->GetCellAtFrame( breakdownFrameIndex );
    if (!cell)
        return;

    int breakdownCellIndex = cell->GetIndexInLayer();
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;
    std::list<FOdysseyVectorEngine*> engineList;

    GetSelectedInbetweenerTags( selectedInbetweenerTagList );

    if( selectedInbetweenerTagList.size() )
    {
        //---------- needed for undos-----------//
        GEditor->BeginTransaction(LOCTEXT("vector-timeline.transaction.add-breakdown", "Add Breakdown"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownAdd( mAnimationLayerImageVector->GetVectorLayer().Get()
                                                                                       , selectedInbetweenerTagList );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
            FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(mAnimationLayerImageVector->GetAnimation());
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
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::RemoveBreakdown()
{
    int breakdownFrameIndex = MousePositionToFrame( mCursorPos.X );
    UOdysseyLayerCell* cell = mAnimationLayerImageVector->GetCellAtFrame( breakdownFrameIndex );
    if (!cell)
        return;

    int breakdownCellIndex = cell->GetIndexInLayer();
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;

    GetSelectedInbetweenerTags( selectedInbetweenerTagList );

    if( selectedInbetweenerTagList.size() )
    {
        //---------- needed for undos-----------//
        GEditor->BeginTransaction(LOCTEXT("vector-timeline.transaction.remove-breakdown", "Remove Breakdown"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownRemove( mAnimationLayerImageVector->GetVectorLayer().Get()
                                                                                          , selectedInbetweenerTagList );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
            FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(mAnimationLayerImageVector->GetAnimation());
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
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::ShowHideTarget()
{
    int breakdownFrameIndex = MousePositionToFrame( mCursorPos.X );
    UOdysseyLayerCell* cell = mAnimationLayerImageVector->GetCellAtFrame( breakdownFrameIndex );

    if (!cell)
        return;

    UnbindLayerDelegates();

    int breakdownCellIndex = cell->GetIndexInLayer();
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;
    std::list<FInbetweenerBreakdown*> breakdownList;

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
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility( mAnimationLayerImageVector->GetVectorLayer().Get()
                                                                                                    , breakdownList );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
            FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(mAnimationLayerImageVector->GetAnimation());
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

    BindLayerDelegates();
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
SOdysseyAnimationLayerImageVectorTimelineInbetweening::UnbindLayerDelegates()
{
    mAnimationLayerImageVector->GetVectorLayer()->OnNotifyDelegate().RemoveAll( this );
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::BindLayerDelegates()
{
    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    mAnimationLayerImageVector->GetVectorLayer()->OnNotifyDelegate().AddRaw( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnVectorSceneNotify );
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::ChangeDirection()
{
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;

    UnbindLayerDelegates();

    GetSelectedInbetweenerTags( selectedInbetweenerTagList );

    //---------- needed for undos-----------//
/*
    GEditor->BeginTransaction(LOCTEXT("vector-timeline.transaction.change-diretion", "Change Direction"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownRemove( nullptr
                                                                                      , selectedInbetweenerTagList
                                                                                      , engineList );

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

    BindLayerDelegates();
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
    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(mAnimationLayerImageVector->GetAnimation());

    // note: editor is NULL in the Sequencer
    FOdysseyPainterEditor::RemoveInbetweenerTag( editor, mAnimationLayerImageVector->GetVectorLayer().Get() );
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
