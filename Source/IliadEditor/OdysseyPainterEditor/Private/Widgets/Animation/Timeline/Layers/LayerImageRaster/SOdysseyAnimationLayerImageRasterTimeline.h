// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/Animation/Timeline/Layers/SOdysseyAnimationLayerImageTimeline.h"

class UOdysseyAnimationLayerImageRaster;
class FOdysseyAnimationTimelineCellImageRasterShortcuts;

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
    virtual TSharedPtr<FExtender> ExtendContextMenu() override;

private:
    virtual TSharedRef<SWidget> OnGenerateCellWidget(UOdysseyAnimationCell* iCell) override;
    virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    TSharedPtr<FOdysseyAnimationTimelineCellImageRasterShortcuts> mAnimationTimelineCellImageRasterShortcuts;
};
