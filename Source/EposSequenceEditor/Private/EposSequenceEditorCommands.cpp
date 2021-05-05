// IDDN FR.001.250001.004.S.X.2019.000.00000
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

    UI_COMMAND( OpenAboutWindow, "About Epos...", "Open the About window", EUserInterfaceActionType::Button, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
