// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include <list>

//#include "OdysseyLighttable.h"

class FInbetweeningListViewItem;
class UOdysseyAnimationLayerImageVector;
class FOdysseyVectorTagInbetweener;
class FOdysseyVectorEngine;
class FOdysseyPainterEditorAnimationTimelinePosition;
class FOdysseyPainterEditor;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorLayer;

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationLayerImageVectorTimelineInbetweening
class SOdysseyAnimationLayerImageVectorTimelineInbetweening
    : public SListView<TSharedPtr<FInbetweeningListViewItem>>
{
    public:
        ~SOdysseyAnimationLayerImageVectorTimelineInbetweening();
        SOdysseyAnimationLayerImageVectorTimelineInbetweening();

    public:
        SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageVectorTimelineInbetweening)
            {}
            SLATE_ARGUMENT( TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition )
        SLATE_END_ARGS()

        void Construct( const FArguments& InArgs
                        , UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector );

        void Update();
        UOdysseyAnimationLayerImageVector* GetAnimationLayerImageVector();
        void SetCursorPos( FVector2D iCursorPos );
        const FSlateBrush *GetForwardArrowBrush();
        const FSlateBrush *GetBackwardArrowBrush();
        virtual FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;
        TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> GetTimelinePosition() const;

    protected:
        void OnSelectionChanged( TSharedPtr<FInbetweeningListViewItem> iItem, ESelectInfo::Type SelectInfo );
        TSharedRef<ITableRow> OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                            , const TSharedRef<STableViewBase>& iOwnerTable );
        TSharedPtr<SWidget> OnContextMenuOpening();
        void AddBreakdown();
        void RemoveBreakdown();
        void ChangeDirection();
        void ShowHideTarget();
        void GetSelectedInbetweenerTags( std::list<FOdysseyVectorTagInbetweener*>& oSelectedInbetweenerTagList );
        void MapActionsToCommandList();
        void OnVectorSceneNotify( FOdysseyVectorLayer* iLayer, uint64 iNotificationFlags );
        void RemoveInbetweenerTag();

        float MousePositionToFrame(float iX) const;
        float FrameToMousePosition(float iFrame) const;

    private :
/*
        virtual void Private_SelectRangeFromCurrentTo ( TSharedPtr<FInbetweeningListViewItem> iItem ) override;
        virtual void Private_SetItemSelection ( TSharedPtr<FInbetweeningListViewItem> iItem
                                              , bool bShouldBeSelected
                                              , bool bWasUserDirected ) override;
        virtual void Private_ClearSelection() override;
*/
        virtual bool Private_IsItemSelected( const TSharedPtr<FInbetweeningListViewItem>& iItem )  const override;

    protected:
        UOdysseyAnimationLayerImageVector* mAnimationLayerImageVector;
        TArray<TSharedPtr<FInbetweeningListViewItem>> mItemsSource;
        FVector2D mCursorPos;
        const FSlateBrush *mForwardArrowBrush;
        const FSlateBrush *mBackwardArrowBrush;
        TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
        TSharedRef<FUICommandList> mCommandList;
};
