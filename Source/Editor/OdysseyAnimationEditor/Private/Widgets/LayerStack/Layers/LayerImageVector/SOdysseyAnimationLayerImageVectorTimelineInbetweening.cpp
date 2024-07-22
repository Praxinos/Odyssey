// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

// From module OdysseyAnimationEditor
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweening.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
// From U.E
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Math/UnitConversion.h"
// From module OdysseyStyle
#include "OdysseyStyleSet.h"
// From module OdysseyVector
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorTagInbetweener.h"


#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerImageVectorTimelineInbetweening::SOdysseyAnimationLayerImageVectorTimelineInbetweening()
{
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::Construct( const FArguments& InArgs
                                                                , FOdysseyVectorSharedEnv* iVectorSharedEnv
                                                                , FOdysseyAnimationEditorExtension* iAnimationEditorExtension )
{
    mVectorSharedEnv = iVectorSharedEnv;
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
        //.OnContextMenuOpening( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnContextMenuOpening )
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

    for( FOdysseyVectorTag* tag : mVectorSharedEnv->GetTagList() )
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

/*
void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnGetChildren( TSharedPtr<FInbetweeningListViewItem> iParent
                                                          , TArray<TSharedPtr<FInbetweeningListViewItem>>& oChildren ) const
{
    oChildren = iParent.Get()->mChildren;
}
*/

TSharedRef<ITableRow>
SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                                                    , const TSharedRef<STableViewBase>& iOwnerTable )
{
   return SNew( SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow, iOwnerTable, iItem );


    //return SNew( SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow, iOwnerTable, iItem )
}

/*
void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnExpansionChanged( TSharedPtr<FVectorSceneTreeViewItem> iItem
                                                               , bool mExpanded )
{
    FOdysseyVectorObject* expandedObject = iItem.Get()->GetVectorObject();

    expandedObject->SetExpanded( mExpanded );

    // Reselect
    SelectedItems.Empty();
    SelectTree( mRootItem );
}
*/

/*
void
SOdysseyAnimationLayerImageVectorTimelineInbetweening::OnSelectionChanged( TSharedPtr<FVectorSceneTreeViewItem> iItem
                                                               , ESelectInfo::Type SelectInfo )
{
    if( mRootItem && ( SelectInfo != ESelectInfo::Type::Direct ) )
    {
        FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.selection-changed","Selection Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( scene );

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

        scene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                                 // sending the MODIFIED flag will trigger the update
                                 // of the Object's DetailsView. If we send the SELECTED signal
                                 // it makes more sense but this widget will be immediately 
                                 // updated whereas it's already being updated, hence it creates
                                 // some problems, one of them being the selection of the whole
                                 //  vector scene when holding the shift key.
                                 // See https://github.com/Praxinos/IliadDev/issues/411
                                  | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
    }
}
*/

#undef LOCTEXT_NAMESPACE
