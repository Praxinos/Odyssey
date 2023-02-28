// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

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
    void Construct(const FArguments& iArgs, class UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster);

private:
    TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FOdysseyRasterBlock> iRasterBlock, const TSharedRef<STableViewBase>& iOwnerTable);

private:
    class UOdysseyAnimationLayerImageRaster* mAnimationLayerImageRaster;
};
