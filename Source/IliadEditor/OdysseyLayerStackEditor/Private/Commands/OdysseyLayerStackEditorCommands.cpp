// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Commands/OdysseyLayerStackEditorCommands.h"
#include "OdysseyStyle.h"
#include "Command/OdysseyCommandMacros.h"
#include "OdysseyBlendingMode.h"

#define LOCTEXT_NAMESPACE "LayerStackEditor"

namespace
{
    const FName LayerStackShortcuts = "LayerStack Shortcuts";
    const FName LayerBlendingModeShortcuts = "Layer Blending Mode Shortcuts";
}

FOdysseyLayerStackEditorCommands::FOdysseyLayerStackEditorCommands()
    : TCommands<FOdysseyLayerStackEditorCommands>( "LayerStackEditor", LOCTEXT( "editor-commands.name", "LayerStack" ), NAME_None, FOdysseyStyle::GetStyleSetName() )
{
    AddBundle(LayerStackShortcuts, LOCTEXT("editor-commands.category.layerstack-shortcuts", "LayerStack Shortcuts"));
    AddBundle(LayerBlendingModeShortcuts, LOCTEXT("editor-commands.category.layer-blending-mode-shortcuts-category", "Layer Blending Mode Shortcuts"));
}

void
FOdysseyLayerStackEditorCommands::RegisterCommands()
{
// LayerStack Shortcuts Category
    UI_BUNDLE_COMMAND( MergeSelectedLayers, LayerStackShortcuts, "Merge Selected Layers", "Merge Selected Layers", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( NavigateToNextLayer, LayerStackShortcuts, "Navigate To The Layer Below", "Navigate To The Layer Below The Current Layer", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( NavigateToPreviousLayer, LayerStackShortcuts, "Navigate To The Layer Above", "Navigate To The Layer Above The Current Layer", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( OpenFolderLayer, LayerStackShortcuts, "Open Folder Layer", "Open Folder Layer", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( CloseFolderLayer, LayerStackShortcuts, "Close Folder Layer", "Close Folder Layer", EUserInterfaceActionType::Button, FInputChord() );

    // Layer Blend Modes shorcuts Category
    UI_BUNDLE_COMMAND( SetCurrentLayerBlendModeToNextBlendMode, LayerBlendingModeShortcuts, "Set Current Layer Blend Mode To Next Blend Mode", "Sets the current layer blend mode to the next blend mode", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetCurrentLayerBlendModeToPreviousBlendMode, LayerBlendingModeShortcuts, "Set Current Layer Blend Mode To Previous Blend Mode", "Sets the current layer blend mode to the previous blend mode", EUserInterfaceActionType::Button, FInputChord() );
    for (int i = 0; EOdysseyBlendingMode blendMode : TEnumRange<EOdysseyBlendingMode>())
    {
        FText blendModeText = UEnum::GetDisplayValueAsText(blendMode);

        TSharedPtr<FUICommandInfo> commandInfo = FUICommandInfoDecl(
              this->AsShared()
            , FName( *FString::Printf( TEXT( "SetCurrentLayerBlendModeTo%s" ), *blendModeText.ToString() ))
            , FText::Format( LOCTEXT( "commands.set-current-layer-blend-mode.label", "{0}"), blendModeText )
            , FText::Format( LOCTEXT( "commands.set-current-layer-blend-mode.tooltip", "Set Current Layer Blend Mode To {0}"), blendModeText )
            , LayerBlendingModeShortcuts
        )
        .UserInterfaceType( EUserInterfaceActionType::Button )
        .DefaultChord( FInputChord() );

        SetCurrentLayerBlendMode.Add(commandInfo);
    }
}

#undef LOCTEXT_NAMESPACE
