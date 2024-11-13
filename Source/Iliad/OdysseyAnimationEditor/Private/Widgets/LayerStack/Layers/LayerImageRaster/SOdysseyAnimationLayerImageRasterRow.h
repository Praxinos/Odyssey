// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerRow.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableKey.h"

class FOdysseyAnimationEditorTimelinePosition;
class FOdysseyPainterEditor;
/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageRasterRow
    : public SOdysseyAnimationLayerRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageRasterRow)
        : _PainterEditor(nullptr)
        {}
        SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
        SLATE_ATTRIBUTE(TSharedPtr<FOdysseyPainterEditor>, PainterEditor)
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
    TAttribute<TSharedPtr<FOdysseyPainterEditor>> mEditor;
};
