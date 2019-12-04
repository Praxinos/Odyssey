// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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
}

#undef LOCTEXT_NAMESPACE
