// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeEditorCommands.h"
// Odyssey
#include "Command/OdysseyCommandMacros.h"
#include "OdysseyStyle.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

namespace
{
    const FName ToolbarShortcuts = "Toolbar Shortcuts";
/*
    const FName BrushAlphaModeShortcuts = "Brush Alpha Mode Shortcuts";
    const FName ViewportShortcuts = "Viewport Shortcuts";
    const FName BrushStrokesShortcuts = "Brushes & Strokes Shortcuts";
    const FName HelpShortcuts = "Help Shortcuts";
    const FName UncategorizedYetShortcuts = "Uncategorized Yet Shortcuts";
    const FName ToolsShortcuts = "Tools Shortcuts";
    const FName LayerStackShortcuts = "LayerStack Shortcuts";
    const FName ImportExportShortcuts = "Import & Export Shortcuts";
*/
}

FArianeEditorCommands::FArianeEditorCommands()
    : TCommands<FArianeEditorCommands>( "ArianeEditor"
                                      , LOCTEXT( "editor-commands.name"
                                               , "Ariane Editor" )
                                      , NAME_None
                                      , FOdysseyStyle::GetStyleSetName() )
{
    AddBundle(ToolbarShortcuts, LOCTEXT("editor-commands.category.toolbar-shortcuts", "Toolbar Shortcuts"));
/*
    AddBundle(BrushAlphaModeShortcuts, LOCTEXT("editor-commands.category.brush-alpha-mode-shortcuts", "Brush Alpha Mode Shortcuts"));
    AddBundle(ViewportShortcuts, LOCTEXT("editor-commands.category.viewport-shortcuts", "Viewport Shortcuts"));
    AddBundle(BrushStrokesShortcuts, LOCTEXT("editor-commands.category.brushes-ans-strokes-shortcuts", "Brushes & Strokes Shortcuts"));
    AddBundle(HelpShortcuts, LOCTEXT("editor-commands.category.help-shortcuts", "Help Shortcuts"));
    AddBundle(ToolsShortcuts, LOCTEXT("editor-commands.category.tools-shortcuts", "Tools Shortcuts"));
    AddBundle(UncategorizedYetShortcuts, LOCTEXT("editor-commands.category.uncategorized-yet-shortcuts", "Uncategorized Yet Shortcuts"));
    AddBundle(LayerStackShortcuts, LOCTEXT("editor-commands.category.layerstack-shortcuts-category", "LayerStack Shortcuts"));
    AddBundle(ImportExportShortcuts, LOCTEXT("editor-commands.category.import-export-shortcuts", "Import & Export Shortcuts"));
*/
}

void
FArianeEditorCommands::RegisterCommands()
{
    UI_BUNDLE_COMMAND( LaunchAriane
                     , ToolbarShortcuts
                     , "Ariane"
                     , "Ariane"
                     , EUserInterfaceActionType::Button
                     , FInputChord() );
}

#undef LOCTEXT_NAMESPACE
