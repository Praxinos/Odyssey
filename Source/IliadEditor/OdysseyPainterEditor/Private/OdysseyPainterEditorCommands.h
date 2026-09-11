// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

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

    /** Action to switch to Normal alpha mode */
    TSharedPtr<FUICommandInfo> SetAlphaModeNormal;

    /** Action to switch to Erase alpha mode */
    TSharedPtr<FUICommandInfo> SetAlphaModeErase;

    /** Action to switch to Top alpha mode */
    TSharedPtr<FUICommandInfo> SetAlphaModeTop;

    /** Action to switch to Back alpha mode */
    TSharedPtr<FUICommandInfo> SetAlphaModeBack;

    /** Action to switch to Sub alpha mode */
    TSharedPtr<FUICommandInfo> SetAlphaModeSub;

    /** Action to switch to Add alpha mode */
    TSharedPtr<FUICommandInfo> SetAlphaModeAdd;

    /** Action to switch to Mul alpha mode */
    TSharedPtr<FUICommandInfo> SetAlphaModeMul;

    /** Action to switch to Min alpha mode */
    TSharedPtr<FUICommandInfo> SetAlphaModeMin;

    /** Action to switch to Max alpha mode */
    TSharedPtr<FUICommandInfo> SetAlphaModeMax;

    /** Action to toggle the eraser button */
    TSharedPtr<FUICommandInfo> ToggleEraserButton;

    /** Action to switch to Normal blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeNormal;

    /** Action to switch to Top blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeTop;

    /** Action to switch to Back blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeBack;

    /** Action to switch to Behind blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeBehind;

    /** Action to switch to Dissolve blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeDissolve;

    /** Action to switch to Bayer Dither 8x8 blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeBayerDither8x8;

    /** Action to switch to Darken blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeDarken;

    /** Action to switch to Multiply blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeMultiply;

    /** Action to switch to ColorBurn blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeColorBurn;

    /** Action to switch to LinearBurn blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeLinearBurn;

    /** Action to switch to DarkerColor blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeDarkerColor;

    /** Action to switch to Lighten blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeLighten;

    /** Action to switch to Screen blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeScreen;

    /** Action to switch to ColorDodge blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeColorDodge;

    /** Action to switch to LinearDodge blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeLinearDodge;

    /** Action to switch to LighterColor blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeLighterColor;

    /** Action to switch to Overlay blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeOverlay;

    /** Action to switch to SoftLight blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeSoftLight;

    /** Action to switch to HardLight blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeHardLight;

    /** Action to switch to VividLight blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeVividLight;

    /** Action to switch to LinearLight blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeLinearLight;

    /** Action to switch to PinLight blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModePinLight;

    /** Action to switch to HardMix blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeHardMix;

    /** Action to switch to Phoenix blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModePhoenix;

    /** Action to switch to Reflect blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeReflect;

    /** Action to switch to Glow blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeGlow;

    /** Action to switch to Difference blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeDifference;

    /** Action to switch to Exclusion blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeExclusion;

    /** Action to switch to Add blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeAdd;

    /** Action to switch to Substract blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeSubstract;

    /** Action to switch to Divide blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeDivide;

    /** Action to switch to Average blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeAverage;

    /** Action to switch to Negation blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeNegation;

    /** Action to switch to Hue blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeHue;

    /** Action to switch to Saturation blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeSaturation;

    /** Action to switch to Color blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeColor;

    /** Action to switch to Luminosity blend mode */
    TSharedPtr<FUICommandInfo> SetBlendMode;

    /** Action to switch to blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeLuminosity;

    /** Action to switch to PartialDerivative blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModePartialDerivative;

    /** Action to switch to WhiteOut blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeWhiteOut;

    /** Action to switch to AngleCorrected blend mode */
    TSharedPtr<FUICommandInfo> SetBlendModeAngleCorrected;

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
