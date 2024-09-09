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
class FInbetweenerBreakdown;

enum EInbetweeningRowCellBoxType : uint8
{
    None      = 0,
    Source    = 1,
    Inbetween = 2,
    Target    = 3
};

struct FInbetweeningRowCellBox
{
    FInbetweeningRowCellBox( EInbetweeningRowCellBoxType iType, uint32 iIndex, double iX, double iY, double iW, double iH )
        : type ( iType )
        , index( iIndex )
        , x ( iX )
        , y ( iY )
        , w ( iW )
        , h ( iH )
    {
    }

    EInbetweeningRowCellBoxType type;
    double x, y, w, h;
    uint32 index;
};

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
        virtual FReply OnMouseButtonDown( const FGeometry & MyGeometry, const FPointerEvent & MouseEvent ) override;
        virtual FReply OnMouseMove ( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
        virtual FReply OnMouseButtonUp( const FGeometry & MyGeometry, const FPointerEvent & MouseEvent ) override;
        virtual FCursorReply OnCursorQuery ( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;
        // from STableRow::IsSelected
        virtual bool IsItemSelected() const override;


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

    protected:
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        TArray<FInbetweeningRowCellBox> mCellBoxBuffer;
        FVector2D mBoxPos;
        FVector2D mBoxSize;
        FInbetweeningRowCellBox mCandidateTargetCellBox;
        FInbetweenerBreakdown* mPickedBreakdown;
        //Box containing the cells widgets
        //TSharedPtr<SHorizontalBox> mCellsBox;
        float mLayoutScaleMultiplier;
};
