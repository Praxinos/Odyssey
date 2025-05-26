// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyViewportDrawingEditorCommands.h"

#define LOCTEXT_NAMESPACE "ViewportDrawingEditor"

FOdysseyViewportDrawingEditorCommands::FOdysseyViewportDrawingEditorCommands()
    : TCommands<FOdysseyViewportDrawingEditorCommands>( "OdysseyViewportDrawingEditMode", LOCTEXT("editor-commands.name", "Odyssey Viewport Drawing Edit Mode"), NAME_None, FOdysseyStyle::GetStyleSetName())
{}

void FOdysseyViewportDrawingEditorCommands::RegisterCommands()
{
}

#undef LOCTEXT_NAMESPACE
