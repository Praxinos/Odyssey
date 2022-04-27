// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EposTracksEditorCommands.h"

#include "Styles/EposTracksEditorStyle.h"

#define LOCTEXT_NAMESPACE "EposTracksEditorCommands"

FEposTracksEditorCommands::FEposTracksEditorCommands()
    : TCommands<FEposTracksEditorCommands>("EposTracksEditorCommands" /* must match Set() parameter in style*/, LOCTEXT("EposTracksEditorStyle", "Epos Tracks Editor"), NAME_None, FEposTracksEditorStyle::Get().GetStyleSetName() )
{
}

void
FEposTracksEditorCommands::RegisterCommands()
{
    UI_COMMAND( ArrangeShotsManually,   "Arrange Shots Manually", "Arrange shots manually", EUserInterfaceActionType::RadioButton, FInputChord() );
    UI_COMMAND( ArrangeShotsOnOneRow,   "Arrange Shots On One Row", "Arrange shots on a single row", EUserInterfaceActionType::RadioButton, FInputChord() );
    UI_COMMAND( ArrangeShotsOnTwoRows,  "Arrange Shots On Two Rows", "Arrange shots on 2 rows and shifted each other", EUserInterfaceActionType::RadioButton, FInputChord() );

    UI_COMMAND( NewSectionWithBoardAtCurrentFrame, "New Board", "Create a new board inside a new section at the current frame", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( NewSectionWithShotAtCurrentFrame, "New Shot", "Create a new shot inside a new section at the current frame", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( NewSectionWithNoteAtCurrentFrame, "New Note", "Create a new note at the current frame", EUserInterfaceActionType::Button, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
