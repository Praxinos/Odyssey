// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "Command/OdysseyCommandMacros.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

namespace
{
    const FName ImportExportShortcuts = "Import & Export Shortcuts";
    const FName TimelineShortcuts = "Timeline Shortcuts";
    const FName LayersShortcuts = "Layers Shortcuts";
    const FName CellsShortcuts = "Cells Shortcuts";
}

FOdysseyAnimationEditorCommands::FOdysseyAnimationEditorCommands()
    : TCommands<FOdysseyAnimationEditorCommands>( "OdysseyAnimationEditorCommands", LOCTEXT( "editor-commands.name", "Iliad Texture Editor" ), NAME_None, FOdysseyStyle::GetStyleSetName() )
{
    AddBundle(ImportExportShortcuts, LOCTEXT("editor-commands.category.import-export-shortcuts", "Import & Export Shortcuts"));
    AddBundle(TimelineShortcuts, LOCTEXT("editor-commands.category.timeline-shortcuts", "Timeline Shortcuts"));
    AddBundle(LayersShortcuts, LOCTEXT("editor-commands.category.layers-shortcuts", "Layers Shortcuts"));
    AddBundle(CellsShortcuts, LOCTEXT("editor-commands.category.cells-shortcuts", "Cells Shortcuts"));
}

void
FOdysseyAnimationEditorCommands::RegisterCommands()
{
// Import & Export Shortcuts Category
    UI_BUNDLE_COMMAND( ImportTextureSequence, ImportExportShortcuts, "Import Texture Sequence", "Import several textures from the content-browser in a new layer",                                               EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::P));

// LayerStack Shortcuts Category

    UI_BUNDLE_COMMAND( StepForward, TimelineShortcuts, "Step Forward", "Change the current frame to be the next frame in the animation timeline",                                                            EUserInterfaceActionType::Button, FInputChord(EKeys::O));
    UI_BUNDLE_COMMAND( StepBackward, TimelineShortcuts, "Step Backward", "Change the current frame to be the previous frame in the animation timeline",                                                      EUserInterfaceActionType::Button, FInputChord(EKeys::U));

    UI_BUNDLE_COMMAND( CreateNewAnimationLayerImageRaster, LayersShortcuts, "Create New Aniamtion Layer Image Raster", "Create New Aniamtion Layer Image Raster", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::N));

    UI_BUNDLE_COMMAND( ChangeLayerOpacity10, LayersShortcuts, "Change current Layer Opacity to 10%", "Change current Layer Opacity to 10%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity20, LayersShortcuts, "Change current Layer Opacity to 20%", "Change current Layer Opacity to 20%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity30, LayersShortcuts, "Change current Layer Opacity to 30%", "Change current Layer Opacity to 30%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity40, LayersShortcuts, "Change current Layer Opacity to 40%", "Change current Layer Opacity to 40%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity50, LayersShortcuts, "Change current Layer Opacity to 50%", "Change current Layer Opacity to 50%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity60, LayersShortcuts, "Change current Layer Opacity to 60%", "Change current Layer Opacity to 60%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity70, LayersShortcuts, "Change current Layer Opacity to 70%", "Change current Layer Opacity to 70%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity80, LayersShortcuts, "Change current Layer Opacity to 80%", "Change current Layer Opacity to 80%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity90, LayersShortcuts, "Change current Layer Opacity to 90%", "Change current Layer Opacity to 90%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ChangeLayerOpacity100, LayersShortcuts, "Change current Layer Opacity to 100%", "Change current Layer Opacity to 100%",                                                             EUserInterfaceActionType::Button, FInputChord());

    UI_BUNDLE_COMMAND( SetPreBehaviourNone, LayersShortcuts, "Set Layer's Pre Behaviour to None", "Set Layer's Pre Behaviour to None", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_BUNDLE_COMMAND( SetPreBehaviourHold, LayersShortcuts, "Set Layer's Pre Behaviour to Hold", "Set Layer's Pre Behaviour to Hold", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_BUNDLE_COMMAND( SetPreBehaviourLoop, LayersShortcuts, "Set Layer's Pre Behaviour to Loop", "Set Layer's Pre Behaviour to Loop", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_BUNDLE_COMMAND( SetPreBehaviourPingPong, LayersShortcuts, "Set Layer's Pre Behaviour to PingPong", "Set Layer's Pre Behaviour to PingPong", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_BUNDLE_COMMAND( SetPostBehaviourNone, LayersShortcuts, "Set Layer's Post Behaviour to None", "Set Layer's Post Behaviour to None", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_BUNDLE_COMMAND( SetPostBehaviourHold, LayersShortcuts, "Set Layer's Post Behaviour to Hold", "Set Layer's Post Behaviour to Hold", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_BUNDLE_COMMAND( SetPostBehaviourLoop, LayersShortcuts, "Set Layer's Post Behaviour to Loop", "Set Layer's Post Behaviour to Loop", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_BUNDLE_COMMAND( SetPostBehaviourPingPong, LayersShortcuts, "Set Layer's Post Behaviour to PingPong", "Set Layer's Post Behaviour to PingPong", EUserInterfaceActionType::RadioButton, FInputChord());

    UI_BUNDLE_COMMAND( StaggerCell, CellsShortcuts, "Stagger Cell", "Creates a Stagger Cell", EUserInterfaceActionType::Button, FInputChord());

    UI_BUNDLE_COMMAND( SetStaggerCellBehaviourHold, CellsShortcuts, "Set Stagger Cell Behaviour to Hold", "Set Stagger Cell Behaviour to Hold", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( SetStaggerCellBehaviourLoop, CellsShortcuts, "Set Stagger Cell Behaviour to Loop", "Set Stagger Cell Behaviour to Loop", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( SetStaggerCellBehaviourPingPong, CellsShortcuts, "Set Stagger Cell Behaviour to PingPong", "Set Stagger Cell Behaviour to PingPong", EUserInterfaceActionType::Button, FInputChord());

    UI_BUNDLE_COMMAND( ActivateTimelineSelectionTool, TimelineShortcuts, "Activate Timeline Selection Tool", "Activates the timeline Selection Tool", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ActivateTimelineMoveTool, TimelineShortcuts, "Activate Timeline Move Tool", "Activates the timeline Move Tool", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ActivateTimelineCutTool, TimelineShortcuts, "Activate Timeline Cut Tool", "Activates the timeline Cut Tool", EUserInterfaceActionType::Button, FInputChord());

    UI_BUNDLE_COMMAND( HoldActivateTimelineSelectionTool, TimelineShortcuts, "Activate Timeline Selection Tool (Hold)", "Hold the key to activate the timeline Selection Tool temporarily", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( HoldActivateTimelineMoveTool, TimelineShortcuts, "Activate Timeline Move Tool (Hold)", "Hold the key to activate the timeline Move Tool temporarily", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( HoldActivateTimelineCutTool, TimelineShortcuts, "Activate Timeline Cut Tool (Hold)", "Hold the key to activate the timeline Cut Tool temporarily", EUserInterfaceActionType::Button, FInputChord());

    UI_BUNDLE_COMMAND( BreakCell, TimelineShortcuts, "Break Cell", "Breaks Cell at Current Frame in Current Layer", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
