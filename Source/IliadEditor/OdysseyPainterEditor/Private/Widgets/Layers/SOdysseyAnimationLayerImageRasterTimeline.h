// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "SOdysseyAnimationLayerImageTimeline.h"

class UOdysseyAnimationLayerImageRaster;
class FOdysseyAnimationTimelineCellImageRasterShortcuts;
class FExtender;
class FUICommandList;

/**
 * Implements a layer row widget
 */
class SOdysseyAnimationLayerImageRasterTimeline
    : public SOdysseyAnimationLayerImageTimeline
{
public:
    // Construction / Destruction
    virtual ~SOdysseyAnimationLayerImageRasterTimeline();
    SOdysseyAnimationLayerImageRasterTimeline();

protected:
    //Context Menu
    virtual void BuildContextMenu(TSharedRef<FUICommandList> CommandList, FMenuBuilder& MenuBuilder, FFrameNumber iClickedFrame, TSharedRef<FExtender> MenuExtender) override;
    void AddCellsMenuEntries( FMenuBuilder& MenuBuilder );

private:
    virtual TSharedRef<SWidget> OnGenerateCellWidget(UOdysseyLayerCell* iCell) override;
    virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    TSharedPtr<FOdysseyAnimationTimelineCellImageRasterShortcuts> mAnimationTimelineCellImageRasterShortcuts;
};
