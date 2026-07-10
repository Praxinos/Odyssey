// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Styles/ArianeEditorStyle.h"

#include "Brushes/SlateBorderBrush.h"
#include "Brushes/SlateBoxBrush.h"
#include "Brushes/SlateImageBrush.h"
#include "Fonts/SlateFontInfo.h"
#include "Interfaces/IPluginManager.h"
#include "Layout/Margin.h"
#include "Misc/Paths.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/StyleColors.h"

namespace
{
static const FVector2D Icon8x8( 8.0f, 8.0f );
static const FVector2D Icon14x14( 14.0f, 14.0f );
static const FVector2D Icon16x16( 16.0f, 16.0f );
static const FVector2D Icon20x20( 20.0f, 20.0f );
static const FVector2D Icon24x24( 24.0f, 24.0f );
static const FVector2D Icon48x48( 48.0f, 48.0f );
static const FVector2D Icon64x64( 64.0f, 64.0f );
}

//---

//static
void
FArianeEditorStyle::Register()
{
    FSlateStyleRegistry::RegisterSlateStyle( Get() );
}

//static
void
FArianeEditorStyle::Unregister()
{
    FSlateStyleRegistry::UnRegisterSlateStyle( Get() );
}

//---

FArianeEditorStyle::FArianeEditorStyle()
    : FSlateStyleSet( "ArianeEditorStyle" )
{
    SetParentStyleName( FAppStyle::Get().GetStyleSetName() );

    Init();
}

