// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Models/OdysseyFlipbookEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorCommands"

FOdysseyFlipbookEditorCommands::FOdysseyFlipbookEditorCommands()
    : TCommands<FOdysseyFlipbookEditorCommands>( "OdysseyFlipbookEditor", NSLOCTEXT( "Contexts", "OdysseyFlipbookEditor", "Odyssey Flipbook Editor" ), NAME_None, FEditorStyle::GetStyleSetName() )
{
}

void
FOdysseyFlipbookEditorCommands::RegisterCommands()
{
    UI_COMMAND( Render3DInRealTime, "Render Real Time ", "Toggles the render in real time of the 3D props which use this Flipbook", EUserInterfaceActionType::ToggleButton, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
