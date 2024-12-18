// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/SListView.h"

class FOdysseyVectorTagInbetweener;

// we need to encapsulate iInbetweenerTag because of garbage collection for the Listview
class FInbetweeningListViewItem
{
    public:
        ~FInbetweeningListViewItem();
        FInbetweeningListViewItem( FOdysseyVectorTagInbetweener* iInbetweenerTag );

        FOdysseyVectorTagInbetweener* GetInbetweenerTag();

    public:
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
};

/**
 * Implements the List View Widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineInbetweeningHeaderRow
    : public STableRow<TSharedPtr<FInbetweeningListViewItem>>
{
    public:
        ~SOdysseyAnimationTimelineInbetweeningHeaderRow();
        SOdysseyAnimationTimelineInbetweeningHeaderRow();

        void Construct( const typename STableRow<TSharedPtr<FInbetweeningListViewItem>>::FArguments& InArgs
                      , const TSharedRef< STableViewBase >& InOwnerTableView
                      , const TSharedPtr<FInbetweeningListViewItem> iInbetweenerTag );
        // from STableRow::IsSelected
        virtual bool IsItemSelected() const override;


        virtual FReply OnMouseButtonDown( const FGeometry & MyGeometry, const FPointerEvent & MouseEvent ) override;
        virtual FReply OnMouseButtonUp( const FGeometry & MyGeometry, const FPointerEvent & MouseEvent ) override;

    protected:
        virtual FVector2D ComputeDesiredSize ( float LayoutScaleMultiplier ) const override;

    protected:
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        TSharedPtr<STextBlock> mTextBlockWidget;
};
