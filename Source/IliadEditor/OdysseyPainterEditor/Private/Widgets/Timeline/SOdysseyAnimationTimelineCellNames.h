// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "OdysseyRenderingAbility.h"

class UOdysseyLayerCell;
class FOdysseyPainterEditorAnimationTimelinePosition;
class SHorizontalBox;

class SOdysseyAnimationTimelineCellNames
    : public SCompoundWidget
{
    SLATE_DECLARE_WIDGET(SOdysseyAnimationTimelineCellNames, SCompoundWidget)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineCellNames )
    {}
        SLATE_ATTRIBUTE(TArray<UOdysseyLayerCell*>, Cells)
        SLATE_ARGUMENT(TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition)
    SLATE_END_ARGS()

    SOdysseyAnimationTimelineCellNames();
    void Construct(const FArguments& InArgs);

private:
    void RebuildRow();
    void OnCellsChanged();

private:
    TSlateAttribute<TArray<UOdysseyLayerCell*>> mCells;
    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
    TSharedPtr<SHorizontalBox> mRowWidget;
};
