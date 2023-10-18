// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "Command/OdysseyCommandMacros.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorCommands"

namespace
{
    const FName ImportExportShortcuts = "Import & Export Shortcuts";
    const FName TimelineShortcuts = "Timeline Shortcuts";
}

FOdysseyAnimationEditorCommands::FOdysseyAnimationEditorCommands()
    : TCommands<FOdysseyAnimationEditorCommands>( "IliadAnimationEditor", NSLOCTEXT( "Contexts", "IliadAnimationEditor", "Iliad Texture Editor" ), NAME_None, FAppStyle::GetAppStyleSetName() )
{
    AddBundle(ImportExportShortcuts, LOCTEXT("ImportExportShortcutsCategory", "Import & Export Shortcuts"));
    AddBundle(TimelineShortcuts, LOCTEXT("TimelineShortcutsCategory", "Timeline Shortcuts"));
}

void
FOdysseyAnimationEditorCommands::RegisterCommands()
{
// Import & Export Shortcuts Category
    UI_CMD( ImportTextureSequence, ImportExportShortcuts, "Import Texture Sequence", "Import several textures from the content-browser in a new layer",                                               EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::P));

// LayerStack Shortcuts Category
    UI_CMD( CreateNewAnimationLayerImageRaster, TimelineShortcuts, "Create New Aniamtion Layer Image Raster", "Create New Aniamtion Layer Image Raster",                                                                                                      EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::N));

    UI_CMD( ChangeLayerOpacity10, TimelineShortcuts, "Change current Layer Opacity to 10%", "Change current Layer Opacity to 10%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity20, TimelineShortcuts, "Change current Layer Opacity to 20%", "Change current Layer Opacity to 20%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity30, TimelineShortcuts, "Change current Layer Opacity to 30%", "Change current Layer Opacity to 30%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity40, TimelineShortcuts, "Change current Layer Opacity to 40%", "Change current Layer Opacity to 40%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity50, TimelineShortcuts, "Change current Layer Opacity to 50%", "Change current Layer Opacity to 50%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity60, TimelineShortcuts, "Change current Layer Opacity to 60%", "Change current Layer Opacity to 60%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity70, TimelineShortcuts, "Change current Layer Opacity to 70%", "Change current Layer Opacity to 70%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity80, TimelineShortcuts, "Change current Layer Opacity to 80%", "Change current Layer Opacity to 80%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity90, TimelineShortcuts, "Change current Layer Opacity to 90%", "Change current Layer Opacity to 90%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity100, TimelineShortcuts, "Change current Layer Opacity to 100%", "Change current Layer Opacity to 100%",                                                             EUserInterfaceActionType::Button, FInputChord());

    UI_CMD( StepForward, TimelineShortcuts, "Step Forward", "Change the current frame to be the next frame in the animation timeline",                                                            EUserInterfaceActionType::Button, FInputChord(EKeys::O));
    UI_CMD( StepBackward, TimelineShortcuts, "Step Backward", "Change the current frame to be the previous frame in the animation timeline",                                                      EUserInterfaceActionType::Button, FInputChord(EKeys::U));

    UI_CMD( SetPreBehaviourNone, TimelineShortcuts, "Set Layer's Pre Behaviour to None", "Set Layer's Pre Behaviour to None", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_CMD( SetPreBehaviourHold, TimelineShortcuts, "Set Layer's Pre Behaviour to Hold", "Set Layer's Pre Behaviour to Hold", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_CMD( SetPreBehaviourLoop, TimelineShortcuts, "Set Layer's Pre Behaviour to Loop", "Set Layer's Pre Behaviour to Loop", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_CMD( SetPreBehaviourPingPong, TimelineShortcuts, "Set Layer's Pre Behaviour to PingPong", "Set Layer's Pre Behaviour to PingPong", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_CMD( SetPostBehaviourNone, TimelineShortcuts, "Set Layer's Post Behaviour to None", "Set Layer's Post Behaviour to None", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_CMD( SetPostBehaviourHold, TimelineShortcuts, "Set Layer's Post Behaviour to Hold", "Set Layer's Post Behaviour to Hold", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_CMD( SetPostBehaviourLoop, TimelineShortcuts, "Set Layer's Post Behaviour to Loop", "Set Layer's Post Behaviour to Loop", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_CMD( SetPostBehaviourPingPong, TimelineShortcuts, "Set Layer's Post Behaviour to PingPong", "Set Layer's Post Behaviour to PingPong", EUserInterfaceActionType::RadioButton, FInputChord());
}

#undef LOCTEXT_NAMESPACE
