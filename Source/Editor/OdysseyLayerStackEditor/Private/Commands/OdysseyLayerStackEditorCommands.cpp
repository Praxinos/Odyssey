// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Commands/OdysseyLayerStackEditorCommands.h"
#include "OdysseyStyleSet.h"
#include "Command/OdysseyCommandMacros.h"

#define LOCTEXT_NAMESPACE "LayerStackEditor"

namespace
{
    const FName LayerStackShortcuts = "LayerStack Shortcuts";
}

FOdysseyLayerStackEditorCommands::FOdysseyLayerStackEditorCommands()
    : TCommands<FOdysseyLayerStackEditorCommands>( "LayerStackEditor", LOCTEXT( "editor-commands.name", "LayerStack" ), NAME_None, FOdysseyStyle::GetStyleSetName() )
{
    AddBundle(LayerStackShortcuts, LOCTEXT("editor-commands.category.layerstack-shortcuts", "LayerStack Shortcuts"));
}

void
FOdysseyLayerStackEditorCommands::RegisterCommands()
{
// LayerStack Shortcuts Category
    UI_BUNDLE_COMMAND( MergeSelectedLayers, LayerStackShortcuts, "Merge Selected Layers", "Merge Selected Layers", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( NavigateToNextLayer, LayerStackShortcuts, "Navigate To The Layer Below", "Navigate To The Layer Below The Current Layer", EUserInterfaceActionType::Button, FInputChord(EKeys::L) );
    UI_BUNDLE_COMMAND( NavigateToPreviousLayer, LayerStackShortcuts, "Navigate To The Layer Above", "Navigate To The Layer Above The Current Layer", EUserInterfaceActionType::Button, FInputChord(EKeys::O) );
    UI_BUNDLE_COMMAND( OpenFolderLayer, LayerStackShortcuts, "Open Folder Layer", "Open Folder Layer", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( CloseFolderLayer, LayerStackShortcuts, "Close Folder Layer", "Close Folder Layer", EUserInterfaceActionType::Button, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
