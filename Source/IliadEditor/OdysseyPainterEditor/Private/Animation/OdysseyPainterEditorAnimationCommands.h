// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

class FOdysseyPainterEditorAnimationCommands
    : public TCommands<FOdysseyPainterEditorAnimationCommands>
{
public:
    /**
     * Default constructor.
     */
    FOdysseyPainterEditorAnimationCommands();

public:
    // TCommands interface
    virtual void RegisterCommands() override;

public:
    //Import Export

    /** Action used to import Textures as Layers */
    TSharedPtr<FUICommandInfo> ImportTextureSequence;

    /** Action used to import Images as Layers */
    TSharedPtr<FUICommandInfo> ImportImageSequence;

    /** Action used to export the animation as an image sequence */
    TSharedPtr<FUICommandInfo> ExportImageSequence;

    /** Action used to export the animation as an image sequence */
    TSharedPtr<FUICommandInfo> ExportAsFlipbook;

    //Layers Actions
    TSharedPtr<FUICommandInfo> ConvertToStaggerCell;
    TSharedPtr<FUICommandInfo> ConvertToReferenceCells;

    // Cross Fade the selected cells
    TSharedPtr<FUICommandInfo> CrossFade;

    TSharedPtr<FUICommandInfo> ActivateTimelineSelectionTool;
    TSharedPtr<FUICommandInfo> ActivateTimelineMoveTool;
    TSharedPtr<FUICommandInfo> ActivateTimelineCutTool;
    TSharedPtr<FUICommandInfo> HoldActivateTimelineSelectionTool;
    TSharedPtr<FUICommandInfo> HoldActivateTimelineMoveTool;
    TSharedPtr<FUICommandInfo> HoldActivateTimelineCutTool;
    TSharedPtr<FUICommandInfo> BreakCell;
    TSharedPtr<FUICommandInfo> BreakAndClearCell;
    TSharedPtr<FUICommandInfo> RemoveCellMark;
    TArray<TSharedPtr<FUICommandInfo>> SetCellMark;

    //Timeline Navigation Commands
    TSharedPtr<FUICommandInfo> NavigateToNextCell;
    TSharedPtr<FUICommandInfo> NavigateToPreviousCell;
    TSharedPtr<FUICommandInfo> NavigateToNextFrame;
    TSharedPtr<FUICommandInfo> NavigateToPreviousFrame;
    TSharedPtr<FUICommandInfo> NavigateToAnimationFirstFrame;
    TSharedPtr<FUICommandInfo> NavigateToAnimationLastFrame;
    TSharedPtr<FUICommandInfo> Play;
    TSharedPtr<FUICommandInfo> PlayStop;
    TSharedPtr<FUICommandInfo> PlayReversed;
    TSharedPtr<FUICommandInfo> Stop;
    TSharedPtr<FUICommandInfo> ActivateLooping;
    TSharedPtr<FUICommandInfo> InactivateLooping;
    TSharedPtr<FUICommandInfo> ToggleLooping;

    TSharedPtr<FUICommandInfo> IncreaseCellExposure;
    TSharedPtr<FUICommandInfo> DecreaseCellExposure;
    TSharedPtr<FUICommandInfo> SetCellExposure;

    /** Action to pan/Zoom the timeline */
    TSharedPtr<FUICommandInfo> PanZoomTimeline;

    /** Flip System */
    TArray<TSharedPtr<FUICommandInfo>> Flip;
};
