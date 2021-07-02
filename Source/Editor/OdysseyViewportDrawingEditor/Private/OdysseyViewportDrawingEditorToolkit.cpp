// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "ModeToolbar/FOdysseyViewportDrawingEditorModeToolbar.h"
#include "OdysseyViewportDrawingEditorToolkit.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorToolkit"

FOdysseyViewportDrawingEditorToolkit::FOdysseyViewportDrawingEditorToolkit(TSharedPtr<FOdysseyViewportDrawingEditor> iEditor, class FEdMode* iEditorMode)
	: FOdysseyModeToolkit( FName("OdysseyViewportDrawingApp"), iEditor, iEditorMode )
{
}

FName
FOdysseyViewportDrawingEditorToolkit::GetToolkitFName() const
{
	return FName("OdysseyMeshPaintMode");
}

FText
FOdysseyViewportDrawingEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("ToolkitName", "Odyssey Mesh Paint");
}


void
FOdysseyViewportDrawingEditorToolkit::GetToolPaletteNames( TArray<FName>& ioPaletteNames ) const
{
    ioPaletteNames.Add( FName( "Iliad Panels Manager" ));
}

void
FOdysseyViewportDrawingEditorToolkit::BuildToolPalette( FName iPalette, class FToolBarBuilder& ioToolbarBuilder )
{
    FOdysseyViewportDrawingEditor* viewportDrawingEditor = static_cast<FOdysseyViewportDrawingEditor*>( mEditor.Get() );
    if( !viewportDrawingEditor ) return;

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenLayerStackTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleLayerStackTab", "Layerstack"),
        LOCTEXT("ViewportDrawingEditorToggleLayerStackTabTooltip", "Display Layerstack Panel"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Layers64" )
    );
}


#undef LOCTEXT_NAMESPACE // "OdysseyViewportDrawingEditorToolkit"