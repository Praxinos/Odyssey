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
    void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime );

private:
    void RequestRefresh();
    void RefreshWidgets();
    TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FOdysseyAnimationCell> iCell, const TSharedRef<STableViewBase>& iOwnerTable);
    void OnCellsChanged(class UOdysseyAnimationLayerImageRaster* iLayer);
    void OnTimelineOffsetChanged();

    FOptionalSize GetCellWidth() const;
    FOptionalSize GetCellHeight() const;

private:
    class UOdysseyAnimationLayerImageRaster* mAnimationLayerImageRaster;
    bool mIsRefreshPending;
};
