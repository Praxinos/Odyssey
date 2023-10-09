// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerImageTimeline.h"

class UOdysseyAnimationLayerImageVector;

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageVectorTimeline
    : public SOdysseyAnimationLayerImageTimeline
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageVectorTimeline)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    virtual ~SOdysseyAnimationLayerImageVectorTimeline();
    SOdysseyAnimationLayerImageVectorTimeline();
    void Construct(
        const FArguments& iArgs, 
        FOdysseyAnimationEditorExtension* iExtension,
        UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
    );

private:
    virtual TSharedRef<FOdysseyAnimationCell> OnCreateCell() override;
    virtual TSharedRef<SWidget> OnGenerateCellWidget(TSharedPtr<FOdysseyAnimationCell> iCell) override;
};
