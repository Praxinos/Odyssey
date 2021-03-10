// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorToolkit.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorToolkit"

FOdysseyViewportDrawingEditorToolkit::FOdysseyViewportDrawingEditorToolkit(TSharedPtr<FOdysseyViewportDrawingEditor> iEditor, class FEdMode* iEditorMode)
	: FOdysseyModeToolkit( FName("OdysseyViewportDrawingApp"), iEditor, iEditorMode )
{
}

FName FOdysseyViewportDrawingEditorToolkit::GetToolkitFName() const
{
	return FName("OdysseyMeshPaintMode");
}

FText FOdysseyViewportDrawingEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("ToolkitName", "Odyssey Mesh Paint");
}

#undef LOCTEXT_NAMESPACE // "OdysseyViewportDrawingEditorToolkit"