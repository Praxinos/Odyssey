// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerImageTimeline.h"

class UOdysseyAnimationLayerImageRaster;

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageRasterTimeline
    : public SOdysseyAnimationLayerImageTimeline
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageRasterTimeline)
        {}
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

private:
    virtual TSharedRef<FOdysseyAnimationCell> OnCreateCell() override;
    virtual TSharedRef<SWidget> OnGenerateCellWidget(TSharedPtr<FOdysseyAnimationCell> iCell) override;
};