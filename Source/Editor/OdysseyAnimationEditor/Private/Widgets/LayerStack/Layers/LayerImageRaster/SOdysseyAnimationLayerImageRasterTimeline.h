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
	virtual FReply OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
	virtual FReply OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& MouseEvent) override;
    void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime );

private:
    void RequestRefresh();
    void RefreshWidgets();
    void OnCellsChanged(class UOdysseyAnimationLayerImageRaster* iLayer);
    
    float GetLayerOffset() const;
    float GetCellHeight() const;
    float GetCellLength(int iCellIndex) const;

    void AddPreBehaviourWidget();
    void AddCellsWidgets();
    
    EVisibility GetTimingHandleVisibility() const;
    EVisibility GetLengthHandleVisibility() const;

    void OnLengthHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, int iCellIndex);
    void OnLengthHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnLengthHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

private:
    class UOdysseyAnimationLayerImageRaster* mAnimationLayerImageRaster;
    bool mIsRefreshPending;
    bool mIsOffsettingLayer;
    bool mIsDraggingCellLengthHandle;


	const FSlateBrush* mTimingHandleBrush;
	const FSlateBrush* mLengthHandleBrush;
    struct
    {
        int mStartOffset;
        int mOffset;
        double mMousePosition;
    } mLayerOffsetData;

    struct
    {
        int mCellIndex = INDEX_NONE;
        int mStartLength;
        int mLength;
        double mMousePosition;
    } mLengthHandleDragData;
};
