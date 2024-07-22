// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/SListView.h"

class FOdysseyVectorTagInbetweener;
class FInbetweeningListViewItem;

/**
 * Implements the List View Widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow
    : public STableRow<TSharedPtr<FInbetweeningListViewItem>>
{
    public:
        ~SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow();
        SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow();

        void Construct( const typename STableRow<TSharedPtr<FInbetweeningListViewItem>>::FArguments& InArgs
                      , const TSharedRef< STableViewBase >& InOwnerTableView
                      , const TSharedPtr<FInbetweeningListViewItem> iTem );

    protected:
        void Update();
        virtual int32 OnPaint( const FPaintArgs& Args
                             , const FGeometry& AllottedGeometry
                             , const FSlateRect& MyCullingRect
                             , FSlateWindowElementList& OutDrawElements
                             , int32 LayerId
                             , const FWidgetStyle& InWidgetStyle
                             , bool bParentEnabled ) const override;
        FText GetInbetweenerTagInbetweenCount() const;

    protected:
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        FVector2D mBoxPos;
        FVector2D mBoxSize;
        //Box containing the cells widgets
        //TSharedPtr<SHorizontalBox> mCellsBox;
};
