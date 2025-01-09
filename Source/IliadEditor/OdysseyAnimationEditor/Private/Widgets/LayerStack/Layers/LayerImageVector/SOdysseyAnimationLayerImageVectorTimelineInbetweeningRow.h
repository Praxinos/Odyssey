// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/SListView.h"

class FOdysseyVectorTagInbetweener;
class FInbetweeningListViewItem;
class FInbetweenerBreakdown;

/**
 * Implements the List View Widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow
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

        static const uint32 TYPE_SOURCE    = ( 1UL << 0 );
        static const uint32 TYPE_INBETWEEN = ( 1UL << 1 );
        static const uint32 TYPE_TARGET    = ( 1UL << 2 );
        static const uint32 TYPE_VISIBLE   = ( 1UL << 3 );

        uint32 type;
        uint32 index;
        double x, y, w, h;
    };

    public:
        ~SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow();
        SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow();

        void Construct( const typename STableRow<TSharedPtr<FInbetweeningListViewItem>>::FArguments& InArgs
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
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        TArray<CellBox> mCellBoxBuffer;
        FVector2D mBoxPos;
        FVector2D mBoxSize;
        FInbetweenerBreakdown* mPickedBreakdown;
        CellBox mCandidateTargetCellBox;
        //Box containing the cells widgets
        //TSharedPtr<SHorizontalBox> mCellsBox;
        //float mLayoutScaleMultiplier;
};
