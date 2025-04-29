// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/Animation/Timeline/Layers/SOdysseyAnimationLayerImageTimeline.h"

class UOdysseyAnimationLayerImageVector;
class SOdysseyAnimationLayerImageVectorTimelineInbetweening;

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

protected:
    virtual TSharedRef<SWidget> GenerateWidget( const FName& iRow, const FName& iColumn ) override;
    virtual EVisibility GetRowVisibility(FName iRow) const override;
    TSharedRef<SWidget> GenerateInbetweeningRowTimelineWidget();

private:
    TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> mInbetweeningListView;
};
