// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "SOdysseyAnimationLayerImageTimeline.h"

class UOdysseyAnimationLayerImageVector;
class SOdysseyAnimationLayerImageVectorTimelineInbetweening;
class SOdysseyPainterEditorVectorMassModifierView;

/**
 * Implements a layer row widget
 */
class SOdysseyAnimationLayerImageVectorTimeline
    : public SOdysseyAnimationLayerImageTimeline
{
public:
    // Construction / Destruction
    virtual ~SOdysseyAnimationLayerImageVectorTimeline();
    SOdysseyAnimationLayerImageVectorTimeline();

public:
    TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> GetInbetweeningListView();

private:
    virtual TSharedRef<SWidget> OnGenerateCellWidget(UOdysseyLayerCell* iCell) override;
    virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    EVisibility IsInbetweeningTimelineVisible() const;

    virtual void BuildContextMenu( TSharedRef<FUICommandList> CommandList, FMenuBuilder& MenuBuilder, FFrameNumber iClickedFrame, TSharedRef<FExtender> MenuExtender ) override;
    void AddCellsMenuEntries( FMenuBuilder& MenuBuilder );

protected:
    virtual TSharedRef<SWidget> GenerateWidget( const FName& iRow, const FName& iColumn ) override;
    virtual EVisibility GetRowVisibility(FName iRow) const override;
    TSharedRef<SWidget> GenerateInbetweeningRowTimelineWidget();

    FReply MassModifierAcceptProperties( TSharedRef<SOdysseyPainterEditorVectorMassModifierView> iObjectView );
    void MassModifierWindowClosed( const TSharedRef<SWindow>& iWindow, TSharedRef<SOdysseyPainterEditorVectorMassModifierView> objectView );
    void MassModifier();

private:
    TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> mInbetweeningListView;
};
