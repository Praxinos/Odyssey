// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

/**
 * Holds the UI commands for the OdysseyAnimationEditorToolkit widget.
 */
class FOdysseyAnimationEditorCommands
    : public TCommands<FOdysseyAnimationEditorCommands>
{
public:
    /**
     * Default constructor.
     */
    FOdysseyAnimationEditorCommands();

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

    /** Action for creating a new animation layer image raster */
    TSharedPtr<FUICommandInfo> CreateNewAnimationLayerImageRaster;

    /** Action for deleting the current layer */
    TSharedPtr<FUICommandInfo> ChangeLayerOpacity10;

    /** Action for deleting the current layer */
    TSharedPtr<FUICommandInfo> ChangeLayerOpacity20;

    /** Action for deleting the current layer */
    TSharedPtr<FUICommandInfo> ChangeLayerOpacity30;

    /** Action for deleting the current layer */
    TSharedPtr<FUICommandInfo> ChangeLayerOpacity40;

    /** Action for deleting the current layer */
    TSharedPtr<FUICommandInfo> ChangeLayerOpacity50;

    /** Action for deleting the current layer */
    TSharedPtr<FUICommandInfo> ChangeLayerOpacity60;

    /** Action for deleting the current layer */
    TSharedPtr<FUICommandInfo> ChangeLayerOpacity70;

    /** Action for deleting the current layer */
    TSharedPtr<FUICommandInfo> ChangeLayerOpacity80;

    /** Action for deleting the current layer */
    TSharedPtr<FUICommandInfo> ChangeLayerOpacity90;

    /** Action for deleting the current layer */
    TSharedPtr<FUICommandInfo> ChangeLayerOpacity100;

    TSharedPtr<FUICommandInfo> ConvertToStaggerCell;
    TSharedPtr<FUICommandInfo> ConvertToRasterCell;
    
	// Cross Fade the selected cells
	TSharedPtr<FUICommandInfo> CrossFade;

    TSharedPtr<FUICommandInfo> ActivateTimelineSelectionTool;
    TSharedPtr<FUICommandInfo> ActivateTimelineMoveTool;
    TSharedPtr<FUICommandInfo> ActivateTimelineCutTool;
    TSharedPtr<FUICommandInfo> HoldActivateTimelineSelectionTool;
    TSharedPtr<FUICommandInfo> HoldActivateTimelineMoveTool;
    TSharedPtr<FUICommandInfo> HoldActivateTimelineCutTool;
    TSharedPtr<FUICommandInfo> BreakCell;
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

    TSharedPtr<FUICommandInfo> IncreaseCellLength;
    TSharedPtr<FUICommandInfo> DecreaseCellLength;
    TSharedPtr<FUICommandInfo> SetCellLength;

    /** Action to pan/Zoom the timeline */
    TSharedPtr<FUICommandInfo> PanZoomTimeline;

    /** Flip System */
    TArray<TSharedPtr<FUICommandInfo>> Flip;
};
