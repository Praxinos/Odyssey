// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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

    /** Action to go to praxinos' forum */
    TSharedPtr<FUICommandInfo> VisitPraxinosForums;
    

    //Undo Redo

    /** Action for undoing a stroke in ILIAD*/
    TSharedPtr<FUICommandInfo> Undo;
    
    /** Action for redoing a stroke in ILIAD*/
    TSharedPtr<FUICommandInfo> Redo;


    //Import Export

    /** Action used to import Textures as Layers */
    TSharedPtr<FUICommandInfo> ImportTexturesAsLayers;

    /** Action used to export Layers as Textures */
    TSharedPtr<FUICommandInfo> ExportLayersAsTextures;


    //Tools Actions

    /** Action for filling the current layer with the current color */
    TSharedPtr<FUICommandInfo> FillCurrentLayer;

    /** Action for clearing the current layer */
    TSharedPtr<FUICommandInfo> ClearCurrentLayer;

    
    //Layers Actions

    /** Action for creating a new layer */
    TSharedPtr<FUICommandInfo> CreateNewLayer;

    /** Action for duplicating the current layer */
    TSharedPtr<FUICommandInfo> DuplicateCurrentLayer;

    /** Action for deleting the current layer */
    TSharedPtr<FUICommandInfo> DeleteCurrentLayer;

    
    //Viewport Actions

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
    TSharedPtr<FUICommandInfo> ZoomIn; 

    /** Action to set zoom out*/
    TSharedPtr<FUICommandInfo> ZoomOut; 


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


    //Switch tablet API

    /** Action to switch tablet API */
    TSharedPtr<FUICommandInfo> SwitchTabletAPI; 

};
