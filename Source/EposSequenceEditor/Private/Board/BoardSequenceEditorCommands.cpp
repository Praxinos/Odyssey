// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Board/BoardSequenceEditorCommands.h"

#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "BoardSequenceEditorCommands"

FBoardSequenceEditorCommands::FBoardSequenceEditorCommands()
    : TCommands<FBoardSequenceEditorCommands>("BoardSequenceEditor" /* must match Set() parameter in style*/, LOCTEXT("BoardSequenceEditorStyle", "Epos Editor (Board)"), NAME_None, FEposSequenceEditorStyle::Get()->GetStyleSetName() )
{
}

void
FBoardSequenceEditorCommands::RegisterCommands()
{
    UI_COMMAND( NewStoryboardWithSettings,   "New Storyboard", "Create a new storyboard with settings", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( ArrangeShotsManually,   "Arrange Shots Manually", "Arrange shots manually", EUserInterfaceActionType::RadioButton, FInputChord() );
    UI_COMMAND( ArrangeShotsOnOneRow,   "Arrange Shots On One Row", "Arrange shots on a single row", EUserInterfaceActionType::RadioButton, FInputChord() );
    UI_COMMAND( ArrangeShotsOnTwoRows,  "Arrange Shots On Two Rows", "Arrange shots on 2 rows and shifted each other", EUserInterfaceActionType::RadioButton, FInputChord() );

    UI_COMMAND( NewSectionWithBoardAtCurrentFrame,  "Create a new board at current frame", "Create a new board inside a new section at the current frame", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( NewSectionWithShotAtCurrentFrame,  "Create a new shot at current frame", "Create a new shot inside a new section at the current frame", EUserInterfaceActionType::Button, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
