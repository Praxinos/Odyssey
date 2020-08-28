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
    UI_COMMAND(AddNewKeyFrame, "Add Key Frame", "Adds a new key frame to the flipbook.", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddNewKeyFrameBefore, "Insert Key Frame Before", "Adds a new key frame to the flipbook before the selection.", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddNewKeyFrameAfter, "Insert Key Frame After", "Adds a new key frame to the flipbook after the selection.", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(PlayForward, "Play Forward", "Plays the Flipbook forward", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(PlayBackward, "Play Backward", "Plays the Flipbook backward", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(Pause, "Pause", "Pauses the Flipbook playback", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(Stop, "Stop", "Stops the Flipbook playback", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(PreviousFrame, "Go To Previous Frame", "Moves the playback position to the previous frame", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(NextFrame, "Go To Next Frame", "Moves the playback position to the next frame", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(PreviousKeyFrame, "Go To Previous Key Frame", "Moves the playback position to the previous key frame", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(NextKeyFrame, "Got To Next Key Frame", "Moves the playback position to the next frame", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(FirstFrame, "Go To First Frame", "Moves the playback position to the first frame", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(LastFrame, "Go To Last Frame", "Moves the playback position to the last frame", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(ToggleLooping, "Toggle Looping", "Toggles the playback looping mode", EUserInterfaceActionType::ToggleButton, FInputChord());

	//UI_COMMAND(PickNewSpriteForKeyFrame, "Pick New Sprite", "Picks a new sprite for this key frame.", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(EditSpriteForKeyFrame, "Edit Sprite", "Opens the sprite for this key frame in the Sprite Editor.", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ShowSpriteInContentBrowser, "Show in Content Browser", "Shows the sprite for this key frame in the Content Browser.", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
