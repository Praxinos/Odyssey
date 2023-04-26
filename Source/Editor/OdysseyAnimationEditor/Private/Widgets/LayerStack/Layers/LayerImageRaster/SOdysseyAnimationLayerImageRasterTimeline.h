// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineWidget.h"

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageRasterTimeline
    : public SOdysseyAnimationTimelineWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageRasterTimeline)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyAnimationLayerImageRasterTimeline();
    SOdysseyAnimationLayerImageRasterTimeline();
    void Construct(
        const FArguments& iArgs, 
        TSharedPtr<SOdysseyAnimationLayerStack> iLayerStackWidget,
        class UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
    );

public:
    //SWidget overrides
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime );

private:
    void RequestRefresh();
    void RefreshWidgets();
    void OnCellsChanged(class UOdysseyAnimationLayerImageRaster* iLayer);
    
    float GetLayerOffset() const;
    float GetCellHeight() const;

    void AddPreBehaviourWidget();
    void AddCellsWidgets();

private:
    class UOdysseyAnimationLayerImageRaster* mAnimationLayerImageRaster;
    bool mIsRefreshPending;
    bool mIsOffsettingLayer;

    FVector2D mOffsetMousePosition;
    int mLayerOffset;
};
