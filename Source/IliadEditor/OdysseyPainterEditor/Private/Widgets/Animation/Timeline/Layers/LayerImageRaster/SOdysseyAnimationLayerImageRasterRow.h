// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Animation/Timeline/Layers/SOdysseyAnimationLayerRow.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLighttableKey.h"

class FOdysseyPainterEditorAnimationTimelinePosition;
class FOdysseyPainterEditor;
/**
 * Implements a layer row widget
 */
class SOdysseyAnimationLayerImageRasterRow
    : public SOdysseyAnimationLayerRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageRasterRow)
        {}
        SLATE_ARGUMENT( TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(
        const FArguments& iArgs,
        const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
        class UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
    );

protected:
    virtual TArray<TSharedPtr<SWidget>> GenerateMainRowHeaderOptionWidgets() override;

private:
    void OnIsAlphaLockedCheckStateChanged(ECheckBoxState iState);
    ECheckBoxState GetIsAlphaLockedIsChecked() const;

private:
    class UOdysseyAnimationLayerImageRaster* mAnimationLayerImageRaster;
};
