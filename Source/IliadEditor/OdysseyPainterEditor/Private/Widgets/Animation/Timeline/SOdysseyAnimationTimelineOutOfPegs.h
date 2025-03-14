// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImageRenderingAbility.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineOutOfPegsKey.h"

class UOdysseyAnimationLayer;
class UOdysseyAnimation;
class FOdysseyPainterEditorAnimationTimelinePosition;

class SOdysseyAnimationTimelineOutOfPegs
    : public SCompoundWidget
{
public:
    static inline const float mDesiredHeight = 60.f;

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineOutOfPegs)
    {}
        SLATE_ARGUMENT(TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition)
        SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnActivateOutOfPegs, OnActivateOutOfPegs)
        SLATE_EVENT(FSimpleDelegate, OnInactivateOutOfPegs)
        SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnIsOutOfPegsChecked, OnIsOutOfPegsChecked)
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
