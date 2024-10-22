// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SOdysseyLayerStackTreeView.h"

class FOdysseyAnimationTimelineShortcuts;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerStackTreeView
    : public SOdysseyLayerStackTreeView
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerStackTreeView)
        : _HeaderFillWidth( 1.0f )
        {}
        SLATE_ARGUMENT( UOdysseyLayerStack*, LayerStack )
        SLATE_ARGUMENT( TArray<SHeaderRow::FColumn::FArguments>, AdditionalColumns )
        /** Set the HeaderColumn Size Mode to Fill. It's a fraction between 0 and 1 */
        SLATE_ATTRIBUTE( float, HeaderFillWidth )
        /** Set the HeaderColumn Size Mode to Fixed. */
        SLATE_ARGUMENT( TOptional< float >, HeaderFixedWidth )
        /** Set the HeaderColumn Size Mode to Manual. */
        SLATE_ATTRIBUTE( float, HeaderManualWidth )
        /** Set the HeaderColumn Size Mode to Fill Sized. */
        SLATE_ARGUMENT(TOptional< float >, HeaderFillSized)
        SLATE_EVENT( FOnGenerateRow, OnGenerateRow )
		SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelineCellSelection>, TimelineCellSelection )
    SLATE_END_ARGS()

public:
    SOdysseyAnimationLayerStackTreeView();
    void Construct(const FArguments& InArgs);

private:
    virtual FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;
    virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
    virtual void Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo);
    
    virtual TArray<TSharedPtr<FExtender>> ExtendContextMenu();

private:
    void ExtendContextMenuLayerSection(FMenuBuilder& iMenuBuilder);
    void Action_ConvertLayerToRasterLayer();

private:
    TSharedPtr<FOdysseyAnimationTimelineShortcuts> mTimelineShortcuts;
	TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
	TSharedPtr<FOdysseyAnimationEditorTimelineCellSelection> mTimelineCellSelection;
};
