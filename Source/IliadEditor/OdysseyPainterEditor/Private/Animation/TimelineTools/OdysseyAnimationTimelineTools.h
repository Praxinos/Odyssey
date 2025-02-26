// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
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
class FOdysseyPainterEditorAnimationTImelinePosition;
class FOdysseyAnimationCellSelection;

class FOdysseyAnimationTimelineTools
{
public:
    static FOdysseyAnimationTimelineTools& Get();

private:
    FOdysseyAnimationTimelineTools();

public:
    TSharedPtr<FOdysseyAnimationTimelineTool> CreateTool(TSharedRef<FOdysseyPainterEditorAnimationTImelinePosition> iTimelinePosition, TSharedRef<FOdysseyAnimationCellSelection> iTimelineCellSelection) const;
    EOdysseyTimelineTool GetCurrentTool() const;
    void SetCurrentTool(EOdysseyTimelineTool iTool);

private:
    EOdysseyTimelineTool mCurrentTool;
};
