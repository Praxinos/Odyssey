// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeader.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"

#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"

#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Math/UnitConversion.h"
#include "OdysseyStyleSet.h"

#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorObject.h"

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
        //.OnContextMenuOpening( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnContextMenuOpening )
        //.SelectionMode( ESelectionMode::Multi )
        //.HeaderRow(headerRow)
    );

    Update();
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
    return iItem.Get()->GetInbetweenerTag()->GetOwner()->IsSelected();
}

TSharedRef<ITableRow>
SOdysseyAnimationTimelineInbetweeningHeader::OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                                          , const TSharedRef<STableViewBase>& iOwnerTable )
{
    return SNew( SOdysseyAnimationTimelineInbetweeningHeaderRow, iOwnerTable, iItem );
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
