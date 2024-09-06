// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

// From module OdysseyAnimationEditor
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweening.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
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
#include "OdysseyVectorAnimationCell.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorTagInbetweener.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAdd.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownRemove.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerImageVectorTimelineInbetweening::SOdysseyAnimationLayerImageVectorTimelineInbetweening()
{
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::Construct( const FArguments& InArgs
                                                                , UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
                                                                , FOdysseyAnimationEditorExtension* iAnimationEditorExtension )
{
    mAnimationLayerImageVector = iAnimationLayerImageVector;
    mAnimationEditorExtension = iAnimationEditorExtension;

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

FOdysseyAnimationEditorExtension*
SOdysseyAnimationLayerImageVectorTimelineInbetweening::GetAnimationEditorExtension()
{
    return mAnimationEditorExtension;
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
    return iItem.Get()->GetInbetweenerTag()->GetOwner()->IsSelected();
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

    return menu.MakeWidget();
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::GetSelectedInbetweenerTags( std::list<FOdysseyVectorTagInbetweener*>& oSelectedInbetweenerTagList
                                                                                 , std::list<FOdysseyVectorEngine*>& oEngineList )
{
    FOdysseyVectorSharedEnv* sharedEnv = mAnimationLayerImageVector->GetSharedEnv();

    oSelectedInbetweenerTagList.clear();
    oEngineList.clear();

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

                // find which engine/scene to update/redraw
                if( std::find_if( oEngineList.begin()
                                , oEngineList.end()
                                , [ inbetweenerTagEngine ]( FOdysseyVectorEngine* engine ) -> bool
                                  {
                                      return ( inbetweenerTagEngine == engine ) ? true : false;
                                  } ) == oEngineList.end() )
                {
                    oEngineList.push_back( inbetweenerTagEngine );
                }
            }
        }
    }
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::AddBreakdown()
{
    int breakdownFrameIndex = mAnimationEditorExtension->Timeline()->GetFrameIndexAtMousePosition( mCursorPos.X );
    int breakdownCellIndex = mAnimationLayerImageVector->GetCellsContainer()->GetCellIndexAtFrame( breakdownFrameIndex );
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;
    std::list<FOdysseyVectorEngine*> engineList;
    uint64 retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                    | FOdysseyPainterEditor::UI_UPDATE_TIMELINE;

    GetSelectedInbetweenerTags( selectedInbetweenerTagList, engineList );

    //---------- needed for undos-----------//
    GEditor->BeginTransaction(LOCTEXT("vector-timeline.transaction.add-breakdown", "Add Breakdown"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownAdd( nullptr
                                                                                   , selectedInbetweenerTagList
                                                                                   , engineList
                                                                                   , retFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = mAnimationEditorExtension->GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();
    //--------------------------------------//

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : selectedInbetweenerTagList )
    {
        uint32 tagCellIndex = inbetweenerTag->GetOwner()->GetEngine()->GetAnimationCell()->GetIndex();
        uint32 drawingIndex = breakdownCellIndex - tagCellIndex;
        FInbetweenerBreakdown* curBreakdown = inbetweenerTag->GetBreakdown( drawingIndex );

        FOdysseyVectorGroupPaint* scene = inbetweenerTag->GetOwner()->GetScene();

        if(  ( curBreakdown )
          // check new breakdown isn't on an existing breakdown limits
          && ( curBreakdown->GetSourceDrawingIndex() < drawingIndex )
          && ( curBreakdown->GetTargetDrawingIndex() > drawingIndex ) )
        {
            FInbetweenerBreakdown* newbreakdown = new FInbetweenerBreakdown( inbetweenerTag );

            inbetweenerTag->AddBreakdown( newbreakdown, drawingIndex, true );
        }
    }

    // force recompute internal geometry of the attached widget
    for( TSharedPtr<FInbetweeningListViewItem> item : GetItems() )
    {
        WidgetFromItem ( item ).Get()->AsWidget()->MarkPrepassAsDirty();
    }

    for( FOdysseyVectorEngine* engine : engineList )
    {
        engine->GetScene()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        engine->Signal( retFlags );
    }

    //mAnimationLayerImageVector.Get

    //Update();
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::RemoveBreakdown()
{
    int breakdownFrameIndex = mAnimationEditorExtension->Timeline()->GetFrameIndexAtMousePosition( mCursorPos.X );
    int breakdownCellIndex = mAnimationLayerImageVector->GetCellsContainer()->GetCellIndexAtFrame( breakdownFrameIndex );
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;
    std::list<FOdysseyVectorEngine*> engineList;
    uint64 retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                    | FOdysseyPainterEditor::UI_UPDATE_TIMELINE;

    GetSelectedInbetweenerTags( selectedInbetweenerTagList, engineList );

    //---------- needed for undos-----------//
    GEditor->BeginTransaction(LOCTEXT("vector-timeline.transaction.remove-breakdown", "Remove Breakdown"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownRemove( nullptr
                                                                                      , selectedInbetweenerTagList
                                                                                      , engineList
                                                                                      , retFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = mAnimationEditorExtension->GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();
    //--------------------------------------//

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : selectedInbetweenerTagList )
    {
        uint32 tagCellIndex = inbetweenerTag->GetOwner()->GetEngine()->GetAnimationCell()->GetIndex();
        FOdysseyVectorEngine* inbetweenerTagEngine = inbetweenerTag->GetOwner()->GetEngine();
        uint32 drawingIndex = breakdownCellIndex - tagCellIndex;
        FInbetweenerBreakdown* breakdown = inbetweenerTag->GetBreakdown( drawingIndex );
        FOdysseyVectorGroupPaint* scene = inbetweenerTag->GetOwner()->GetScene();

        // we delete the breakdown only if it is not the master breakdown ( the default one)
        if( breakdown && breakdown->GetMasterBreakdown() )
        {
            inbetweenerTag->RemoveBreakdown( breakdown, false );
        }
    }

    // force recompute internal geometry of the attached widget
    for( TSharedPtr<FInbetweeningListViewItem> item : GetItems() )
    {
        WidgetFromItem ( item ).Get()->AsWidget()->MarkPrepassAsDirty();
    }

    for( FOdysseyVectorEngine* engine : engineList )
    {
        engine->GetScene()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        engine->Signal( retFlags );
    }

    //mAnimationLayerImageVector.Get

    //Update();
}

#undef LOCTEXT_NAMESPACE
