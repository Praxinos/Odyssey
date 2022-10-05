// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

    /** Test ExportTextureToOperatingSystem */
    TSharedPtr<FUICommandInfo> ExportTextureToOperatingSystem;

    /** Action used to import Textures as Layers */
    TSharedPtr<FUICommandInfo> ImportTexturesAsLayers;

    /** Action used to export Layers as Textures */
    TSharedPtr<FUICommandInfo> ExportLayersAsTextures;

    /** Action used to export current Layer as Texture */
    TSharedPtr<FUICommandInfo> ExportCurrentLayerAsTexture;

    //Layers Actions

    /** Action for creating a new layer */
    TSharedPtr<FUICommandInfo> CreateNewLayer;

    /** Action for duplicating the current layer */
    TSharedPtr<FUICommandInfo> DuplicateCurrentLayer;

    /** Action for deleting the current layer */
    TSharedPtr<FUICommandInfo> DeleteCurrentLayer;

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
};
