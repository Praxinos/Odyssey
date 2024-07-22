// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

    protected:
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        TSharedPtr<STextBlock> mTextBlockWidget;
};
