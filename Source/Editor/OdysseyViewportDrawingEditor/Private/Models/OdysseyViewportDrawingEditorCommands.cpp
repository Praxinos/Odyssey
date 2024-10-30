// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyViewportDrawingEditorCommands.h"

#define LOCTEXT_NAMESPACE "ViewportDrawingEditor"

FOdysseyViewportDrawingEditorCommands::FOdysseyViewportDrawingEditorCommands()
    : TCommands<FOdysseyViewportDrawingEditorCommands>( "OdysseyViewportDrawingEditMode", LOCTEXT("editor-commands.name", "Odyssey Viewport Drawing Edit Mode"), NAME_None, FOdysseyStyle::GetStyleSetName())
{}

void FOdysseyViewportDrawingEditorCommands::RegisterCommands()
{
}

#undef LOCTEXT_NAMESPACE

