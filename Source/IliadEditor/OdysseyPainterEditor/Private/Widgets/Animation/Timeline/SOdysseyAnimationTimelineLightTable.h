// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "OdysseyRenderingAbility.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLightTableKey.h"

class UOdysseyAnimationLayer;
class UOdysseyAnimation;
class UOdysseyAnimationCell;
class FOdysseyPainterEditorAnimationTimelinePosition;

class SOdysseyAnimationTimelineLightTable
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineLightTable)
    : _CurrentFrame(0)
    {}
        SLATE_ATTRIBUTE(int, CurrentFrame)
        SLATE_ARGUMENT(TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition)
    SLATE_END_ARGS()

    void Construct(
        const FArguments& InArgs,
        UOdysseyAnimationLayer* iLayer
    );

private:
    UOdysseyAnimationCell* GetCurrentCell() const;

private:
    TAttribute<int> mCurrentFrame;
    UOdysseyAnimationLayer* mLayer = nullptr;
};