void
FArianeEditorStyle::Init()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) / TEXT( "OdysseyAssetResources" ) );

    Set( "ArianeEditor.EdMode24", new IMAGE_BRUSH_SVG( "ArianeEditor/ariane_edmode", Icon24x24 ) );

    // Tooling
    Set( "ArianeEditor.ToolsTab.PathDrawing64"     , new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_drawing_tool", Icon64x64 ) );
    Set( "ArianeEditor.ToolsTab.PrimitiveDrawing64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/shapes_tool"      , Icon64x64 ) );
    Set( "ArianeEditor.ToolsTab.Eraser64"          , new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/eraser_tool"      , Icon64x64 ) );
    Set( "ArianeEditor.ToolsTab.PathEdit64"        , new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_edit_tool"   , Icon64x64 ) );
    Set( "ArianeEditor.ToolsTab.Transform64"       , new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/transform_tool"   , Icon64x64 ) );
    Set( "ArianeEditor.ToolsTab.Transform16"       , new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/transform_tool"   , Icon16x16 ) );

    // Tools shortcut
    Set( "ArianeEditor.ToolsShortcuts.PathEditAddRemoveVertex20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_edit_tool_add_remove_vertex", Icon20x20 ) );
    Set( "ArianeEditor.ToolsShortcuts.PathEditDeformSegment20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_edit_tool_deform_segment", Icon20x20 ) );
    Set( "ArianeEditor.ToolsShortcuts.PathEditMoveVertex20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_edit_tool_move_vertex", Icon20x20 ) );
    Set( "ArianeEditor.ToolsShortcuts.PathEditWidenVertex20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_edit_tool_widen_vertex", Icon20x20 ) );

    Set("ArianeEditorCheckBoxStyle.ToggleButton", FCheckBoxStyle( FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckbox") )
        .SetPadding(FMargin())
    );

    // Ariane Layer Stack
    Set( "ArianeEditor.Layers16", new IMAGE_BRUSH_SVG( "PainterEditor/layers", Icon16x16 ) );
    Set( "ArianeEditor.LayerStack.Visible16", new IMAGE_BRUSH_SVG( "OdysseyLayerStack/visible", Icon16x16 ) );
    Set( "ArianeEditor.LayerStack.Locked16", new IMAGE_BRUSH_SVG( "OdysseyLayerStack/locked", Icon16x16 ) );

    {
        FSlateColor selectedRow(FStyleColors::Select.GetSpecifiedColor().CopyWithNewOpacity(0.3f));
        FSlateColor selectedInactiveRow(FStyleColors::SelectInactive.GetSpecifiedColor().CopyWithNewOpacity(0.3f));
        FSlateColor selectedParentRow(FStyleColors::SelectParent.GetSpecifiedColor().CopyWithNewOpacity(0.3f));
        Set ("ArianeEditor.LayerStack.CurrentLayerBackgroundBrush", new FSlateColorBrush(FStyleColors::Select));
        Set ("ArianeEditor.LayerStack.CurrentLayerInactiveBackgroundBrush", new FSlateColorBrush(FStyleColors::SelectInactive));
        Set( "ArianeEditor.LayerStack.AlternatedRows", FTableRowStyle( FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row") )
                .SetOddRowBackgroundBrush(FSlateColorBrush(FStyleColors::Header))
                .SetSelectorFocusedBrush(FSlateNoResource())
                .SetActiveBrush(FSlateColorBrush(selectedRow))
                .SetActiveHoveredBrush(FSlateColorBrush(selectedRow))
                .SetInactiveBrush(FSlateColorBrush(selectedInactiveRow))
                .SetInactiveHoveredBrush(FSlateColorBrush(selectedInactiveRow))
                .SetActiveHighlightedBrush(FSlateColorBrush(selectedParentRow)) // This is the parent hightlight
                .SetInactiveHighlightedBrush(FSlateColorBrush(selectedParentRow))// This is the parent highlight
                //.SetDropIndicator_Above(const FSlateBrush& InValue)
                //.SetDropIndicator_Onto(const FSlateBrush& InValue)
                //.SetDropIndicator_Below(const FSlateBrush& InValue)
        );
    }

    Set("ArianeEditor.LayerStack.IsTransformedToggle", FCheckBoxStyle( FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckbox") )
        .SetPadding(FMargin(0)) //8 because left+right = 16 and top+bottom = 16
    );

    Set("ArianeEditor.LayerStack.IsVisibleToggle", FCheckBoxStyle()
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(*FAppStyle::Get().GetBrush("Level.NotVisibleIcon16x"))
        .SetUncheckedHoveredImage(*FAppStyle::Get().GetBrush("Level.NotVisibleIcon16x"))
        .SetUncheckedPressedImage(*FAppStyle::Get().GetBrush("Level.NotVisibleIcon16x"))
        //.SetCheckedImage(FSlateNoResource())
        .SetCheckedImage(*FAppStyle::Get().GetBrush("Level.VisibleIcon16x"))
        .SetCheckedHoveredImage(*FAppStyle::Get().GetBrush("Level.VisibleIcon16x"))
        .SetCheckedPressedImage(*FAppStyle::Get().GetBrush("Level.VisibleIcon16x"))
    );

    Set("ArianeEditor.LayerStack.IsLockedToggle", FCheckBoxStyle()
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(*FAppStyle::Get().GetBrush("Level.UnlockedIcon16x"))
        .SetUncheckedHoveredImage(*FAppStyle::Get().GetBrush("Level.UnlockedIcon16x"))
        .SetUncheckedPressedImage(*FAppStyle::Get().GetBrush("Level.UnlockedIcon16x"))
        .SetCheckedImage(*FAppStyle::Get().GetBrush("Level.LockedIcon16x"))
        .SetCheckedHoveredImage(*FAppStyle::Get().GetBrush("Level.LockedIcon16x"))
        .SetCheckedPressedImage(*FAppStyle::Get().GetBrush("Level.LockedIcon16x"))
    );

    //LayerStack
    {
        FTableRowStyle CoreTableRowStyle = FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");
        FSlateColor SelectedRow(FStyleColors::Select.GetSpecifiedColor().CopyWithNewOpacity(0.3f));
        FSlateColor SelectedInactiveRow(FStyleColors::SelectInactive.GetSpecifiedColor().CopyWithNewOpacity(0.3f));
        FSlateColor SelectedParentRow(FStyleColors::SelectParent.GetSpecifiedColor().CopyWithNewOpacity(0.3f));
        Set ("OdysseyLayerStack.CurrentLayerBackgroundBrush", new FSlateColorBrush(FStyleColors::Select));
        Set( "ArianeEditor.LayerStack.AlternatedRows", FTableRowStyle( CoreTableRowStyle )
            .SetOddRowBackgroundBrush(FSlateColorBrush(FStyleColors::Header))
            .SetSelectorFocusedBrush(FSlateNoResource())
            .SetActiveBrush(FSlateColorBrush(SelectedRow))
            .SetActiveHoveredBrush(FSlateColorBrush(SelectedRow))
            .SetInactiveBrush(FSlateColorBrush(SelectedInactiveRow))
            .SetInactiveHoveredBrush(FSlateColorBrush(SelectedInactiveRow))
            .SetActiveHighlightedBrush(FSlateColorBrush(SelectedParentRow)) // This is the parent hightlight
            .SetInactiveHighlightedBrush(FSlateColorBrush(SelectedParentRow))// This is the parent highlight
            //.SetDropIndicator_Above(const FSlateBrush& InValue)
            //.SetDropIndicator_Onto(const FSlateBrush& InValue)
            //.SetDropIndicator_Below(const FSlateBrush& InValue)
        );
    }

    // Ariane Editor Drawing Orientation
    Set( "ArianeEditor.DrawingOrientation.View20", new IMAGE_BRUSH_SVG( "ArianeEditor/drawing_orientation_view", Icon20x20 ) );
    Set( "ArianeEditor.DrawingOrientation.LayerXY20", new IMAGE_BRUSH_SVG( "ArianeEditor/drawing_orientation_xy", Icon20x20 ) );
    Set( "ArianeEditor.DrawingOrientation.LayerYZ20", new IMAGE_BRUSH_SVG( "ArianeEditor/drawing_orientation_yz", Icon20x20 ) );
    Set( "ArianeEditor.DrawingOrientation.LayerZX20", new IMAGE_BRUSH_SVG( "ArianeEditor/drawing_orientation_zx", Icon20x20 ) );
    Set( "ArianeEditor.DrawingOrientation.LayerDefined20", new IMAGE_BRUSH_SVG( "ArianeEditor/drawing_orientation_layer_defined", Icon20x20 ) );

    // Ariane Editor Scene Tree View
    Set( "ArianeEditor.SceneTreeView.Paintgroup", new IMAGE_BRUSH_SVG( "OdysseyVectorSceneTreeView/paintgroup", Icon16x16 ) );
    Set( "ArianeEditor.SceneTreeView.Path", new IMAGE_BRUSH_SVG( "OdysseyVectorSceneTreeView/path", Icon16x16 ) );
    Set( "ArianeEditor.SceneTreeView.Group", new IMAGE_BRUSH_SVG( "OdysseyVectorSceneTreeView/group", Icon16x16 ) );
    Set( "ArianeEditor.SceneTreeView.MenuIcon", new IMAGE_BRUSH_SVG( "OdysseyVectorSceneTreeView/MenuIcon", Icon16x16 ) );

    Set("ArianeEditor.SceneTreeView.IsVisibleToggle", FCheckBoxStyle()
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(*FAppStyle::Get().GetBrush("Level.NotVisibleIcon16x"))
        .SetUncheckedHoveredImage(*FAppStyle::Get().GetBrush("Level.NotVisibleIcon16x"))
        .SetUncheckedPressedImage(*FAppStyle::Get().GetBrush("Level.NotVisibleIcon16x"))
        .SetCheckedImage(FSlateNoResource())
        .SetCheckedHoveredImage(*FAppStyle::Get().GetBrush("Level.VisibleIcon16x"))
        .SetCheckedPressedImage(*FAppStyle::Get().GetBrush("Level.VisibleIcon16x"))
    );
}

//static
const FArianeEditorStyle&
FArianeEditorStyle::Get()
{
    static FArianeEditorStyle* Style;

    if( Style == nullptr )
    {
        Style = new FArianeEditorStyle();
    }

    return *Style;
}
