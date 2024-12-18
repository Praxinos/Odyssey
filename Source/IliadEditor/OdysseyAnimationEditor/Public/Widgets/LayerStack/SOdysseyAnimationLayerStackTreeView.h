// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SOdysseyLayerStackTreeView.h"

class FOdysseyAnimationTimelineShortcuts;
class FOdysseyAnimationEditorTimelinePosition;
class UOdysseyAnimationLayerStack;
class FOdysseyPainterEditor;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerStackTreeView
    : public SOdysseyLayerStackTreeView
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerStackTreeView)
        : _ExternalScrollbar(nullptr)
        , _PainterEditor(nullptr)
        {}
        SLATE_ARGUMENT( UOdysseyAnimationLayerStack*, LayerStack )
        SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
        SLATE_ARGUMENT( TOptional<TArray<SHeaderRow::FColumn::FArguments>>, Columns )
        SLATE_EVENT( FOnGenerateRow, OnGenerateRow )
        SLATE_ARGUMENT( TSharedPtr<SScrollBar>, ExternalScrollbar )
        SLATE_EVENT( FOnTableViewScrolled, OnTreeViewScrolled )
        SLATE_ATTRIBUTE(FOdysseyPainterEditor*, PainterEditor)
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
    TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;

    UOdysseyAnimationLayerStack* mLayerStack;
    TAttribute<FOdysseyPainterEditor*> mEditor;
};
