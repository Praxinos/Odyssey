// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

//#include "OdysseyLighttable.h"

class FInbetweeningListViewItem;
class FOdysseyVectorLayer;
class UOdysseyAnimationLayerImageVector;
class FOdysseyPainterEditor;
class FOdysseyVectorGroupPaint;
struct FOdysseyVectorObjectInvalidationFlags;

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineInbetweeningHeader
class SOdysseyAnimationTimelineInbetweeningHeader
    : public SListView<TSharedPtr<FInbetweeningListViewItem>>
{
    public:
        DECLARE_DELEGATE_OneParam(FOnTransactCurrentFrame, TOptional<int>)

    public:
        ~SOdysseyAnimationTimelineInbetweeningHeader();
        SOdysseyAnimationTimelineInbetweeningHeader();

    public:
        SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineInbetweeningHeader)
            {}
            SLATE_EVENT(FOnTransactCurrentFrame, OnTransactCurrentFrame)
        SLATE_END_ARGS()

        void Construct( const FArguments& InArgs, UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector );
        UOdysseyAnimationLayerImageVector* GetAnimationLayerImageVector();
        virtual FReply OnKeyDown ( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent ) override;
        virtual FReply OnKeyUp ( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent ) override;
        void Update();

    protected:
        void OnSelectionChanged( TSharedPtr<FInbetweeningListViewItem> iItem, ESelectInfo::Type SelectInfo );
        TSharedRef<ITableRow> OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                           , const TSharedRef<STableViewBase>& iOwnerTable );
        TSharedPtr<SWidget> OnContextMenuOpening();
        void RemoveInbetweenerTag();
        void ResetSpacingCharts();
        void Commit();
        void OnVectorSceneNotify( FOdysseyVectorLayer* iLayer
                                , const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags );
        void MapActionsToCommandList();

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
        TSharedRef<FUICommandList> mCommandList;
        FOnTransactCurrentFrame mOnTransactCurrentFrame;
};
