// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditorCommands.h"
#include "Command/OdysseyCommandMacros.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorCommands"

namespace
{
    const FName ImportExportShortcuts = "Import & Export Shortcuts";
    const FName LayerStackShortcuts = "LayerStack Shortcuts";
}

FOdysseyTextureEditorCommands::FOdysseyTextureEditorCommands()
    : TCommands<FOdysseyTextureEditorCommands>( "IliadTextureEditor", NSLOCTEXT( "Contexts", "IliadTextureEditor", "Iliad Texture Editor" ), NAME_None, FAppStyle::GetAppStyleSetName() )
{
    AddBundle(ImportExportShortcuts, LOCTEXT("ImportExportShortcutsCategory", "Import & Export Shortcuts"));
    AddBundle(LayerStackShortcuts, LOCTEXT("LayerStackShortcutsCategory", "LayerStack Shortcuts"));
}

void
FOdysseyTextureEditorCommands::RegisterCommands()
{
// Import & Export Shortcuts Category
    UI_CMD( ImportTexturesAsLayers, ImportExportShortcuts, "Import Textures As Layers", "Import several textures from the content-browser as layers",                                               EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::P));
    UI_CMD( ExportLayersAsTextures, ImportExportShortcuts, "Export Layers As Textures", "Export existing layers as textures within the content-browser",                                            EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::E));
    UI_CMD( ExportCurrentLayerAsTexture, ImportExportShortcuts, "Export Current Layer As Texture", "Export current layers as texture within the content-browser",                                   EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ExportTextureToOperatingSystem, ImportExportShortcuts, "Export Texture To Operating System", "Export current texture to your operating system, several file format are available",      EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::F));

// LayerStack Shortcuts Category
    UI_CMD( CreateNewLayer, LayerStackShortcuts, "Create New Layer", "Create new image layer",                                                                                                      EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::N));
    UI_CMD( DuplicateCurrentLayer, LayerStackShortcuts, "Duplicate Current Layer", "Duplicate current layer",                                                                                       EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::D));
    UI_CMD( DeleteCurrentLayer, LayerStackShortcuts, "Delete Current Layer", "Delete current layer",                                                                                                EUserInterfaceActionType::Button, FInputChord(EModifierKey::Shift, EKeys::Delete));
    UI_CMD( ChangeLayerOpacity10, LayerStackShortcuts, "Change current Layer Opacity to 10%", "Change current Layer Opacity to 10%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity20, LayerStackShortcuts, "Change current Layer Opacity to 20%", "Change current Layer Opacity to 20%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity30, LayerStackShortcuts, "Change current Layer Opacity to 30%", "Change current Layer Opacity to 30%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity40, LayerStackShortcuts, "Change current Layer Opacity to 40%", "Change current Layer Opacity to 40%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity50, LayerStackShortcuts, "Change current Layer Opacity to 50%", "Change current Layer Opacity to 50%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity60, LayerStackShortcuts, "Change current Layer Opacity to 60%", "Change current Layer Opacity to 60%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity70, LayerStackShortcuts, "Change current Layer Opacity to 70%", "Change current Layer Opacity to 70%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity80, LayerStackShortcuts, "Change current Layer Opacity to 80%", "Change current Layer Opacity to 80%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity90, LayerStackShortcuts, "Change current Layer Opacity to 90%", "Change current Layer Opacity to 90%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity100, LayerStackShortcuts, "Change current Layer Opacity to 100%", "Change current Layer Opacity to 100%",                                                             EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
