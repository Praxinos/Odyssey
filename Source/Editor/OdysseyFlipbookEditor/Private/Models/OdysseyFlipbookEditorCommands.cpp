// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Models/OdysseyFlipbookEditorCommands.h"
#include "OdysseyStyleSet.h"
#include "Command/OdysseyCommandMacros.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorCommands"

namespace
{
    const FName PlayBackShortcuts = "PlayBack Shortcuts";
    const FName FrameShortcuts = "Frame Shortcuts";
    const FName KeyFrameShortcuts = "KeyFrame Shortcuts";
    const FName OtherShortcuts = "Other Shortcuts";
}

FOdysseyFlipbookEditorCommands::FOdysseyFlipbookEditorCommands()
    : TCommands<FOdysseyFlipbookEditorCommands>( "IliadFlipbookEditor", NSLOCTEXT( "Contexts", "IliadFlipbookEditor", "Iliad Flipbook Editor" ), NAME_None, FOdysseyStyle::GetStyleSetName() )
{
    AddBundle(PlayBackShortcuts, LOCTEXT("PlayBackShortcutsCategory", "PlayBack Shortcuts"));
    AddBundle(FrameShortcuts, LOCTEXT("FrameShortcutsCategory", "Frame Shortcuts"));
    AddBundle(KeyFrameShortcuts, LOCTEXT("KeyFrameShortcutsCategory", "KeyFrame Shortcuts"));
    AddBundle(OtherShortcuts, LOCTEXT("OtherShortcutsCategory", "Other Shortcuts"));
}

void
FOdysseyFlipbookEditorCommands::RegisterCommands()
{
// PlayBack Shortcuts Category
	UI_CMD( PlayForward, PlayBackShortcuts, "Play Forward / Pause", "Plays the Flipbook forward or pauses if already playing forward",                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( PlayBackward, PlayBackShortcuts, "Play Backward / Pause", "Plays the Flipbook backward or pauses if already playing backward",                            EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( Pause, PlayBackShortcuts, "Pause", "Pauses the Flipbook playback",                                                                                        EUserInterfaceActionType::Button, FInputChord( EKeys::SpaceBar ));
    UI_CMD( Stop, PlayBackShortcuts, "Stop", "Stops the Flipbook playback",                                                                                           EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ToggleLooping, PlayBackShortcuts, "Toggle Looping", "Toggles the playback looping mode",                                                                  EUserInterfaceActionType::ToggleButton, FInputChord( EModifierKey::Shift, EKeys::L ));

// Frame Shortcuts Category
    UI_CMD( AddNewKeyFrame, KeyFrameShortcuts, "Add Key Frame", "Adds a new key frame to the flipbook.",                                                              EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::N ));
    UI_CMD( AddNewKeyFrameBefore, KeyFrameShortcuts, "Insert Key Frame Before", "Adds a new key frame to the flipbook before the selection.",                         EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::I ));
    UI_CMD( AddNewKeyFrameAfter, KeyFrameShortcuts, "Insert Key Frame After", "Adds a new key frame to the flipbook after the selection.",                            EUserInterfaceActionType::Button, FInputChord( EKeys::I ));
    UI_CMD( PreviousKeyFrame, KeyFrameShortcuts, "Go To Previous Key Frame", "Moves the playback position to the previous key frame",                                 EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::Left ));
    UI_CMD( NextKeyFrame, KeyFrameShortcuts, "Go To Next Key Frame", "Moves the playback position to the next frame",                                                 EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::Right ));

// KeyFrame Shortcuts Category
    UI_CMD( PreviousFrame, FrameShortcuts, "Go To Previous Frame", "Moves the playback position to the previous frame",                                               EUserInterfaceActionType::Button, FInputChord( EKeys::Left ));
    UI_CMD( NextFrame, FrameShortcuts, "Go To Next Frame", "Moves the playback position to the next frame",                                                           EUserInterfaceActionType::Button, FInputChord( EKeys::Right ));
    UI_CMD( FirstFrame, FrameShortcuts, "Go To First Frame", "Moves the playback position to the first frame",                                                        EUserInterfaceActionType::Button, FInputChord( EModifierKey::FromBools( false, true, true, false ), EKeys::Left ));
    UI_CMD( LastFrame, FrameShortcuts, "Go To Last Frame", "Moves the playback position to the last frame",                                                           EUserInterfaceActionType::Button, FInputChord( EModifierKey::FromBools( false, true, true, false ), EKeys::Right));

// Other Shortcuts
    UI_CMD( ShowSpriteInContentBrowser, OtherShortcuts, "Show in Content Browser", "Shows the sprite for this key frame in the Content Browser.",                     EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::B ));
//    UI_CMD(PickNewSpriteForKeyFrame, OtherShortcuts, "Pick New Sprite", "Picks a new sprite for this key frame.",                                                    EUserInterfaceActionType::Button, FInputChord());
//    UI_CMD(EditSpriteForKeyFrame, OtherShortcuts, "Edit Sprite", "Opens the sprite for this key frame in the Sprite Editor.",                                        EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::E ));
}

#undef LOCTEXT_NAMESPACE
