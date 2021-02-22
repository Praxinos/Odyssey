// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Models/OdysseyTextureEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorCommands"

FOdysseyTextureEditorCommands::FOdysseyTextureEditorCommands()
    : TCommands<FOdysseyTextureEditorCommands>( "IliadTextureEditor", NSLOCTEXT( "Contexts", "IliadTextureEditor", "Iliad Texture Editor" ), NAME_None, FEditorStyle::GetStyleSetName() )
{
}

void
FOdysseyTextureEditorCommands::RegisterCommands()
{
    UI_COMMAND(ImportTexturesAsLayers, "Import Textures As Layers", "Import Textures As Layers", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::P));
    UI_COMMAND(ExportLayersAsTextures, "Export Layer As Textures", "Export Layers As Textures", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::E));

    UI_COMMAND(CreateNewLayer, "Create New Layer", "Create new image layer", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::N));
    UI_COMMAND(DuplicateCurrentLayer, "Duplicate Current Layer", "Duplicate current layer", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::D));
    UI_COMMAND(DeleteCurrentLayer, "Delete Current Layer", "Delete current layer", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Shift, EKeys::Delete));
}

#undef LOCTEXT_NAMESPACE
