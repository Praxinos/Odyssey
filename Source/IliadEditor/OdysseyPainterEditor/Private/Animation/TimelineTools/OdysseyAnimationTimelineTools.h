// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "CoreMinimal.h"

enum class EOdysseyTimelineTool
{
    None,
    Selection,
    Move,
    Cut
};

class FOdysseyAnimationTimelineSelectionTool;
class FOdysseyAnimationTimelineMoveTool;
class FOdysseyAnimationTimelineCutTool;
class FOdysseyAnimationTimelineTool;
class FOdysseyPainterEditorAnimationTimelinePosition;
class FOdysseyLayerCellSelection;

class FOdysseyAnimationTimelineTools
{
public:
    static FOdysseyAnimationTimelineTools& Get();

private:
    FOdysseyAnimationTimelineTools();

public:
    TSharedPtr<FOdysseyAnimationTimelineTool> CreateTool(TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> iTimelinePosition, TSharedRef<FOdysseyLayerCellSelection> iTimelineCellSelection) const;
    EOdysseyTimelineTool GetCurrentTool() const;
    void SetCurrentTool(EOdysseyTimelineTool iTool);

private:
    EOdysseyTimelineTool mCurrentTool;
};
