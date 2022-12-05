// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Commands/OdysseyLayerStackEditorCommands.h"
#include "OdysseyStyleSet.h"
#include "Command/OdysseyCommandMacros.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStackEditorCommands"

namespace
{
    const FName LayerStackShortcuts = "LayerStack Shortcuts";
}

FOdysseyLayerStackEditorCommands::FOdysseyLayerStackEditorCommands()
    : TCommands<FOdysseyLayerStackEditorCommands>( "LayerStackEditor", NSLOCTEXT( "Contexts", "LayerStackEditor", "LayerStack" ), NAME_None, FOdysseyStyle::GetStyleSetName() )
{
    AddBundle(LayerStackShortcuts, LOCTEXT("LayerStackShortcutsCategory", "LayerStack Shortcuts"));
}

void
FOdysseyLayerStackEditorCommands::RegisterCommands()
{
// LayerStack Shortcuts Category
    UI_CMD( MergeSelectedLayers, LayerStackShortcuts, "Merge Selected Layers", "Merge Selected Layers", EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( FlattenSelectedLayers, LayerStackShortcuts, "Flatten Selected Layers", "Flatten Selected Layers", EUserInterfaceActionType::Button, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
