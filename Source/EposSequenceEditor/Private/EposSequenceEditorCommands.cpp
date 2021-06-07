// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposSequenceEditorCommands.h"

#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "EposSequenceEditorCommands"

FEposSequenceEditorCommands::FEposSequenceEditorCommands()
    : TCommands<FEposSequenceEditorCommands>("EposSequenceEditor" /* must match Set() parameter in style*/, LOCTEXT("EposSequenceEditorStyle", "Epos Editor (Epos)"), NAME_None, FEposSequenceEditorStyle::Get()->GetStyleSetName() )
{
}

void
FEposSequenceEditorCommands::RegisterCommands()
{
    UI_COMMAND( ToggleStoryboardViewportCommand, "Storyboard Viewport", "A viewport layout tailored to storyboard preview", EUserInterfaceActionType::RadioButton, FInputChord() );

    UI_COMMAND( GotoPraxinos, "Praxinos website...", "Go to Praxinos website", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoForum, "Praxinos forum...", "Go to Praxinos forum", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoUserDocumentation, "Epos User Documentation...", "Go to User Documentation", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( OpenAboutWindow, "About Epos...", "Open the About window", EUserInterfaceActionType::Button, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
