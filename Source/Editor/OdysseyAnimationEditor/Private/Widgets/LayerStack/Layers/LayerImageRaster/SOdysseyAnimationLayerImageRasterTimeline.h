// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerImageTimeline.h"

class UOdysseyAnimationLayerImageRaster;
class FOdysseyAnimationTimelineCellImageRasterShortcuts;

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageRasterTimeline
    : public SOdysseyAnimationLayerImageTimeline
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageRasterTimeline)
        {}
        SLATE_ATTRIBUTE(bool, DisplayOptions)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    virtual ~SOdysseyAnimationLayerImageRasterTimeline();
    SOdysseyAnimationLayerImageRasterTimeline();
    void Construct(
        const FArguments& iArgs, 
        FOdysseyAnimationEditorExtension* iExtension,
        UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
    );

protected:
    //Context Menu
    virtual TSharedPtr<FExtender> ExtendContextMenu() override;

private:
    virtual TSharedRef<SWidget> OnGenerateCellWidget(UOdysseyAnimationCell* iCell) override;
    virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    bool GetShowCellContent() const;

private:
    TSharedPtr<FOdysseyAnimationTimelineCellImageRasterShortcuts> mAnimationTimelineCellImageRasterShortcuts;
};