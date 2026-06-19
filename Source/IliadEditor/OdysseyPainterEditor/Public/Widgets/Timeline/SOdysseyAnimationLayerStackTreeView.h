// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Widgets/SOdysseyLayerStackTreeView.h"

class FOdysseyAnimationTimelineShortcuts;
class FOdysseyPainterEditorAnimationTimelinePosition;
class UOdysseyAnimationLayerStack;

class ODYSSEYPAINTEREDITOR_API SOdysseyAnimationLayerStackTreeView
    : public SOdysseyLayerStackTreeView
{
    DECLARE_DELEGATE_OneParam(FOnTransactCurrentFrame, TOptional<int>)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerStackTreeView)
        : _CurrentFrame(0)
        , _ExternalScrollbar(nullptr)
        {}
        SLATE_ARGUMENT( UOdysseyAnimationLayerStack*, LayerStack )
        SLATE_ATTRIBUTE( int, CurrentFrame )
        SLATE_ARGUMENT( TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition )
        SLATE_ARGUMENT( TOptional<TArray<SHeaderRow::FColumn::FArguments>>, Columns )
        SLATE_EVENT( FOnGenerateRow, OnGenerateRow )
        SLATE_ARGUMENT( TSharedPtr<SScrollBar>, ExternalScrollbar )
        SLATE_EVENT( FOnTableViewScrolled, OnTreeViewScrolled )
        SLATE_EVENT(FOnTransactCurrentFrame, OnTransactCurrentFrame)
    SLATE_END_ARGS()

public:
    SOdysseyAnimationLayerStackTreeView();
    void Construct(const FArguments& InArgs);
    TSharedRef<ITableRow> OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable);

private:
    virtual FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;
    virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
    virtual void Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo);

    virtual TArray<TSharedPtr<FExtender>> ExtendContextMenu();

private:
    void ExtendContextMenuLayerSection(FMenuBuilder& iMenuBuilder);
    void Action_ConvertLayerToRasterLayer();
    void OnLayerAdded(UOdysseyLayer* iLayer);

private:
    TSharedPtr<FOdysseyAnimationTimelineShortcuts> mTimelineShortcuts;
    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
    TAttribute<int> mCurrentFrame;

    UOdysseyAnimationLayerStack* mLayerStack;
};
