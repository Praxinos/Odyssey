// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
#pragma once

#include "Widgets/SOdysseyLayerRowBase.h"

class UOdysseyAnimationLayer;
class SOdysseyLayerStackTreeView;

/**
 * Implements a layer row widget
 */
class SOdysseyAnimationLayerTimeline
    : public SOdysseyLayerRowBase
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerTimeline)
        {}
    SLATE_END_ARGS()

public:
    void Construct(
        const FArguments& iArgs,
        const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
        UOdysseyAnimationLayer* iLayer
    );

protected:
    virtual const FSlateBrush* GetBorder() const override;
};
