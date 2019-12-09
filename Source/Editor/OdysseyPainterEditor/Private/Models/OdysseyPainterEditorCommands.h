// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
    /** If enabled, the texture will be scaled to fit the viewport */
    TSharedPtr<FUICommandInfo> FitToViewport;

    /** Sets the checkered background pattern */
    TSharedPtr<FUICommandInfo> CheckeredBackground;

    /** Sets the checkered background pattern (filling the view port) */
    TSharedPtr<FUICommandInfo> CheckeredBackgroundFill;

    /** Sets the solid color background */
    TSharedPtr<FUICommandInfo> SolidBackground;

    /** If enabled, a border is drawn around the texture */
    TSharedPtr<FUICommandInfo> TextureBorder;

    /** If enabled, render in real time the 3D props on which the texture is used */
    TSharedPtr<FUICommandInfo> Render3DInRealTime;

    /** Action used to import Textures as Layers */
    TSharedPtr<FUICommandInfo> ImportTexturesAsLayers;

    /** Action used to export Layers as Textures */
    TSharedPtr<FUICommandInfo> ExportLayersAsTextures;

    /** Action to see the team and links to ILIAD resources */
    TSharedPtr<FUICommandInfo> AboutIliad;

    /** Action to go to praxinos' website */
    TSharedPtr<FUICommandInfo> VisitPraxinosWebsite;

    /** Action to go to praxinos' forum */
    TSharedPtr<FUICommandInfo> VisitPraxinosForums;
    
    /** Action for undoing a stroke in ILIAD*/
    TSharedPtr<FUICommandInfo> Undo;
    
    /** Action for redoing a stroke in ILIAD*/
    TSharedPtr<FUICommandInfo> Redo;

};
