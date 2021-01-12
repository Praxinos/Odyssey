// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorEdModeToolkit.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "IMeshPainter.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorEdModeToolkit"

FOdysseyViewportDrawingEditorEdModeToolkit::FOdysseyViewportDrawingEditorEdModeToolkit(class FOdysseyViewportDrawingEditorEdMode* iOwningMode)
	: mMeshPaintEdMode(iOwningMode)
{
}

void FOdysseyViewportDrawingEditorEdModeToolkit::Init(const TSharedPtr< class IToolkitHost >& iInitToolkitHost)
{
	FModeToolkit::Init(iInitToolkitHost);
}

FName FOdysseyViewportDrawingEditorEdModeToolkit::GetToolkitFName() const
{
	return FName("OdysseyMeshPaintMode");
}

FText FOdysseyViewportDrawingEditorEdModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("ToolkitName", "Odyssey Mesh Paint");
}

class FEdMode* FOdysseyViewportDrawingEditorEdModeToolkit::GetEditorMode() const
{
	return mMeshPaintEdMode;
}

TSharedPtr<SWidget> FOdysseyViewportDrawingEditorEdModeToolkit::GetInlineContent() const
{
	return mMeshPaintEdMode->GetMeshPainter()->GetWidget();
}

#undef LOCTEXT_NAMESPACE // "OdysseyViewportDrawingEditorEdModeToolkit"