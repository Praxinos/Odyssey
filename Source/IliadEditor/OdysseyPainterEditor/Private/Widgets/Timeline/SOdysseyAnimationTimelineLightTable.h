// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "OdysseyRenderingAbility.h"
#include "SOdysseyAnimationTimelineLighttableKey.h"

class UOdysseyAnimationLayer;
class UOdysseyAnimation;
class UOdysseyAnimationCell;
class FOdysseyPainterEditorAnimationTimelinePosition;

class SOdysseyAnimationTimelineLighttable
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineLighttable)
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
