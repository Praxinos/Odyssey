// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Animation/Timeline/Layers/SOdysseyAnimationLayerRow.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLightTableKey.h"

class FOdysseyAnimationEditorTimelinePosition;
class FOdysseyPainterEditor;
/**
 * Implements a layer row widget
 */
class SOdysseyAnimationLayerImageRasterRow
    : public SOdysseyAnimationLayerRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageRasterRow)
        : _PainterEditor(nullptr)
        {}
        SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
        SLATE_ATTRIBUTE(FOdysseyPainterEditor*, PainterEditor)
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
    TAttribute<FOdysseyPainterEditor*> mEditor;
};
