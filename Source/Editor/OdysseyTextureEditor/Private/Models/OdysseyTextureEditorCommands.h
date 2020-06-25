// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

/**
 * Holds the UI commands for the OdysseyTextureEditorToolkit widget.
 */
class FOdysseyTextureEditorCommands
    : public TCommands<FOdysseyTextureEditorCommands>
{
public:
    /**
     * Default constructor.
     */
    FOdysseyTextureEditorCommands();

public:
    // TCommands interface
    virtual void RegisterCommands() override;

public:
    /** If enabled, render in real time the 3D props on which the texture is used */
    TSharedPtr<FUICommandInfo> Render3DInRealTime;

    /** Action used to import Textures as Layers */
    TSharedPtr<FUICommandInfo> ImportTexturesAsLayers;

    /** Action used to export Layers as Textures */
    TSharedPtr<FUICommandInfo> ExportLayersAsTextures;
};
