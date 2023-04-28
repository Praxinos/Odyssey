// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineScrollBox.h"

class SOdysseyAnimationTimelineScrollBox;
/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageRasterTimeline
    : public SCompoundWidget
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
        FOdysseyAnimationEditor* iEditor,
        class UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
    );

public:
    //SWidget overrides
    void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime );

private:
    void RequestRefresh();
    void RefreshWidgets();
    void OnCellsChanged(class UOdysseyAnimationLayerImageRaster* iLayer);
    
    float GetLayerOffset() const;
    float GetCellHeight() const;
    float GetCellLength(int iCellIndex) const;

    void AddCellsWidgets();
    
    EVisibility GetTimingHandleVisibility() const;
    EVisibility GetLengthHandleVisibility() const;

    void OnLengthHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, int iCellIndex);
    void OnLengthHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnLengthHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

    FReply OnCellsMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    FReply OnCellsMouseMove(const FGeometry& iGeometry, const FPointerEvent& MouseEvent);
    FReply OnCellsMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& MouseEvent);

private:
    FOdysseyAnimationEditor* mEditor;
    class UOdysseyAnimationLayerImageRaster* mAnimationLayerImageRaster;
    TSharedPtr<SOdysseyAnimationTimelineScrollBox> mScrollBox;
    TSharedPtr<SBorder> mCellsBorder;
    TSharedPtr<SHorizontalBox> mCellsBox;
    
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
