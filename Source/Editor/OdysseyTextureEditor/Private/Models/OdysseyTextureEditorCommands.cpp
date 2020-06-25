// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Models/OdysseyTextureEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorCommands"

FOdysseyTextureEditorCommands::FOdysseyTextureEditorCommands()
    : TCommands<FOdysseyTextureEditorCommands>( "OdysseyTextureEditor", NSLOCTEXT( "Contexts", "OdysseyTextureEditor", "Odyssey Texture Editor" ), NAME_None, FEditorStyle::GetStyleSetName() )
{
}

void
FOdysseyTextureEditorCommands::RegisterCommands()
{
    UI_COMMAND( Render3DInRealTime, "Render Real Time ", "Toggles the render in real time of the 3D props which use this texture", EUserInterfaceActionType::ToggleButton, FInputChord() );
    UI_COMMAND( ImportTexturesAsLayers, "Import Textures As Layers", "Import Textures As Layers", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( ExportLayersAsTextures, "Export Layer As Textures", "Export Layer As Textures", EUserInterfaceActionType::Button, FInputChord() );    
}

#undef LOCTEXT_NAMESPACE
