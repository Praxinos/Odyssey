// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Models/OdysseyPainterEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorCommands"

FOdysseyPainterEditorCommands::FOdysseyPainterEditorCommands()
    : TCommands<FOdysseyPainterEditorCommands>( "OdysseyPainterEditor", NSLOCTEXT( "Contexts", "OdysseyPainterEditor", "Odyssey Painter Editor" ), NAME_None, FEditorStyle::GetStyleSetName() )
{
}

void
FOdysseyPainterEditorCommands::RegisterCommands()
{
    UI_COMMAND( CheckeredBackground, "Checkered", "Checkered background pattern behind the texture", EUserInterfaceActionType::RadioButton, FInputChord() );
    UI_COMMAND( CheckeredBackgroundFill, "Checkered (Fill)", "Checkered background pattern behind the entire viewport", EUserInterfaceActionType::RadioButton, FInputChord() );
    UI_COMMAND( FitToViewport, "Scale To Fit", "If enabled, the texture will be scaled to fit the viewport", EUserInterfaceActionType::ToggleButton, FInputChord() );
    UI_COMMAND( SolidBackground, "Solid Color", "Solid color background", EUserInterfaceActionType::RadioButton, FInputChord() );
    UI_COMMAND( TextureBorder, "Draw Border", "If enabled, a border is drawn around the texture", EUserInterfaceActionType::ToggleButton, FInputChord() );
    UI_COMMAND( Render3DInRealTime, "Render Real Time ", "Toggles the render in real time of the 3D props which use this texture", EUserInterfaceActionType::ToggleButton, FInputChord() );

    UI_COMMAND( ImportTexturesAsLayers, "Import Textures As Layers", "Import Textures As Layers", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( ExportLayersAsTextures, "Export Layer As Textures", "Export Layer As Textures", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( AboutIliad, "About ILIAD", "About ILIAD", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( VisitPraxinosWebsite, "Praxinos Website...", "Praxinos Website...", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( VisitPraxinosForums, "Praxinos Forums...", "Praxinos Forums...", EUserInterfaceActionType::Button, FInputChord() );
    
#if PLATFORM_MAC
    UI_COMMAND( Undo, "Undo Iliad", "Undo Iliad", EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::Z ) );
    UI_COMMAND( Redo, "Redo Iliad", "Redo Iliad", EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::Y ) );
#else
    UI_COMMAND( Undo, "Undo Iliad", "Undo Iliad", EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::Z ) );
    UI_COMMAND( Redo, "Redo Iliad", "Redo Iliad", EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::Y ) );
#endif
    
}

#undef LOCTEXT_NAMESPACE
