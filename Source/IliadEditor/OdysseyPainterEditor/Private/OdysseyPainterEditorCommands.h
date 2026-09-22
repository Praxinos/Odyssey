// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

#include "OdysseyBlendMode.h"

/**
 * Holds the UI commands for the OdysseyPainterEditorToolkit widget.
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorCommands
    : public TCommands<FOdysseyPainterEditorCommands>
{
public:
    /**
     * Default constructor.
     */
    FOdysseyPainterEditorCommands();

public:
    // TCommands interface
    virtual void RegisterCommands() override;

public:
    //About actions

    /** Action to see the team and links to Odyssey resources */
    TSharedPtr<FUICommandInfo> AboutOdyssey;

    /** Action to go to praxinos' website */
    TSharedPtr<FUICommandInfo> VisitPraxinosWebsite;

    /** Action to go to praxinos' forum Release Notes*/
    TSharedPtr<FUICommandInfo> ManualAndReleaseNotes;

    /** Action to go to the brush pack on the marketplace*/
    TSharedPtr<FUICommandInfo> GetBrushPack;

    /** Action to go to praxinos' Discord server*/
    TSharedPtr<FUICommandInfo> Discord;


    //Uncategorized Actions

    /** Action for clearing the current layer */
    TSharedPtr<FUICommandInfo> ClearCurrentLayer;

    /** Action for selecting the whole layer in a selection */
    TSharedPtr<FUICommandInfo> SelectAll;

    /** Action for clearing the current selection block, if it exists */
    TSharedPtr<FUICommandInfo> ClearCurrentSelection;

    /** Action for copying the current selection block, if it exists */
    TSharedPtr<FUICommandInfo> CopyCurrentSelection;

    /** Action for cuting the current selection block, if it exists */
    TSharedPtr<FUICommandInfo> CutCurrentSelection;

    /** Action for pasting the current selection */
    TSharedPtr<FUICommandInfo> PasteCurrentSelection;

    /** Action for pasting the current selection in a new layer */
    TSharedPtr<FUICommandInfo> PasteCurrentSelectionInNewLayer;

    /** Action for inverting the current raster selection */
    TSharedPtr<FUICommandInfo> InvertSelection;


    //Brush actions

    /** Action to refresh the brush back to its original overidden parameters */
    TSharedPtr<FUICommandInfo> RecreateBrush;

    //Top Bar actions
    /** Action to increase size by 1 */
    TSharedPtr<FUICommandInfo> IncreaseBrushSize;

    /** Action to decrease size by 1 */
    TSharedPtr<FUICommandInfo> DecreaseBrushSize;

    TArray<TPair<TSharedPtr<FUICommandInfo>, EOdysseyBlendMode>> SetToolBlendMode;
    TArray<TPair<TSharedPtr<FUICommandInfo>, EOdysseyColorBlendMode>> SetToolColorBlendMode;
    TArray<TPair<TSharedPtr<FUICommandInfo>, EOdysseyAlphaBlendMode>> SetToolAlphaBlendMode;

    /** Action to toggle the eraser button */
    TSharedPtr<FUICommandInfo> ToggleEraserButton;

    //Switch tablet API

    /** Action to switch tablet API */
    TSharedPtr<FUICommandInfo> SwitchTabletAPI;

    // Tools
    // Generic Tools commands
    TSharedPtr<FUICommandInfo> ActivateColorPickerTool;
    TSharedPtr<FUICommandInfo> ActivateDrawingTool;
    TSharedPtr<FUICommandInfo> ActivateSelectionTool;
    TSharedPtr<FUICommandInfo> ActivatePaintBucketTool;
    TSharedPtr<FUICommandInfo> ActivateTransformTool;
    TSharedPtr<FUICommandInfo> ActivatePrimitiveDrawingTool;
    TSharedPtr<FUICommandInfo> ActivateEraserTool;
    TSharedPtr<FUICommandInfo> ActivateWarpTool;

    TSharedPtr<FUICommandInfo> ActivateTemporaryColorPickerTool;

    TSharedPtr<FUICommandInfo> InactivateTemporaryTool;

    // Specific Tools commands
    TSharedPtr<FUICommandInfo> ActivateRasterDrawingTool;
    TSharedPtr<FUICommandInfo> ActivateRasterEraserTool;
    TSharedPtr<FUICommandInfo> ActivateRasterPaintBucketTool;
    TSharedPtr<FUICommandInfo> ActivateRasterPrimitiveDrawingTool;
    TSharedPtr<FUICommandInfo> ActivateRasterTransformTool;
    TSharedPtr<FUICommandInfo> ActivateVectorEraserTool;
    TSharedPtr<FUICommandInfo> ActivateVectorGridTool;
    TSharedPtr<FUICommandInfo> ActivateVectorPaintBucketTool;
    TSharedPtr<FUICommandInfo> ActivateVectorPathCutTool;
    TSharedPtr<FUICommandInfo> ActivateVectorPathDrawingTool;
    TSharedPtr<FUICommandInfo> ActivateVectorPathEditTool;
    TSharedPtr<FUICommandInfo> ActivateVectorPathPushTool;
    TSharedPtr<FUICommandInfo> ActivateVectorPathSmoothTool;
    TSharedPtr<FUICommandInfo> ActivateVectorPathStitchTool;
    TSharedPtr<FUICommandInfo> ActivateVectorPrimitiveDrawingTool;
    TSharedPtr<FUICommandInfo> ActivateVectorScenePanTool;
    TSharedPtr<FUICommandInfo> ActivateVectorSelectionTool;
    TSharedPtr<FUICommandInfo> ActivateVectorTransformTool;

    TSharedPtr<FUICommandInfo> SetToolRadius;

    // Vector Tools Actions in Object Mode
    TSharedPtr<FUICommandInfo> VectorResetView;
    TSharedPtr<FUICommandInfo> VectorMakePaintGroup;
    TSharedPtr<FUICommandInfo> VectorGroup;
    TSharedPtr<FUICommandInfo> VectorUngroup;
    TSharedPtr<FUICommandInfo> VectorBringForward;
    TSharedPtr<FUICommandInfo> VectorSendBackward;
    TSharedPtr<FUICommandInfo> VectorFlipHorizontal;
    TSharedPtr<FUICommandInfo> VectorFlipVertical;
    TSharedPtr<FUICommandInfo> VectorClearColoring;
    TSharedPtr<FUICommandInfo> VectorApplyTransformations;
    // Vector Tools Actions in vertex Mode
    TSharedPtr<FUICommandInfo> VectorSubdivideSegments;
    TSharedPtr<FUICommandInfo> VectorAlignPointSelection;
    TSharedPtr<FUICommandInfo> VectorUnalignPointSelection;
    TSharedPtr<FUICommandInfo> VectorLockPointSelection;
    TSharedPtr<FUICommandInfo> VectorUnlockPointSelection;

    // Raster Tools Actions
    // Transform Tool specific
    TSharedPtr<FUICommandInfo> RasterTranformToolTogglePerspectiveMode;

    //Layers Actions
    /** Action for creating a new layer */
    TSharedPtr<FUICommandInfo> CreateNewLayer;

    /** Action for changing the current layer's opacity */
    TArray<TSharedPtr<FUICommandInfo>> ChangeLayerOpacity;
};
