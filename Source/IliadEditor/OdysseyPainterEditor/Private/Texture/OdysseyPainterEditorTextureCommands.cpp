// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorTextureCommands.h"
#include "Command/OdysseyCommandMacros.h"
#include "OdysseyStyle.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

namespace
{
    const FName ImportExportShortcuts = "Import & Export Shortcuts";
}

FOdysseyPainterEditorTextureCommands::FOdysseyPainterEditorTextureCommands()
    : TCommands<FOdysseyPainterEditorTextureCommands>( "OdysseyPainterEditorTextureCommands", LOCTEXT( "editor-commands.name", "Odyssey Texture Editor" ), NAME_None, FOdysseyStyle::GetStyleSetName() )
{
    AddBundle( ImportExportShortcuts, LOCTEXT("editor-commands.category.import-export-shortcuts", "Import & Export Shortcuts"));
}

void
FOdysseyPainterEditorTextureCommands::RegisterCommands()
{
// Import & Export Shortcuts Category
    UI_BUNDLE_COMMAND( ImportTextures, ImportExportShortcuts, "Import Textures", "Import several textures from the content-browser as layers",                                               EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ImportImages, ImportExportShortcuts, "Import Images", "Import several images from your operating system as layers",                                               EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ExportLayersAsTextures, ImportExportShortcuts, "Export Layers As Textures", "Export existing layers as textures within the content-browser",                                            EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::E));
    UI_BUNDLE_COMMAND( ExportLayersAsImages, ImportExportShortcuts, "Export Layers As Images", "Export existing layers as images in your operating system",                                            EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::F));
}

#undef LOCTEXT_NAMESPACE
