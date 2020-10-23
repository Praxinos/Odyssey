// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoardSequenceEditorCommands.h"

#include "Styles/EposEditorStyle.h"

#define LOCTEXT_NAMESPACE "BoardSequenceEditorCommands"

FBoardSequenceEditorCommands::FBoardSequenceEditorCommands()
	: TCommands<FBoardSequenceEditorCommands>("BoardSequenceEditor" /* must match Set() parameter in style*/, LOCTEXT("BoardSequenceEditorStyle", "Epos Editor"), NAME_None, FEposEditorStyle::Get()->GetStyleSetName() )
{
}

void
FBoardSequenceEditorCommands::RegisterCommands()
{
    UI_COMMAND( ArrangeShots,           "Arrange Shots", "Arrange shots in multiple ways", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( ArrangeShotsOnOneRow,   "Arrange Shots On One Row", "Arrange shots on a single row", EUserInterfaceActionType::RadioButton, FInputChord() );
    UI_COMMAND( ArrangeShotsOnTwoRows,  "Arrange Shots On Two Rows", "Arrange shots on 2 rows and shifted each other", EUserInterfaceActionType::RadioButton, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
