// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationLayerStackEditorCommands.h"

#include "Command/OdysseyCommandMacros.h"
#include "OdysseyStyle.h"

#define LOCTEXT_NAMESPACE "AnimationLayerStackEditor"

namespace
{
const FName LayerStackShortcuts = "Animation LayerStack Shortcuts";
}

FOdysseyAnimationLayerStackEditorCommands::FOdysseyAnimationLayerStackEditorCommands()
    : TCommands<FOdysseyAnimationLayerStackEditorCommands>("AnimationLayerStackEditorCommands",
        LOCTEXT("editor-commands.name", "Animation LayerStack"), NAME_None, FOdysseyStyle::GetStyleSetName())
{
    AddBundle(LayerStackShortcuts, LOCTEXT("editor-commands.category", "Animation LayerStack Shortcuts"));
}

void
FOdysseyAnimationLayerStackEditorCommands::RegisterCommands()
{
    UI_BUNDLE_COMMAND( ActivateLayerLighttable, LayerStackShortcuts, "Activate Layer's Lighttable", "Activates selected Layers' Lighttable", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( InactivateLayerLighttable, LayerStackShortcuts, "Inactivate Layer's Lighttable", "Inactivates selected Layers' Lighttable", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ToggleLayerLighttable, LayerStackShortcuts, "Toggle Layer's Lighttable", "Activates/Inactivates selected Layers' Lighttable", EUserInterfaceActionType::Button, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
