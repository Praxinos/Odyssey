// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineSection.h"

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageRasterCellSection
    : public SOdysseyAnimationTimelineSection
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageRasterCellSection)
        {}
    SLATE_END_ARGS()

public:
    void Construct(
        const FArguments& iArgs,
        TSharedPtr<SOdysseyAnimationLayerStack> iLayerStackWidget,
        TSharedPtr<FOdysseyAnimationCell> iCell
    );

private:
    virtual FOptionalSize GetSectionWidth() const override;
    virtual FOptionalSize GetSectionHeight() const override;

private:
    TWeakPtr<FOdysseyAnimationCell> mCell;
};