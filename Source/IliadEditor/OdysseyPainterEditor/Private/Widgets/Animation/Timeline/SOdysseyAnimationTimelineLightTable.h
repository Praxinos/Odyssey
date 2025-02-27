// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImageRenderingAbility.h"
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
    {}
        SLATE_ARGUMENT(TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition)
    SLATE_END_ARGS()

    void Construct(
        const FArguments& InArgs,
        UOdysseyAnimationLayer* iLayer
    );

private:
    void OnCurrentFrameChanged(UOdysseyAnimation* iAnimation);
    void OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent);
    void Update();

private:
    UOdysseyAnimationLayer* mLayer = nullptr;
    UOdysseyAnimationCell* mCurrentCell = nullptr;
};
