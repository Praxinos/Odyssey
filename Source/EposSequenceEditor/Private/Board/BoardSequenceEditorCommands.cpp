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
}

#undef LOCTEXT_NAMESPACE
