// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
class FOdysseyAnimationEditorTimelinePosition;
class FOdysseyAnimationCellSelection;

class FOdysseyAnimationTimelineTools
{
public:
    static FOdysseyAnimationTimelineTools& Get();

private:
	FOdysseyAnimationTimelineTools();

public:
	TSharedPtr<FOdysseyAnimationTimelineTool> CreateTool(TSharedRef<FOdysseyAnimationEditorTimelinePosition> iTimelinePosition, TSharedRef<FOdysseyAnimationCellSelection> iTimelineCellSelection) const;
    EOdysseyTimelineTool GetCurrentTool() const;
	void SetCurrentTool(EOdysseyTimelineTool iTool);

private:
	EOdysseyTimelineTool mCurrentTool;
};