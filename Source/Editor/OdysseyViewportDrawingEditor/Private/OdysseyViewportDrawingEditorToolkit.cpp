// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorToolkit.h"
#include "ModeToolbar/FOdysseyViewportDrawingEditorModeToolbar.h"
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
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenTopTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleTopTab", "Top Bar"),
        LOCTEXT("ViewportDrawingEditorToggleTopTabTooltip", "Display Top Bar"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.BrushExposedParameters20" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenBrushSelectorTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleBrushSelectorTab", "Brush Selector"),
        LOCTEXT("ViewportDrawingEditorToggleBrushSelectorTabTooltip", "Display Brush Selector"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.BrushSelector20" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenLayerStackTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleLayerStackTab", "Layer Stack"),
        LOCTEXT("ViewportDrawingEditorToggleLayerStackTabTooltip", "Display Layerstack Panel"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Layers20" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenColorWheelTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleColorWheelTab", "Color Wheel"),
        LOCTEXT("ViewportDrawingEditorToggleColorWheelTabTooltip", "Display Color Wheel"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.ColorWheel20" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenColorSlidersTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleColorSlidersTab", "Color Sliders"),
        LOCTEXT("ViewportDrawingEditorToggleColorSlidersTabTooltip", "Display Color Sliders"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.ColorSliders_2_20" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenToolsTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleToolsTab", "Tools"),
        LOCTEXT("ViewportDrawingEditorToggleToolsTabTooltip", "Display Tools"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Tools20" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenToolOptionsTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleToolOptionsTab", "Tool Options"),
        LOCTEXT("ViewportDrawingEditorToggleToolOptionsTabTooltip", "Display Tool Options"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.ToolOptions20" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenViewportTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleViewportTab", "2D Viewport"),
        LOCTEXT("ViewportDrawingEditorToggleViewportTabTooltip", "Display 2D Viewport"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Viewport20" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenTextureDetailsTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleTextureDetailsTab", "Texture Details"),
        LOCTEXT("ViewportDrawingEditorToggleTextureDetailsTabTooltip", "Display Texture Details"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Trombone20" )
    );

    ioToolbarBuilder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateSP( viewportDrawingEditor->GetToolbar(), &FOdysseyViewportDrawingEditorModeToolbar::OpenMeshSelectorTab) ),
        NAME_None,
        LOCTEXT("ViewportDrawingEditorToggleMeshSelectorTab", "Mesh Selector"),
        LOCTEXT("ViewportDrawingEditorToggleMeshSelectorTabTooltip", "Display Mesh Selector"),
        FSlateIcon( "OdysseyStyle", "PainterEditor.Mesh20" )
    );
}


#undef LOCTEXT_NAMESPACE // "OdysseyViewportDrawingEditorToolkit"