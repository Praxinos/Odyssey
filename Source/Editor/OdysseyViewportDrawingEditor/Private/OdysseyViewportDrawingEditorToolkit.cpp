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
        LOCTEXT("ViewportDrawingEditorToggleLayerStackTab", "Layer Stack"),
        LOCTEXT("ViewportDrawingEditorToggleLayerStackTabTooltip", "Display Layerstack Panel"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Layers64" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenColorSlidersTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleColorSlidersTab", "Color Sliders"),
        LOCTEXT("ViewportDrawingEditorToggleColorSlidersTabTooltip", "Display Color Sliders"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Layers64" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenBrushExposedParametersTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleBrushExposedParametersTab", "Exposed Param"),
        LOCTEXT("ViewportDrawingEditorToggleBrushExposedParametersTabTooltip", "Display Brush Exposed Parameters"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Layers64" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenBrushSelectorTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleBrushSelectorTab", "Brush Selector"),
        LOCTEXT("ViewportDrawingEditorToggleBrushSelectorTabTooltip", "Display Brush Selector"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Layers64" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenColorWheelTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleColorWheelTab", "Color Wheel"),
        LOCTEXT("ViewportDrawingEditorToggleColorWheelTabTooltip", "Display Color Wheel"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Layers64" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenMeshSelectorTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleMeshSelectorTab", "Mesh Selector"),
        LOCTEXT("ViewportDrawingEditorToggleMeshSelectorTabTooltip", "Display Mesh Selector"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Layers64" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenStrokeOptionsTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleStrokeOptionsTab", "Stroke Options"),
        LOCTEXT("ViewportDrawingEditorToggleStrokeOptionsTabTooltip", "Display Stroke Options"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Layers64" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenTextureDetailsTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleTextureDetailsTab", "Texture Details"),
        LOCTEXT("ViewportDrawingEditorToggleTextureDetailsTabTooltip", "Display Texture Details"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Layers64" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenToolsTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleToolsTab", "Tools"),
        LOCTEXT("ViewportDrawingEditorToggleToolsTabTooltip", "Display Tools"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Layers64" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenTopTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleTopTab", "Top Tab"),
        LOCTEXT("ViewportDrawingEditorToggleTopTabTooltip", "Display Top Tab"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Layers64" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenViewportTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleViewportTab", "2D Viewport"),
        LOCTEXT("ViewportDrawingEditorToggleViewportTabTooltip", "Display 2D Viewport"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Layers64" )
    );
}


#undef LOCTEXT_NAMESPACE // "OdysseyViewportDrawingEditorToolkit"