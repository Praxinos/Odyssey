// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditorCommands.h"
#include "Command/OdysseyCommandMacros.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

namespace
{
    const FName ImportExportShortcuts = "Import & Export Shortcuts";
    const FName LayerStackShortcuts = "LayerStack Shortcuts";
    const FName HelpShortcuts = "Help Shortcuts";
}

FOdysseyTextureEditorCommands::FOdysseyTextureEditorCommands()
    : TCommands<FOdysseyTextureEditorCommands>( "IliadTextureEditor", LOCTEXT( "editor-commands.name", "Iliad Texture Editor" ), NAME_None, FAppStyle::GetAppStyleSetName() )
{
    AddBundle(ImportExportShortcuts, LOCTEXT("editor-commands.category.import-export-shortcuts", "Import & Export Shortcuts"));
    AddBundle(LayerStackShortcuts, LOCTEXT("editor-commands.category.layerstack-shortcuts-category", "LayerStack Shortcuts"));
}

void
FOdysseyTextureEditorCommands::RegisterCommands()
{
// Import & Export Shortcuts Category
    UI_BUNDLE_COMMAND( ImportTexturesAsLayers, ImportExportShortcuts, "Import Textures As Layers", "Import several textures from the content-browser as layers",                                               EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::P));
    UI_BUNDLE_COMMAND( ExportLayersAsTextures, ImportExportShortcuts, "Export Layers As Textures", "Export existing layers as textures within the content-browser",                                            EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::E));
    UI_BUNDLE_COMMAND( ExportCurrentLayerAsTexture, ImportExportShortcuts, "Export Current Layer As Texture", "Export current layers as texture within the content-browser",                                   EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ExportTextureToOperatingSystem, ImportExportShortcuts, "Export Texture To Operating System", "Export current texture to your operating system, several file format are available",      EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::F));

// LayerStack Shortcuts Category
    UI_BUNDLE_COMMAND( CreateNewLayer, LayerStackShortcuts, "Create New Layer", "Create new image layer",                                                                                                      EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::N));
    UI_BUNDLE_COMMAND( ChangeLayerOpacity10, LayerStackShortcuts, "Change current Layer Opacity to 10%", "Change current Layer Opacity to 10%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity20, LayerStackShortcuts, "Change current Layer Opacity to 20%", "Change current Layer Opacity to 20%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity30, LayerStackShortcuts, "Change current Layer Opacity to 30%", "Change current Layer Opacity to 30%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity40, LayerStackShortcuts, "Change current Layer Opacity to 40%", "Change current Layer Opacity to 40%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity50, LayerStackShortcuts, "Change current Layer Opacity to 50%", "Change current Layer Opacity to 50%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity60, LayerStackShortcuts, "Change current Layer Opacity to 60%", "Change current Layer Opacity to 60%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity70, LayerStackShortcuts, "Change current Layer Opacity to 70%", "Change current Layer Opacity to 70%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity80, LayerStackShortcuts, "Change current Layer Opacity to 80%", "Change current Layer Opacity to 80%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity90, LayerStackShortcuts, "Change current Layer Opacity to 90%", "Change current Layer Opacity to 90%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity100, LayerStackShortcuts, "Change current Layer Opacity to 100%", "Change current Layer Opacity to 100%",                                                             EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
