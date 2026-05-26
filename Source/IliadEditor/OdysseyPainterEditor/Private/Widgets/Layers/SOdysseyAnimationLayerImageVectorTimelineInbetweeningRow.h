// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/SListView.h"

class FOdysseyVectorTagInbetweener;
class FInbetweeningListViewItem;
class FInbetweenerBreakdown;
class FOdysseyVectorUndo;

/**
 * Implements the List View Widget
 */
class SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow
    : public STableRow<TSharedPtr<FInbetweeningListViewItem>>
{
    struct CellBox
    {
        CellBox( uint32 iType, uint32 iIndex, double iX, double iY, double iW, double iH )
            : type ( iType )
            , index(iIndex)
            , x ( iX )
            , y ( iY )
            , w ( iW )
            , h ( iH )
        {
        }

        static const uint32 TYPE_SOURCE     = ( 1UL << 0 );
        static const uint32 TYPE_INBETWEEN  = ( 1UL << 1 );
        static const uint32 TYPE_TARGET     = ( 1UL << 2 );
        static const uint32 TYPE_VISIBLE    = ( 1UL << 3 );
        static const uint32 TYPE_INCOMPLETE = ( 1UL << 4 );

        uint32 type;
        uint32 index;
        double x, y, w, h;
    };

    public:
        DECLARE_DELEGATE_OneParam(FOnTransactCurrentFrame, TOptional<int>)

    public:
        SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow)
            : _CurrentFrame(0)
            {}
            SLATE_ATTRIBUTE(int, CurrentFrame)
            SLATE_EVENT(FOnTransactCurrentFrame, OnTransactCurrentFrame)
        SLATE_END_ARGS()

    public:
        ~SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow();
        SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow();

        void Construct( const FArguments& InArgs
                      , const TSharedRef< STableViewBase >& InOwnerTableView
                      , const TSharedPtr<FInbetweeningListViewItem> iTem );
        virtual FReply OnMouseButtonDown( const FGeometry & MyGeometry, const FPointerEvent & MouseEvent ) override;
        virtual FReply OnMouseMove ( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
        virtual FReply OnMouseButtonUp( const FGeometry & MyGeometry, const FPointerEvent & MouseEvent ) override;
        virtual FCursorReply OnCursorQuery ( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

    protected:
        virtual int32 OnPaint( const FPaintArgs& Args
                             , const FGeometry& AllottedGeometry
                             , const FSlateRect& MyCullingRect
                             , FSlateWindowElementList& OutDrawElements
                             , int32 LayerId
                             , const FWidgetStyle& InWidgetStyle
                             , bool bParentEnabled ) const override;
        FText GetInbetweenerTagInbetweenCount() const;

        virtual FVector2D ComputeDesiredSize ( float LayoutScaleMultiplier ) const override;
        virtual void CacheDesiredSize ( float LayoutScaleMultiplier ) override;

        float MousePositionToFrame(float iX) const;
        float FrameToMousePosition(float iFrame) const;

    protected:
        TAttribute<int> mCurrentFrame;
        FOnTransactCurrentFrame mOnTransactCurrentFrame;

        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        TArray<CellBox> mCellBoxBuffer;
        FVector2D mBoxPos;
        FVector2D mBoxSize;
        FInbetweenerBreakdown* mPickedBreakdown;
        CellBox mCandidateTargetCellBox;
        //Box containing the cells widgets
        //TSharedPtr<SHorizontalBox> mCellsBox;
        //float mLayoutScaleMultiplier;
        FOdysseyVectorUndo* mUndo;
};
