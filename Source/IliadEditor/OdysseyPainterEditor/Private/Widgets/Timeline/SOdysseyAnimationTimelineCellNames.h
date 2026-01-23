// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyRenderingAbility.h"

class UOdysseyAnimationLayer;
class UOdysseyAnimation;
class UOdysseyAnimationCell;
class FOdysseyPainterEditorAnimationTimelinePosition;
class SHorizontalBox;

class SOdysseyAnimationTimelineCellNames
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineCellNames )
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
    //UOdysseyAnimationCell* GetCurrentCell() const;

    void RebuildRow();

    void OnCellsChanged();

private:
    //TAttribute<int> mCurrentFrame;
    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
    UOdysseyAnimationLayer* mLayer = nullptr;

    TSharedPtr<SHorizontalBox> mRowWidget;
};
