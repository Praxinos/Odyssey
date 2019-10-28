// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

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
    FOdysseyPainterEditorCommands( )
        : TCommands<FOdysseyPainterEditorCommands>("OdysseyPainterEditor", NSLOCTEXT("Contexts", "OdysseyPainterEditor", "Odyssey Painter Editor"), NAME_None, FEditorStyle::GetStyleSetName())
    { }

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

};
