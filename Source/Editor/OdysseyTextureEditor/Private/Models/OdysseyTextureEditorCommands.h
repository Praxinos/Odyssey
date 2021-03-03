// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
    //Import Export

    /** Action used to import Textures as Layers */
    TSharedPtr<FUICommandInfo> ImportTexturesAsLayers;

    /** Action used to export Layers as Textures */
    TSharedPtr<FUICommandInfo> ExportLayersAsTextures;

    //Layers Actions

    /** Action for creating a new layer */
    TSharedPtr<FUICommandInfo> CreateNewLayer;

    /** Action for duplicating the current layer */
    TSharedPtr<FUICommandInfo> DuplicateCurrentLayer;

    /** Action for deleting the current layer */
    TSharedPtr<FUICommandInfo> DeleteCurrentLayer;
};
