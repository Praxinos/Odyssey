// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Models/OdysseyTextureEditorCommands.h"
#include "OdysseyEditorCommandsMacro.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorCommands"

namespace
{
    const FName ImportExportShortcuts = "Import & Export Shortcuts";
    const FName LayerStackShortcuts = "LayerStack Shortcuts";
}

FOdysseyTextureEditorCommands::FOdysseyTextureEditorCommands()
    : TCommands<FOdysseyTextureEditorCommands>( "IliadTextureEditor", NSLOCTEXT( "Contexts", "IliadTextureEditor", "Iliad Texture Editor" ), NAME_None, FEditorStyle::GetStyleSetName() )
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
}

#undef LOCTEXT_NAMESPACE
