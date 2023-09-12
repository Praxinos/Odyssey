// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditorExtension;
class FOdysseyAnimationCell;
class UOdysseyAnimationLayerImageRaster;

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
        FOdysseyAnimationEditorExtension* iExtension,
        UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
    );

private:
    TSharedRef<FOdysseyAnimationCell> OnCreateCell();
    TSharedRef<SWidget> OnCreateCellWidget(TSharedPtr<FOdysseyAnimationCell> iCell);
    EVisibility GetFrameSelectorVisibility() const;
    void OnBuildCellContextMenu(FMenuBuilder& iMenuBuilder, int iFrame);

private:
    FOdysseyAnimationEditorExtension* mExtension;
    UOdysseyAnimationLayerImageRaster* mAnimationLayerImageRaster;
};
