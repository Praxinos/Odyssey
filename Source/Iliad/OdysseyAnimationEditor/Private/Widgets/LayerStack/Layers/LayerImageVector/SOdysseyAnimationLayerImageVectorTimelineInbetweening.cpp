// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

// From module OdysseyAnimationEditor
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweening.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
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
#include "OdysseyStyleSet.h"
// From module OdysseyVector
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorTagInbetweener.h"
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
        //.OnSelectionChanged( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnSelectionChanged )
        //.OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnContextMenuOpening )
        //.SelectionMode( ESelectionMode::Multi )
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

    for( FOdysseyVectorTag* tag : mAnimationLayerImageVector->GetSharedEnv()->GetSharedTagList() )
    {
        if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>( tag );

            mItemsSource.Insert( MakeShareable( new FInbetweeningListViewItem(inbetweenerTag) ), 0 );
        }
    }

    // Select items if needed
    //SelectedItems.Empty();
    RequestListRefresh();
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
    FOdysseyVectorSharedEnv* sharedEnv = mAnimationLayerImageVector->GetSharedEnv();

    oSelectedInbetweenerTagList.clear();

    // I could not make it work using SListView SelectedItems methods, for some reason. So I use the SharedEnv
    for( FOdysseyVectorTag* tag : sharedEnv->GetSharedTagList() )
    {
        if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

            if( tag->GetOwner()->IsSelected() )
            {
                FOdysseyVectorEngine* inbetweenerTagEngine = inbetweenerTag->GetOwner()->GetEngine();

                oSelectedInbetweenerTagList.push_back( inbetweenerTag );
            }
        }
    }
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::AddBreakdown()
{
    int breakdownFrameIndex = mTimelinePosition->MousePositionToFrame( mCursorPos.X );
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
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownAdd( mAnimationLayerImageVector->GetSharedEnv()
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
            uint32 tagCellIndex = inbetweenerTag->GetOwner()->GetEngine()->GetCell()->GetIndex();
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
        mAnimationLayerImageVector->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    }

    FOdysseyVectorEngine::Notify( nullptr, notificationFlags );
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::RemoveBreakdown()
{
    int breakdownFrameIndex = mTimelinePosition->MousePositionToFrame( mCursorPos.X );
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
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownRemove( mAnimationLayerImageVector->GetSharedEnv()
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
            uint32 tagCellIndex = inbetweenerTag->GetOwner()->GetEngine()->GetCell()->GetIndex();
            FOdysseyVectorEngine* inbetweenerTagEngine = inbetweenerTag->GetOwner()->GetEngine();
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
        mAnimationLayerImageVector->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    }

    // static call
    FOdysseyVectorEngine::Notify( nullptr, notificationFlags );
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::ShowHideTarget()
{
    int breakdownFrameIndex = mTimelinePosition->MousePositionToFrame( mCursorPos.X );
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
        uint32 tagCellIndex = inbetweenerTag->GetOwner()->GetEngine()->GetCell()->GetIndex();
        FOdysseyVectorEngine* inbetweenerTagEngine = inbetweenerTag->GetOwner()->GetEngine();
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
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility( mAnimationLayerImageVector->GetSharedEnv()
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
        mAnimationLayerImageVector->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
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
    mAnimationLayerImageVector->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

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
    FOdysseyPainterEditor::RemoveInbetweenerTag( editor, mAnimationLayerImageVector->GetSharedEnv() );
}

#undef LOCTEXT_NAMESPACE
