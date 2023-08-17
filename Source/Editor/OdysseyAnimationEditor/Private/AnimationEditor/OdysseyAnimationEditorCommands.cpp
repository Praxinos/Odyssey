// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "Command/OdysseyCommandMacros.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorCommands"

namespace
{
    const FName ImportExportShortcuts = "Import & Export Shortcuts";
    const FName LayerStackShortcuts = "LayerStack Shortcuts";
}

FOdysseyAnimationEditorCommands::FOdysseyAnimationEditorCommands()
    : TCommands<FOdysseyAnimationEditorCommands>( "IliadAnimationEditor", NSLOCTEXT( "Contexts", "IliadAnimationEditor", "Iliad Texture Editor" ), NAME_None, FAppStyle::GetAppStyleSetName() )
{
    AddBundle(ImportExportShortcuts, LOCTEXT("ImportExportShortcutsCategory", "Import & Export Shortcuts"));
    AddBundle(LayerStackShortcuts, LOCTEXT("LayerStackShortcutsCategory", "LayerStack Shortcuts"));
}

void
FOdysseyAnimationEditorCommands::RegisterCommands()
{
// Import & Export Shortcuts Category
    UI_CMD( ImportTextureSequence, ImportExportShortcuts, "Import Texture Sequence", "Import several textures from the content-browser in a new layer",                                               EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::P));

// LayerStack Shortcuts Category
    UI_CMD( CreateNewAnimationLayerImageRaster, LayerStackShortcuts, "Create New Aniamtion Layer Image Raster", "Create New Aniamtion Layer Image Raster",                                                                                                      EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::N));
    UI_CMD( ChangeLayerOpacity10, LayerStackShortcuts, "Change current Layer Opacity to 10%", "Change current Layer Opacity to 10%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity20, LayerStackShortcuts, "Change current Layer Opacity to 20%", "Change current Layer Opacity to 20%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity30, LayerStackShortcuts, "Change current Layer Opacity to 30%", "Change current Layer Opacity to 30%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity40, LayerStackShortcuts, "Change current Layer Opacity to 40%", "Change current Layer Opacity to 40%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity50, LayerStackShortcuts, "Change current Layer Opacity to 50%", "Change current Layer Opacity to 50%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity60, LayerStackShortcuts, "Change current Layer Opacity to 60%", "Change current Layer Opacity to 60%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity70, LayerStackShortcuts, "Change current Layer Opacity to 70%", "Change current Layer Opacity to 70%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity80, LayerStackShortcuts, "Change current Layer Opacity to 80%", "Change current Layer Opacity to 80%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity90, LayerStackShortcuts, "Change current Layer Opacity to 90%", "Change current Layer Opacity to 90%",                                                                EUserInterfaceActionType::Button, FInputChord());
    UI_CMD( ChangeLayerOpacity100, LayerStackShortcuts, "Change current Layer Opacity to 100%", "Change current Layer Opacity to 100%",                                                             EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
