// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerImageTimeline.h"

class UOdysseyAnimationLayerImageVector;
class SOdysseyAnimationLayerImageVectorTimelineInbetweening;

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageVectorTimeline
    : public SOdysseyAnimationLayerImageTimeline
{
public:
    // Construction / Destruction
    virtual ~SOdysseyAnimationLayerImageVectorTimeline();
    SOdysseyAnimationLayerImageVectorTimeline();

public:
    TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> GetInbetweeningListView();

private:
    virtual TSharedRef<SWidget> OnGenerateCellWidget(UOdysseyAnimationCell* iCell) override;
    virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    EVisibility IsInbetweeningTimelineVisible() const;

protected:
	virtual TSharedRef<SWidget> GenerateWidget( const FName& iRow, const FName& iColumn ) override;
	virtual EVisibility GetRowVisibility(FName iRow) const override;
	TSharedRef<SWidget> GenerateInbetweeningRowTimelineWidget();

private:
    TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> mInbetweeningListView;
};
