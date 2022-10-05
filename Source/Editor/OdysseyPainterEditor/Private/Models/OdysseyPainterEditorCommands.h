// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

/**
 * Holds the UI commands for the OdysseyPainterEditorToolkit widget.
 */
class FOdysseyPainterEditorCommands
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

    /** Action to see the team and links to ILIAD resources */
    TSharedPtr<FUICommandInfo> AboutIliad;

    /** Action to go to praxinos' website */
    TSharedPtr<FUICommandInfo> VisitPraxinosWebsite;

    /** Action to go to praxinos' forum Release Notes*/
    TSharedPtr<FUICommandInfo> ManualAndReleaseNotes;

    /** Action to go to the brush pack on the marketplace*/
    TSharedPtr<FUICommandInfo> GetBrushPack;

    /** Action to go to praxinos' Discord server*/
    TSharedPtr<FUICommandInfo> Discord;

    //Undo Redo

    /** Action for undoing a stroke in ILIAD*/
    TSharedPtr<FUICommandInfo> Undo;
    
    /** Action for redoing a stroke in ILIAD*/
    TSharedPtr<FUICommandInfo> Redo;
    
    /** Action for redoing a stroke in ILIAD*/
    TSharedPtr<FUICommandInfo> ClearUndo;


    //Tools Actions

    /** Action for filling the current layer with the current color */
    TSharedPtr<FUICommandInfo> FillCurrentLayer;

    /** Action for clearing the current layer */
    TSharedPtr<FUICommandInfo> ClearCurrentLayer;

    
    //Viewport Actions

    /** Action to pan the viewport */
    TSharedPtr<FUICommandInfo> PanViewport;

    /** Action to rotate the viewport*/
    TSharedPtr<FUICommandInfo> RotateViewport;

    /** Action to zoom the viewport */
    TSharedPtr<FUICommandInfo> ZoomViewport;

    /** Action to pick a color in the viewport */
    TSharedPtr<FUICommandInfo> PickColorInViewport;

    /** Action to reset the viewport to its original position */
    TSharedPtr<FUICommandInfo> ResetViewportPosition;

    /** Action to reset the viewport rotation */
    TSharedPtr<FUICommandInfo> ResetViewportRotation; 

    /** Action to rotate the viewport left */
    TSharedPtr<FUICommandInfo> RotateViewportLeft; 

    /** Action to rotate the viewport right */
    TSharedPtr<FUICommandInfo> RotateViewportRight; 

    /** Action to set zoom pourcentage at 10% */
    TSharedPtr<FUICommandInfo> SetZoom10Percent; 

    /** Action to set zoom pourcentage at 20% */
    TSharedPtr<FUICommandInfo> SetZoom20Percent; 

    /** Action to set zoom pourcentage at 30% */
    TSharedPtr<FUICommandInfo> SetZoom30Percent; 

    /** Action to set zoom pourcentage at 40% */
    TSharedPtr<FUICommandInfo> SetZoom40Percent; 

    /** Action to set zoom pourcentage at 50% */
    TSharedPtr<FUICommandInfo> SetZoom50Percent; 

    /** Action to set zoom pourcentage at 60% */
    TSharedPtr<FUICommandInfo> SetZoom60Percent; 

    /** Action to set zoom pourcentage at 70% */
    TSharedPtr<FUICommandInfo> SetZoom70Percent; 

    /** Action to set zoom pourcentage at 80% */
    TSharedPtr<FUICommandInfo> SetZoom80Percent; 

    /** Action to set zoom pourcentage at 90% */
    TSharedPtr<FUICommandInfo> SetZoom90Percent; 

    /** Action to set zoom pourcentage at 100% */
    TSharedPtr<FUICommandInfo> SetZoom100Percent; 

    /** Action to set zoom at fit to screen */
    TSharedPtr<FUICommandInfo> SetZoomFitScreen; 

    /** Action to zoom in*/
    TSharedPtr<FUICommandInfo> ZoomInExponential; 

    /** Action to set zoom out*/
    TSharedPtr<FUICommandInfo> ZoomOutExponential; 


    //Brush actions

    /** Action to refresh the brush back to its original overidden parameters */
    TSharedPtr<FUICommandInfo> RefreshBrush;

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

};
