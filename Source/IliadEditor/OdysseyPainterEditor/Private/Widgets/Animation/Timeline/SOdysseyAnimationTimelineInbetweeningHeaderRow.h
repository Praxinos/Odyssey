// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
class SOdysseyAnimationTimelineInbetweeningHeaderRow
    : public STableRow<TSharedPtr<FInbetweeningListViewItem>>
{
    public:
        DECLARE_DELEGATE_OneParam(FOnTransactCurrentFrame, TOptional<int>)

    public:
        SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineInbetweeningHeaderRow)
            {}
            SLATE_EVENT(FOnTransactCurrentFrame, OnTransactCurrentFrame)
        SLATE_END_ARGS()

    public:
        ~SOdysseyAnimationTimelineInbetweeningHeaderRow();
        SOdysseyAnimationTimelineInbetweeningHeaderRow();

        void Construct( const FArguments& InArgs
                      , const TSharedRef< STableViewBase >& InOwnerTableView
                      , const TSharedPtr<FInbetweeningListViewItem> iInbetweenerTag );
        // from STableRow::IsSelected
        virtual bool IsItemSelected() const override;
        virtual FReply OnMouseButtonDown( const FGeometry & MyGeometry, const FPointerEvent & MouseEvent ) override;


    protected:
        virtual FVector2D ComputeDesiredSize ( float LayoutScaleMultiplier ) const override;
        ECheckBoxState GetVisibility() const;
        void OnCheckBoxStateChanged( ECheckBoxState iState );
        bool IsVisibilityEnabled() const;
        int32 OnPaint( const FPaintArgs& Args
                     , const FGeometry& AllottedGeometry
                     , const FSlateRect& MyCullingRect
                     , FSlateWindowElementList& OutDrawElements
                     , int32 LayerId
                     , const FWidgetStyle& InWidgetStyle
                     , bool bParentEnabled ) const override;

    protected:
        FOnTransactCurrentFrame mOnTransactCurrentFrame;
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        TSharedPtr<STextBlock> mTextBlockWidget;
        FOnTransactCurrentFrame mOnTransactCurrentFrame;
};
