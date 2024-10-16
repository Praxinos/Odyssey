// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeader.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"

#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Math/UnitConversion.h"
#include "OdysseyStyleSet.h"

// from module OdysseyPainterEditor
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditor.h"

// from module OdysseyVector
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorEngine.h"
#include "Undo/OdysseyVectorUndoTagRemove.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationTimelineInbetweeningHeader::SOdysseyAnimationTimelineInbetweeningHeader()
{
}

void
SOdysseyAnimationTimelineInbetweeningHeader::Construct( const FArguments& InArgs
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
        .OnGenerateRow( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnGenerateRow ) 
        //.OnGetChildren( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnGetChildren )
        //.OnSelectionChanged( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnSelectionChanged )
        //.OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnContextMenuOpening )
        //.SelectionMode( ESelectionMode::Multi )
        //.HeaderRow(headerRow)
    );

    Update();
}

FReply
SOdysseyAnimationTimelineInbetweeningHeader::OnKeyUp ( const FGeometry& MyGeometry
                                                        , const FKeyEvent& InKeyEvent )
{
// Commented-out. Pressing delete would delete the whole Cell due to the way events are managed in Odyssey.
/*
    if( InKeyEvent.GetKey() == EKeys::Delete )
    {
        RemoveInbetweenerTags();

        return FReply::Handled();
    }
*/
    return FReply::Unhandled();
}

void
SOdysseyAnimationTimelineInbetweeningHeader::Update()
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
SOdysseyAnimationTimelineInbetweeningHeader::Private_IsItemSelected( const TSharedPtr<FInbetweeningListViewItem>& iItem )  const
{
    UOdysseyAnimationLayerStack* layerStack = mAnimationEditorExtension->LayerStack();

    return iItem.Get()->GetInbetweenerTag()->GetOwner()->IsSelected() && ( layerStack->CurrentLayer == mAnimationLayerImageVector );
}

TSharedRef<ITableRow>
SOdysseyAnimationTimelineInbetweeningHeader::OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                                          , const TSharedRef<STableViewBase>& iOwnerTable )
{
    return SNew( SOdysseyAnimationTimelineInbetweeningHeaderRow, iOwnerTable, iItem );
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
    FOdysseyPainterEditor::CommitSelectedInbetweenerTag( mAnimationEditorExtension->GetEditor()
                                                       , mAnimationLayerImageVector->GetSharedEnv() );
}

void
SOdysseyAnimationTimelineInbetweeningHeader::ResetSpacingCharts()
{
    FOdysseyPainterEditor::ResetInbetweenerTagSpacingChart( mAnimationEditorExtension->GetEditor()
                                                          , mAnimationLayerImageVector->GetSharedEnv() );


}

void
SOdysseyAnimationTimelineInbetweeningHeader::RemoveInbetweenerTag()
{
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;
    std::list<FOdysseyVectorTag*> tagList;

    mAnimationLayerImageVector->GetSharedEnv()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                                                         , tagList );

    if( tagList.size() )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.delete-tags", "Remove Tags"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagRemove( tagList.front()->GetOwner()->GetScene()
                                                                      , tagList
                                                                      , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
            TSharedPtr<FOdysseyPainterEditorSource> source = mAnimationEditorExtension->GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        for( FOdysseyVectorTag* tag : tagList )
        {
            tag->GetOwner()->RemoveTag( tag );
            tag->GetOwner()->GetEngine()->UnselectObject( tag->GetOwner() );

            tag->GetOwner()->GetEngine()->Invalidate( 0 );
        }
    }

    // update UI
    FOdysseyVectorEngine::Notify( nullptr, notificationFlags );
}

FOdysseyAnimationEditorExtension*
SOdysseyAnimationTimelineInbetweeningHeader::GetAnimationEditorExtension()
{
    return mAnimationEditorExtension;
}

UOdysseyAnimationLayerImageVector*
SOdysseyAnimationTimelineInbetweeningHeader::GetAnimationLayerImageVector()
{
    return mAnimationLayerImageVector;
}


#undef LOCTEXT_NAMESPACE
