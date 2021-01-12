// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorCommands"

void FOdysseyViewportDrawingEditorCommands::RegisterCommands()
{
    UI_COMMAND(NextTexture,"Next Texture","Cycle To Next Texture",EUserInterfaceActionType::Button,FInputChord(EKeys::Period));
    Commands.Add(NextTexture);
    UI_COMMAND(PreviousTexture,"Previous Texture","Cycle To Previous Texture",EUserInterfaceActionType::Button,FInputChord(EKeys::Comma));
    Commands.Add(PreviousTexture);

	UI_COMMAND(SaveTexturePaint, "Save Texture Paint", "Saves the Modified Textures for the selected Mesh Components", EUserInterfaceActionType::Button, FInputChord());
	Commands.Add(SaveTexturePaint);

	UI_COMMAND( SetOdysseyBrushSettingsView, "BrushSettings", "Brush Settings", EUserInterfaceActionType::ToggleButton, FInputChord() );
	UI_COMMAND( SetOdysseyStrokeOptionsView, "StrokeOptions", "Stroke Options", EUserInterfaceActionType::ToggleButton, FInputChord() );
	UI_COMMAND( SetOdysseyLayerStackView, "LayerStack", "Layer Stack", EUserInterfaceActionType::ToggleButton, FInputChord() );
	UI_COMMAND( SetOdysseyToolsView, "Tools", "Tools", EUserInterfaceActionType::ToggleButton, FInputChord() );
}

#undef LOCTEXT_NAMESPACE

