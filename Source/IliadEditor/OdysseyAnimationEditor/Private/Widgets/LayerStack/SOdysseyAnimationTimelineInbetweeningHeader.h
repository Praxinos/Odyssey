// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

//#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"

class FInbetweeningListViewItem;
class FOdysseyVectorSharedEnv;
class UOdysseyAnimationLayerImageVector;
class FOdysseyPainterEditor;
class FOdysseyVectorGroupPaint;

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineInbetweeningHeader
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineInbetweeningHeader
    : public SListView<TSharedPtr<FInbetweeningListViewItem>>
{
    public:
        ~SOdysseyAnimationTimelineInbetweeningHeader();
        SOdysseyAnimationTimelineInbetweeningHeader();

    public:
        SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineInbetweeningHeader)
            : _PainterEditor(nullptr)
            {}
            SLATE_ATTRIBUTE(FOdysseyPainterEditor*, PainterEditor)
        SLATE_END_ARGS()

        void Construct( const FArguments& InArgs, UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector );
        UOdysseyAnimationLayerImageVector* GetAnimationLayerImageVector();
        virtual FReply OnKeyDown ( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent ) override;
        virtual FReply OnKeyUp ( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent ) override;
        void Update();
        FOdysseyPainterEditor* GetEditor() const;

    protected:
        TSharedRef<ITableRow> OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                           , const TSharedRef<STableViewBase>& iOwnerTable );
        TSharedPtr<SWidget> OnContextMenuOpening();
        void RemoveInbetweenerTag();
        void ResetSpacingCharts();
        void Commit();
        void OnVectorSceneNotify( FOdysseyVectorGroupPaint* iScene, uint64 iNotificationFlags );
        void MapActionsToCommandList();

    private :
        virtual void Private_SelectRangeFromCurrentTo ( TSharedPtr<FInbetweeningListViewItem> iItem ) override;
        virtual void Private_SetItemSelection ( TSharedPtr<FInbetweeningListViewItem> iItem
                                              , bool bShouldBeSelected
                                              , bool bWasUserDirected ) override;
        virtual void Private_ClearSelection() override;
        virtual bool Private_IsItemSelected( const TSharedPtr<FInbetweeningListViewItem>& iItem )  const override;

    protected:
        UOdysseyAnimationLayerImageVector* mAnimationLayerImageVector;
        TArray<TSharedPtr<FInbetweeningListViewItem>> mItemsSource;
        TAttribute<FOdysseyPainterEditor*> mEditor;
        TSharedRef<FUICommandList> mCommandList;
};
