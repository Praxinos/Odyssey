// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SlateOdysseyStyle.h"

#include "Misc/CommandLine.h"
#include "Styling/CoreStyle.h"
#include "Styling/SegmentedControlStyle.h"
#include "Styling/SlateStyle.h"
#include "Styling/StyleColors.h"
#include "Styling/SlateTypes.h"
#include "Interfaces/IPluginManager.h"

#if (WITH_EDITOR || (IS_PROGRAM && PLATFORM_DESKTOP))
    #include "PlatformInfo.h"
#endif

#define IMAGE_BRUSH( RelativePath, ... )    FSlateImageBrush( RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define IMAGE_BRUSH_SVG( RelativePath, ... ) FSlateVectorImageBrush( RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)
#define BOX_BRUSH( RelativePath, ... )      FSlateBoxBrush( RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH_SVG( RelativePath, ... )  FSlateVectorBoxBrush( RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)
#define BORDER_BRUSH( RelativePath, ... )   FSlateBorderBrush( RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH_SVG( RelativePath, ... )  FSlateVectorBorderBrush( RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)
#define DEFAULT_FONT( ... )                 FCoreStyle::GetDefaultFontStyle( __VA_ARGS__ )
#define ICON_FONT( ... )                    FSlateFontInfo( RootToContentDir( "Fonts/FontAwesome", TEXT(".ttf") ), __VA_ARGS__ )

#define EXTERN_IMAGE_BRUSH( Style, RelativePath, ... )    FSlateImageBrush( Style.RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )



/* FOdysseyStyleDefault interface
 *****************************************************************************/

static
bool
IncludeOdysseyEditorSpecificStyles()
{
#if IS_PROGRAM
    return true;
#else
    return GIsEditor;
#endif
}

//---

class FOdysseyStyleDefault
    : public FSlateStyleSet
{
public:
    FOdysseyStyleDefault();

public:
    void Initialize();
    void SetupGeneralStyles();
    void SetupClassIconsAndThumbnails();
    void SetupOdysseyCommands();
    void SetupOdysseyAboutWindow();

public:
    const FVector2D mIcon7x16;
    const FVector2D mIcon8x4;
    const FVector2D mIcon16x4;
    const FVector2D mIcon8x8;
    const FVector2D mIcon10x10;
    const FVector2D mIcon12x12;
    const FVector2D mIcon12x16;
    const FVector2D mIcon14x14;
    const FVector2D mIcon16x16;
    const FVector2D mIcon16x20;
    const FVector2D mIcon20x20;
    const FVector2D mIcon22x22;
    const FVector2D mIcon24x24;
    const FVector2D mIcon25x25;
    const FVector2D mIcon30x30;
    const FVector2D mIcon32x32;
    const FVector2D mIcon40x40;
    const FVector2D mIcon48x48;
    const FVector2D mIcon64x64;
    const FVector2D mIcon36x24;
    const FVector2D mIcon128x128;

    const TSharedRef< FLinearColor > mDefaultForeground_LinearRef;
    const FSlateColor mDefaultForeground;

    FButtonStyle mButton;
    FComboButtonStyle mComboButton;

    FTableRowStyle mCoreTableRowStyle;
};

//---

FOdysseyStyleDefault::FOdysseyStyleDefault()
    : FSlateStyleSet( "OdysseyStyle" )

    // Note, these sizes are in Slate Units.
    // Slate Units do NOT have to map to pixels.
    , mIcon7x16( 7.0f, 16.0f )
    , mIcon8x4( 8.0f, 4.0f )
    , mIcon16x4( 16.0f, 4.0f )
    , mIcon8x8( 8.0f, 8.0f )
    , mIcon10x10( 10.0f, 10.0f )
    , mIcon12x12( 12.0f, 12.0f )
    , mIcon12x16( 12.0f, 16.0f )
    , mIcon14x14( 14.0f, 14.0f )
    , mIcon16x16( 16.0f, 16.0f )
    , mIcon16x20( 16.0f, 20.0f )
    , mIcon20x20( 20.0f, 20.0f )
    , mIcon22x22( 22.0f, 22.0f )
    , mIcon24x24( 24.0f, 24.0f )
    , mIcon25x25( 25.0f, 25.0f )
    , mIcon30x30( 30.0f, 30.0f )
    , mIcon32x32( 32.0f, 32.0f )
    , mIcon40x40( 40.0f, 40.0f )
    , mIcon48x48( 48.0f, 48.0f )
    , mIcon64x64( 64.0f, 64.0f )
    , mIcon36x24( 36.0f, 24.0f )
    , mIcon128x128( 128.0f, 128.0f )

    , mDefaultForeground_LinearRef( MakeShareable( new FLinearColor( 0.72f, 0.72f, 0.72f, 1.f ) ) )
    , mDefaultForeground( mDefaultForeground_LinearRef )

    , mButton()
    , mComboButton()

    , mCoreTableRowStyle()
{
}

//---

void
FOdysseyStyleDefault::Initialize()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) / TEXT( "OdysseyAssetResources" ) );
    SetCoreContentRoot( IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) / TEXT( "OdysseyAssetResources" ) );

    // Avoid polluting the game texture atlas with non-core editor style items when not the editor (or a standalone application, like UFE)
    if( !IncludeOdysseyEditorSpecificStyles() )
        return;

    SetupGeneralStyles();
    SetupClassIconsAndThumbnails();
    SetupOdysseyCommands();
    SetupOdysseyAboutWindow();
}

void
FOdysseyStyleDefault::SetupGeneralStyles()
{
    mCoreTableRowStyle = FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");
    mComboButton = FAppStyle::Get().GetWidgetStyle< FComboButtonStyle >( "ComboButton" );
}

void
FOdysseyStyleDefault::SetupClassIconsAndThumbnails()
{
    // Generic Button styles
    Set( "Button.NoPadding", FButtonStyle(FAppStyle::Get().GetWidgetStyle< FButtonStyle >( "Button" ))
        .SetNormalPadding( FMargin(0,0) )
        .SetPressedPadding( FMargin(0,0) )
    );
    Set( "Button.Transparent", FButtonStyle()
        .SetNormal ( FSlateNoResource() )
        .SetPressed( FSlateNoResource() )
        .SetHovered( FSlateNoResource() )
    );
    Set( "Button.TransparentNoPadding", FButtonStyle()
        .SetNormal ( FSlateNoResource() )
        .SetPressed( FSlateNoResource() )
        .SetHovered( FSlateNoResource() )
        .SetNormalPadding( FMargin(0,0) )
        .SetPressedPadding( FMargin(0,0) )
    );
    Set( "SimpleButton.NoPadding", FButtonStyle(FAppStyle::Get().GetWidgetStyle< FButtonStyle >( "SimpleButton" ))
        .SetNormalPadding( FMargin(0,0) )
        .SetPressedPadding( FMargin(0,0) )
    );

    // OdysseyTexture
    Set( "ClassIcon.OdysseyTexture", new IMAGE_BRUSH( "OdysseyTexture/OdysseyTextureDefaultThumbnail_16", mIcon16x16 ) );
    Set( "ClassThumbnail.OdysseyTexture", new IMAGE_BRUSH( "OdysseyTexture/OdysseyTextureDefaultThumbnail_64", mIcon64x64 ) );
    Set( "ClassIcon.Texture2D", new IMAGE_BRUSH( "OdysseyTexture/OdysseyTextureDefaultThumbnail_16", mIcon16x16 ) );
    Set( "ClassThumbnail.Texture2D", new IMAGE_BRUSH( "OdysseyTexture/OdysseyTextureDefaultThumbnail_64", mIcon64x64 ) );
    Set( "ClassIcon.OdysseyFlipbook", new IMAGE_BRUSH("OdysseyFlipbook/OdysseyFlipbook_16x", mIcon16x16));
    Set( "ClassThumbnail.OdysseyFlipbook", new IMAGE_BRUSH("OdysseyFlipbook/OdysseyFlipbook_64x", mIcon64x64));

    Set( "OdysseyTexture.ExportTexture_16", new IMAGE_BRUSH("OdysseyTexture/ExportTexture_16", mIcon16x16));

    // OdysseyBrush
    Set( "ClassIcon.OdysseyBrush", new IMAGE_BRUSH( "Brush/OdysseyBrushDefaultThumbnail_16", mIcon16x16 ) );
    Set( "ClassThumbnail.OdysseyBrush", new IMAGE_BRUSH( "Brush/OdysseyBrushDefaultThumbnail_64", mIcon64x64 ) );
    Set( "ClassIcon.OdysseyBrushAssetBase", new IMAGE_BRUSH( "Brush/OdysseyBrushDefaultThumbnail_16", mIcon16x16 ) );
    Set( "ClassThumbnail.OdysseyBrushAssetBase", new IMAGE_BRUSH( "Brush/OdysseyBrushDefaultThumbnail_64", mIcon64x64 ) );

    // OdysseyPalette
    Set("ClassIcon.OdysseyPalette", new IMAGE_BRUSH( "OdysseyPalette/OdysseyPaletteDefaultThumbnail_16", mIcon16x16 ));
    Set("ClassThumbnail.OdysseyPalette", new IMAGE_BRUSH("OdysseyPalette/OdysseyPaletteDefaultThumbnail_64", mIcon64x64));
    Set("OdysseyPalette.EntryColor", new IMAGE_BRUSH_SVG("OdysseyPalette/OdysseyPaletteEntryColor", mIcon16x16));
    Set("OdysseyPalette.AddColor", new IMAGE_BRUSH_SVG("OdysseyPalette/OdysseyPaletteAddColor", mIcon24x24));
    Set("OdysseyPalette.Row", FTableRowStyle(mCoreTableRowStyle)
        .SetEvenRowBackgroundBrush(FSlateColorBrush(FStyleColors::Panel))
        .SetEvenRowBackgroundHoveredBrush(FSlateColorBrush(FStyleColors::Hover))
        .SetOddRowBackgroundBrush(FSlateColorBrush(FStyleColors::Panel))
        .SetOddRowBackgroundHoveredBrush(FSlateColorBrush(FStyleColors::Hover))
    );

    Set( "OdysseyPalette.BadgeShape", new FSlateRoundedBoxBrush( FStyleColors::White, FVector4( 5.0f, 5.0f, 5.0f, 5.0f ) ) );

    FCheckBoxStyle control_style = FCheckBoxStyle( FAppStyle::Get().GetWidgetStyle<FSegmentedControlStyle>( "SegmentedControl" ).ControlStyle )
        .SetCheckedImage( FSlateRoundedBoxBrush( FStyleColors::Primary, CoreStyleConstants::InputFocusRadius ) )
        .SetCheckedHoveredImage( FSlateRoundedBoxBrush( FStyleColors::PrimaryHover, CoreStyleConstants::InputFocusRadius ) )
        .SetCheckedPressedImage( FSlateRoundedBoxBrush( FStyleColors::PrimaryPress, CoreStyleConstants::InputFocusRadius ) );
    Set( "OdysseyPalette.Tabs", FSegmentedControlStyle( FAppStyle::Get().GetWidgetStyle<FSegmentedControlStyle>( "SegmentedControl" ) )
         .SetControlStyle( control_style )
         .SetFirstControlStyle( control_style )
         .SetLastControlStyle( control_style )
    );

    // OdysseyWidgets - AdvancedColorWheel
    FVector2D AdvancedColorWheelSize( 1024, 1024 );
    Set( "AdvancedColorWheel.WheelBG", new IMAGE_BRUSH( "Color/AdvancedColorWheel/WheelBG", AdvancedColorWheelSize ) );
    Set( "AdvancedColorWheel.InnerWheelBG", new IMAGE_BRUSH( "Color/AdvancedColorWheel/InnerWheelBG", AdvancedColorWheelSize ) );
    Set( "AdvancedColorWheel.InnerWheelHue", new IMAGE_BRUSH( "Color/AdvancedColorWheel/InnerWheelHue", AdvancedColorWheelSize ) );
    Set( "AdvancedColorWheel.InnerWheelDropShadow", new IMAGE_BRUSH( "Color/AdvancedColorWheel/InnerWheelDropShadow", AdvancedColorWheelSize ) );
    Set( "AdvancedColorWheel.TriangleOverlay", new IMAGE_BRUSH( "Color/AdvancedColorWheel/TriangleOverlay", AdvancedColorWheelSize ) );

    Set( "AdvancedColorWheel.HueCursor", new IMAGE_BRUSH( "Color/AdvancedColorWheel/HueCursor", FVector2D( 66, 66 ) ) );
    Set( "AdvancedColorWheel.HueCursorBG", new IMAGE_BRUSH( "Color/AdvancedColorWheel/HueCursorBG", FVector2D( 66, 66 ) ) );
    Set( "AdvancedColorWheel.TriangleCursor", new IMAGE_BRUSH( "Color/AdvancedColorWheel/TriangleCursor", FVector2D( 54, 54 ) ) );
    Set( "AdvancedColorWheel.TriangleCursorBG", new IMAGE_BRUSH( "Color/AdvancedColorWheel/TriangleCursorBG", FVector2D( 54, 54 ) ) );
    Set( "AdvancedColorWheel.CursorOverlay", new IMAGE_BRUSH( "Color/AdvancedColorWheel/CursorOverlay", FVector2D( 406, 59 ) ) );

    Set( "AdvancedColorWheel.HintColorA", new IMAGE_BRUSH( "Color/AdvancedColorWheel/HintColorA", FVector2D( 223, 221 ) ) );
    Set( "AdvancedColorWheel.HintColorB", new IMAGE_BRUSH( "Color/AdvancedColorWheel/HintColorB", FVector2D( 100, 89 ) ) );

    //PainterEditor
    Set( "PainterEditor.Tools", new IMAGE_BRUSH_SVG( "PainterEditor/tools", mIcon16x16 ) );
    Set( "PainterEditor.Layers16", new IMAGE_BRUSH_SVG( "PainterEditor/layers", mIcon16x16 ) );
    Set( "PainterEditor.Viewport16", new IMAGE_BRUSH_SVG( "PainterEditor/viewport", mIcon16x16 ) );

    Set("PainterEditor.FlipVertical32", new IMAGE_BRUSH_SVG("PainterEditor/flip_vertical", mIcon32x32));
    Set("PainterEditor.FlipHorizontal32", new IMAGE_BRUSH_SVG("PainterEditor/flip_horizontal", mIcon32x32));
    Set("PainterEditor.FlipVertical16", new IMAGE_BRUSH_SVG("PainterEditor/flip_vertical_2", mIcon16x16));
    Set("PainterEditor.FlipHorizontal16", new IMAGE_BRUSH_SVG("PainterEditor/flip_horizontal_2", mIcon16x16));

    Set( "PainterEditor.Mesh16", new IMAGE_BRUSH_SVG( "PainterEditor/mesh_selector", mIcon16x16 ) );

    Set( "PainterEditor.RotateLeft", new IMAGE_BRUSH_SVG( "PainterEditor/rotate_left", mIcon16x16 ) );
    Set( "PainterEditor.RotateRight", new IMAGE_BRUSH_SVG( "PainterEditor/rotate_right", mIcon16x16 ) );

    Set( "PainterEditor.ColorWheel16", new IMAGE_BRUSH_SVG( "PainterEditor/color_wheel", mIcon16x16 ) );

    Set( "PainterEditor.DetailsTab", new IMAGE_BRUSH_SVG( "PainterEditor/details_panel", mIcon16x16 ) );

    Set( "PainterEditor.RotateReset", new IMAGE_BRUSH_SVG( "PainterEditor/rotate_reset", mIcon16x16 ) );

    //PainterEditor - Vector Scene Tree View
    Set( "PainterEditor.VectorSceneTreeView.Paintgroup", new IMAGE_BRUSH_SVG( "OdysseyVectorSceneTreeView/paintgroup", mIcon16x16 ) );
    Set( "PainterEditor.VectorSceneTreeView.Path", new IMAGE_BRUSH_SVG( "OdysseyVectorSceneTreeView/path", mIcon16x16 ) );
    Set( "PainterEditor.VectorSceneTreeView.Group", new IMAGE_BRUSH_SVG( "OdysseyVectorSceneTreeView/group", mIcon16x16 ) );
    Set( "PainterEditor.VectorSceneTreeView.MenuIcon", new IMAGE_BRUSH_SVG( "OdysseyVectorSceneTreeView/MenuIcon", mIcon16x16 ) );

    Set("VectorSceneTreeView.IsVisibleToggle", FCheckBoxStyle()
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

    //PainterEditor - Top Bar
    Set( "PainterEditor.TopBar.Undo32", new IMAGE_BRUSH_SVG( "PainterEditor/undo", mIcon32x32 ) );
    Set( "PainterEditor.TopBar.Redo32", new IMAGE_BRUSH_SVG( "PainterEditor/redo", mIcon32x32 ) );
    Set( "PainterEditor.TopBar.Save32", new IMAGE_BRUSH_SVG( "PainterEditor/save", mIcon32x32 ) );
    Set( "PainterEditor.TopBar.Eraser32", new IMAGE_BRUSH_SVG( "PainterEditor/eraser", mIcon32x32 ) );
    Set( "PainterEditor.TopBar.Clear32", new IMAGE_BRUSH_SVG( "PainterEditor/trash", mIcon32x32 ) );
    Set( "PainterEditor.TopBar.VectorModeVertex32",new IMAGE_BRUSH_SVG("PainterEditor/vector_mode_vertex",mIcon32x32));
    Set( "PainterEditor.TopBar.VectorModeObject32",new IMAGE_BRUSH_SVG("PainterEditor/vector_mode_object",mIcon32x32));
    Set( "PainterEditor.TopBar.VectorModeInbetween32",new IMAGE_BRUSH_SVG("PainterEditor/vector_mode_inbetween",mIcon32x32));

    //PainterEditor - ToolsTab
    Set( "PainterEditor.ToolsTab.Transform64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/transform_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Transform16", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/transform_tool", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.ScenePan64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/hand", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PathCut64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_cut_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PathPush64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_push_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PathSmooth64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_smooth_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PathStitch64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_stitch_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Lasso64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/lasso_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Grid64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/grid_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PathEdit64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_edit_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PathDrawing64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_drawing_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.DrawingTool64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/raster_drawing_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.ColorPicker64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/color_picker_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PaintBucket64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/paint_bucket_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Eraser64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/eraser_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Shapes64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/shapes_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Chart64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/chart_tool", mIcon64x64 ) );
// commented out: this SVG icon is poorly displayed, I don't know why. I then converted it to PNG
//    Set( "PainterEditor.ToolsTab.Trajectory64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/trajectory_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Trajectory64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/trajectory_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Matching16", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/matching_tool", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.Matching64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/matching_tool", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Liquify64", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/liquify_tool", mIcon64x64 ) );

    Set("PainterEditor.ColorSelector.ColorType.Raw", new IMAGE_BRUSH_SVG( "PainterEditor/ColorSelector/color_type_raw", mIcon16x16 ));
    Set("PainterEditor.ColorSelector.ColorType.Indexed", new IMAGE_BRUSH_SVG( "PainterEditor/ColorSelector/color_type_indexed", mIcon16x16 ));

    //Tools Modifier keys shortcuts
    Set( "PainterEditor.ToolsShortcuts.PathEditAddRemoveVertex20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_edit_tool_add_remove_vertex", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.PathEditDeformSegment20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_edit_tool_deform_segment", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.PathEditMoveVertex20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_edit_tool_move_vertex", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.PathEditWidenVertex20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/path_edit_tool_widen_vertex", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.EraserEraseDefault20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/eraser_tool_erase_default", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.EraserEraseToIntersection20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/eraser_tool_erase_to_intersection", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.EraserEraseWholePath20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/eraser_tool_erase_whole_path", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.TrajectoryAdd20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/trajectory_tool_add", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.TrajectoryEditCurve20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/trajectory_tool_edit_curve", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.TrajectoryEditSpacing20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/trajectory_tool_edit_spacing", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.ChartOnyByOne20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/chart_tool_one_by_one", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.ChartRelative20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/chart_tool_relative", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.ChartEaseInOrOut20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/chart_tool_ease_in_or_out", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.ChartMagnet20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/chart_tool_magnet", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.ChartReshape20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/chart_tool_reshape", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.PaintBucketAddMove20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/paint_bucket_tool_add_move", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.PaintBucketRemove20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/paint_bucket_tool_remove", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.PaintBucketRadialLinear20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/paint_bucket_tool_radial_linear", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.GridSingleSelection20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/grid_tool_single_selection", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.GridMultipleSelection20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/grid_tool_multiple_selection", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.TransformScalingOppositeCorner20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/transform_tool_scaling_opposite_corner", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.TransformScalingCenter20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/transform_tool_scaling_center", mIcon20x20 ) );
    Set( "PainterEditor.ToolsShortcuts.TransformScalingGizmo20", new IMAGE_BRUSH_SVG( "PainterEditor/ToolsTab/transform_tool_scaling_gizmo", mIcon20x20 ) );

    //OdysseyLayerStack
    Set( "OdysseyLayerStack.LayerBitmap16", new IMAGE_BRUSH( "OdysseyLayerStack/layer_bitmap_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.LayerVector16", new IMAGE_BRUSH( "OdysseyLayerStack/layer_vector_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.Visible16", new IMAGE_BRUSH_SVG( "OdysseyLayerStack/visible", mIcon16x16 ) );
    Set( "OdysseyLayerStack.Locked16", new IMAGE_BRUSH_SVG( "OdysseyLayerStack/locked", mIcon16x16 ) );
    Set( "OdysseyLayerStack.OptionsHeader16", new IMAGE_BRUSH_SVG( "OdysseyLayerStack/options_header", mIcon16x16 ) );

    Set("LayerStack.DisplayOptionsToggle", FCheckBoxStyle()
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/options_hidden", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/options_hidden", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/options_hidden", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/options_displayed", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/options_displayed", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/options_displayed", mIcon16x16))
    );

    Set("LayerStack.IsActivatedToggle", FCheckBoxStyle()
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/not_visible", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/not_visible", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/not_visible", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/visible", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/visible", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/visible", mIcon16x16))
        .SetUndeterminedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/undetermined", mIcon16x16))
        .SetUndeterminedHoveredImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/undetermined", mIcon16x16))
        .SetUndeterminedPressedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/undetermined", mIcon16x16))
    );

    Set("LayerStack.IsLockedToggle", FCheckBoxStyle()
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/unlocked", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/unlocked", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/unlocked", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/locked", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/locked", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/locked", mIcon16x16))
        .SetUndeterminedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/undetermined", mIcon16x16))
        .SetUndeterminedHoveredImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/undetermined", mIcon16x16))
        .SetUndeterminedPressedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/undetermined", mIcon16x16))
    );

    //OdysseyViewportDrawingEditor
    Set("OdysseyViewportDrawingEditMode.OdysseyViewportDrawingIcon16",new IMAGE_BRUSH_SVG("OdysseyViewportDrawing/viewport_drawing_icon",mIcon16x16));
    Set("OdysseyViewportDrawingEditMode.OdysseyViewportDrawingIcon40",new IMAGE_BRUSH_SVG("OdysseyViewportDrawing/viewport_drawing_icon",mIcon40x40));

    //LayerStack
    {
        FSlateColor selectedRow(FStyleColors::Select.GetSpecifiedColor().CopyWithNewOpacity(0.3f));
        FSlateColor selectedInactiveRow(FStyleColors::SelectInactive.GetSpecifiedColor().CopyWithNewOpacity(0.3f));
        FSlateColor selectedParentRow(FStyleColors::SelectParent.GetSpecifiedColor().CopyWithNewOpacity(0.3f));
        Set ("OdysseyLayerStack.CurrentLayerBackgroundBrush", new FSlateColorBrush(FStyleColors::Select));
        Set ("OdysseyLayerStack.CurrentLayerInactiveBackgroundBrush", new FSlateColorBrush(FStyleColors::SelectInactive));
        Set( "OdysseyLayerStack.AlternatedRows", FTableRowStyle(mCoreTableRowStyle)
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

    Set("OdysseyCheckBoxStyle.ToggleButton", FCheckBoxStyle( FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckbox") )
        .SetPadding(FMargin())
    );

    //For flipbook
    Set("FlipbookTimeline.TimelineFrameBackground", new BOX_BRUSH("OdysseyFlipbook/TimelineFrameBackground", FMargin(4.f / 16.f)));
    Set("FlipbookTimeline.TimelineFrameTimingHandle", new IMAGE_BRUSH("OdysseyFlipbook/TimelineFrameTimingHandle", FVector2D(16.f, 16.f)));
    Set("FlipbookTimeline.TimelineFrameLengthHandle", new IMAGE_BRUSH("OdysseyFlipbook/TimelineFrameLengthHandle", FVector2D(16.f, 16.f)));
    Set("FlipbookTimeline.TimelineFrameWarning", new IMAGE_BRUSH("OdysseyFlipbook/icon_warning_16x", FVector2D(16.f, 16.f)));
    Set("FlipbookTimeline.BackgroundColorEven", FLinearColor(1.0f, 1.0f, 1.0f, 0.05f));
    Set("FlipbookTimeline.BackgroundColorOdd", FLinearColor(0.0f, 0.0f, 0.0f, 0.05f));


    //PlaybackControls
    Set("PlaybackControls.Play", FButtonStyle(mButton)
        .SetNormal(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Play_OFF_24", mIcon24x24))
        .SetHovered(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Play_OFF_24", mIcon24x24))
        .SetPressed(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Play_ON_24", mIcon24x24))
    );

    //PlaybackControls
    Set("PlaybackControls.PlayBackward", FButtonStyle(mButton)
        .SetNormal(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_PlayBackward_OFF_24", mIcon24x24))
        .SetHovered(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_PlayBackward_OFF_24", mIcon24x24))
        .SetPressed(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_PlayBackward_ON_24", mIcon24x24))
    );

    Set("PlaybackControls.Pause", FButtonStyle(mButton)
        .SetNormal(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Pause_OFF_24", mIcon24x24))
        .SetHovered(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Pause_OFF_24", mIcon24x24))
        .SetPressed(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Pause_ON_24", mIcon24x24))
    );

    Set("PlaybackControls.Stop", FButtonStyle(mButton)
        .SetNormal(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Stop_OFF_24", mIcon24x24))
        .SetHovered(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Stop_OFF_24", mIcon24x24))
        .SetPressed(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Stop_ON_24", mIcon24x24))
    );

    Set("PlaybackControls.Beginning", FButtonStyle(mButton)
        .SetNormal(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Beginning_OFF_24", mIcon24x24))
        .SetHovered(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Beginning_OFF_24", mIcon24x24))
        .SetPressed(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Beginning_ON_24", mIcon24x24))
    );

    Set("PlaybackControls.End", FButtonStyle(mButton)
        .SetNormal(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_End_OFF_24", mIcon24x24))
        .SetHovered(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_End_OFF_24", mIcon24x24))
        .SetPressed(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_End_ON_24", mIcon24x24))
    );

    Set("PlaybackControls.Previous", FButtonStyle(mButton)
        .SetNormal(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Previous_OFF_24", mIcon24x24))
        .SetHovered(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Previous_OFF_24", mIcon24x24))
        .SetPressed(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Previous_ON_24", mIcon24x24))
    );

    Set("PlaybackControls.PreviousKey", FButtonStyle(mButton)
        .SetNormal(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_PreviousKey_OFF_24", mIcon24x24))
        .SetHovered(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_PreviousKey_OFF_24", mIcon24x24))
        .SetPressed(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_PreviousKey_ON_24", mIcon24x24))
    );

    Set("PlaybackControls.Next", FButtonStyle(mButton)
        .SetNormal(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Next_OFF_24", mIcon24x24))
        .SetHovered(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Next_OFF_24", mIcon24x24))
        .SetPressed(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Next_ON_24", mIcon24x24))
    );

    Set("PlaybackControls.NextKey", FButtonStyle(mButton)
        .SetNormal(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_NextKey_OFF_24", mIcon24x24))
        .SetHovered(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_NextKey_OFF_24", mIcon24x24))
        .SetPressed(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_NextKey_ON_24", mIcon24x24))
    );

    Set("PlaybackControls.Looping", FButtonStyle(mButton)
        .SetNormal(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Looping_OFF_24", mIcon24x24))
        .SetHovered(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Looping_OFF_24", mIcon24x24))
        .SetPressed(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_Looping_ON_24", mIcon24x24))
    );

    Set("PlaybackControls.NotLooping", FButtonStyle(mButton)
        .SetNormal(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_NotLooping_OFF_24", mIcon24x24))
        .SetHovered(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_NotLooping_OFF_24", mIcon24x24))
        .SetPressed(IMAGE_BRUSH("OdysseyFlipbook/PlaybackControls_NotLooping_ON_24", mIcon24x24))
    );

    //Animation
    Set( "ClassIcon.OdysseyAnimation", new IMAGE_BRUSH( "OdysseyAnimation/OdysseyAnimationDefaultThumbnail_16", mIcon16x16 ) );
    Set( "ClassThumbnail.OdysseyAnimation", new IMAGE_BRUSH( "OdysseyAnimation/OdysseyAnimationDefaultThumbnail_64", mIcon64x64 ) );

    Set("TimelineHeader.backgroundColorEven", FLinearColor(1.0f, 1.0f, 1.0f, 0.05f));
    Set("TimelineHeader.backgroundColorOdd", FLinearColor(0.0f, 0.0f, 0.0f, 0.05f));

    Set("Animation.Timeline.Padding", 28.f);

    Set( "Animation.Timeline.LockedOverlay", new IMAGE_BRUSH( "OdysseyAnimation/Timeline/layer-locked", mIcon10x10 /* seems to not being used in tiling mode */, FLinearColor( 0, 0, 0, .5f ), ESlateBrushTileType::Both ) );
    Set( "Animation.Timeline.DeactivatedOverlay", new FSlateColorBrush( FLinearColor( 0, 0, 0, 0.75f ) ) );

    Set("Animation.Timeline.Tools.Selection", new IMAGE_BRUSH_SVG("OdysseyAnimation/Timeline/Tools/Selection", mIcon16x16));
    Set("Animation.Timeline.Tools.Move", new IMAGE_BRUSH_SVG("OdysseyAnimation/Timeline/Tools/Move", mIcon16x16));
    Set("Animation.Timeline.Tools.Cut", new IMAGE_BRUSH_SVG("OdysseyAnimation/Timeline/Tools/Cut", mIcon16x16));

    Set("Animation.Timeline.Inbetweening.Forward16", new IMAGE_BRUSH("OdysseyAnimation/Timeline/Inbetweening/forward16", mIcon16x16));
    Set("Animation.Timeline.Inbetweening.Backward16", new IMAGE_BRUSH("OdysseyAnimation/Timeline/Inbetweening/backward16", mIcon16x16));

    Set("Animation.Timeline.OutOfRangeColor", FStyleColors::AccentBlack);
    Set("Animation.Timeline.OutOfRangeColorOpacity", 0.3f);

    Set("Animation.Timeline.OutOfBoundColor", FStyleColors::AccentBlack);
    Set("Animation.Timeline.OutOfBoundColorOpacity", 0.3f);

    Set("Animation.Timeline.LeftBoundColor", FStyleColors::AccentGreen);
    Set("Animation.Timeline.RightBoundColor", FStyleColors::AccentRed);

    Set("Animation.Timeline.LeftBoundHandle", new IMAGE_BRUSH_SVG("OdysseyAnimation/Timeline/Header/AnimationLeftBoundHandle", mIcon16x16));
    Set("Animation.Timeline.RightBoundHandle", new IMAGE_BRUSH_SVG("OdysseyAnimation/Timeline/Header/AnimationRightBoundHandle", mIcon16x16));

    Set("Animation.Lighttable16", new IMAGE_BRUSH_SVG("OdysseyAnimation/lighttable-off", mIcon16x16));

    Set("Animation.LighttableToggle", FCheckBoxStyle()
        //.Padding(FMargin(0,0,0,0))
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH_SVG("OdysseyAnimation/lighttable-off", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyAnimation/lighttable-off", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyAnimation/lighttable-off", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH_SVG("OdysseyAnimation/lighttable-on", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyAnimation/lighttable-on", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyAnimation/lighttable-on", mIcon16x16))
    );

    Set("Animation.CellNamesToggle", FCheckBoxStyle()
        //.Padding(FMargin(0,0,0,0))
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH_SVG("OdysseyAnimation/cell-names-off", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyAnimation/cell-names-off", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyAnimation/cell-names-off", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH_SVG("OdysseyAnimation/cell-names-on", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyAnimation/cell-names-on", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyAnimation/cell-names-on", mIcon16x16))
    );

    Set("Animation.AlphaLockedToggle", FCheckBoxStyle()
        //.Padding(FMargin(0,0,0,0))
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/alpha_unlocked", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/alpha_unlocked", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/alpha_unlocked", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/alpha_locked", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/alpha_locked", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/alpha_locked", mIcon16x16))
    );

    Set("Animation.Lighttable.Options", FComboButtonStyle(mComboButton)
        .SetShadowOffset(FVector2D(0))
        .SetButtonStyle( FButtonStyle(mButton)
            .SetNormalPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
            .SetPressedPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
            .SetNormal(IMAGE_BRUSH_SVG("OdysseyAnimation/Lighttable_Options_OFF", mIcon16x16))
            .SetHovered(IMAGE_BRUSH_SVG("OdysseyFlipbook/Lighttable_Options_OFF", mIcon16x16))
            .SetPressed(IMAGE_BRUSH_SVG("OdysseyFlipbook/Lighttable_Options_ON", mIcon16x16))
        )
    );

    Set("Animation.Lighttable.Options.DisplayPosition.AboveLayer", new IMAGE_BRUSH_SVG("OdysseyAnimation/Lighttable_Options_DisplayPosition_AboveLayer", mIcon16x16));
    Set("Animation.Lighttable.Options.DisplayPosition.UnderLayer", new IMAGE_BRUSH_SVG("OdysseyAnimation/Lighttable_Options_DisplayPosition_UnderLayer", mIcon16x16));

    Set("Animation.CellNames.Options", FComboButtonStyle(mComboButton)
        .SetShadowOffset(FVector2D(0))
        .SetButtonStyle( FButtonStyle(mButton)
            .SetNormalPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
            .SetPressedPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
            .SetNormal(IMAGE_BRUSH_SVG("OdysseyAnimation/Lighttable_Options_OFF", mIcon16x16)) //TODO: use its own icon
            .SetHovered(IMAGE_BRUSH_SVG("OdysseyFlipbook/Lighttable_Options_OFF", mIcon16x16)) //TODO: use its own icon
            .SetPressed(IMAGE_BRUSH_SVG("OdysseyFlipbook/Lighttable_Options_ON", mIcon16x16)) //TODO: use its own icon
        )
    );


    Set("Animation.AddCellsHandleRight", new IMAGE_BRUSH_SVG("OdysseyAnimation/AddCellsHandleRight", mIcon16x16));
    Set("Animation.AddCellsHandleLeft", new IMAGE_BRUSH_SVG("OdysseyAnimation/AddCellsHandleLeft", mIcon16x16));
    Set("Animation.CellTimingHandle", new IMAGE_BRUSH_SVG("OdysseyAnimation/CellTimingHandle", mIcon16x16));
    Set("Animation.CellExposureHandle", new IMAGE_BRUSH_SVG("OdysseyAnimation/CellExposureHandle", mIcon16x16));
    Set("Animation.CellBreakIndicator", new IMAGE_BRUSH("OdysseyAnimation/CellBreakIndicator", FVector2D(2, 4), FLinearColor::White, ESlateBrushTileType::Vertical));
    Set("Animation.CellBreakIndicatorExtended", new IMAGE_BRUSH("OdysseyAnimation/CellBreakIndicatorExtended", FVector2D(32, 32), FLinearColor::White, ESlateBrushTileType::Both));

    Set("Animation.Layer.PreBehaviourColor", FLinearColor(1.f, 1.f, 1.f, 1.f));
    Set("Animation.Layer.PreBehaviour.None", new IMAGE_BRUSH_SVG("OdysseyAnimation/Layer/PreBehaviourNone", mIcon16x16));
    Set("Animation.Layer.PreBehaviour.Hold", new IMAGE_BRUSH_SVG("OdysseyAnimation/Layer/PreBehaviourHold", mIcon16x16));
    Set("Animation.Layer.PreBehaviour.Loop", new IMAGE_BRUSH_SVG("OdysseyAnimation/Layer/PreBehaviourLoop", mIcon16x16));
    Set("Animation.Layer.PreBehaviour.PingPong", new IMAGE_BRUSH_SVG("OdysseyAnimation/Layer/PreBehaviourPingPong", mIcon16x16));

    Set("Animation.Layer.PostBehaviourColor", FLinearColor(1.f, 1.f, 1.f, 1.f));
    Set("Animation.Layer.PostBehaviour.None", new IMAGE_BRUSH_SVG("OdysseyAnimation/Layer/PostBehaviourNone", mIcon16x16));
    Set("Animation.Layer.PostBehaviour.Hold", new IMAGE_BRUSH_SVG("OdysseyAnimation/Layer/PostBehaviourHold", mIcon16x16));
    Set("Animation.Layer.PostBehaviour.Loop", new IMAGE_BRUSH_SVG("OdysseyAnimation/Layer/PostBehaviourLoop", mIcon16x16));
    Set("Animation.Layer.PostBehaviour.PingPong", new IMAGE_BRUSH_SVG("OdysseyAnimation/Layer/PostBehaviourPingPong", mIcon16x16));

    Set("Animation.CellImageStagger.Reach.SpinBoxStyle", FSpinBoxStyle(FAppStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBox"))
        .SetBackgroundBrush(FSlateNoResource())
        .SetHoveredBackgroundBrush(FSlateRoundedBoxBrush(FStyleColors::Dropdown, 4.0f))
    );

    Set("Animation.CellImageStagger.PreviewReachColor", FLinearColor(0.2f, 0.3f, 1.0f, 0.5f));
    Set("Animation.CellImageStagger.ArrowPreviewReach", new IMAGE_BRUSH("OdysseyAnimation/CellImageStaggerArrowPreviewReach", mIcon32x32, FLinearColor::White, ESlateBrushTileType::Horizontal));
    Set("Animation.CellImageStagger.ArrowsColor", FLinearColor(1.f, 1.f, 1.f, 1.f));
    Set("Animation.CellImageStagger.ArrowTop", new IMAGE_BRUSH("OdysseyAnimation/CellImageStaggerArrowTop", FVector2D(10, 7), FLinearColor::White, ESlateBrushTileType::Horizontal));
    Set("Animation.CellImageStagger.ArrowBottom", new IMAGE_BRUSH("OdysseyAnimation/CellImageStaggerArrowBottom", FVector2D(10, 7), FLinearColor::White, ESlateBrushTileType::Horizontal));
    Set("Animation.CellImageStagger.StaggerLengthColor", FLinearColor(1.f, 1.f, 1.f, 1.f));
    Set("Animation.CellImageStagger.BehaviourColor", FLinearColor(1.f, 1.f, 1.f, 1.f));
    Set("Animation.CellImageStagger.Behaviour.Loop", new IMAGE_BRUSH_SVG("OdysseyAnimation/CellImageStaggerBehaviourLoop", mIcon16x16));
    Set("Animation.CellImageStagger.Behaviour.PingPong", new IMAGE_BRUSH_SVG("OdysseyAnimation/CellImageStaggerBehaviourPingPong", mIcon16x16));
    Set("Animation.CellImageStagger.Behaviour.Random", new IMAGE_BRUSH_SVG("OdysseyAnimation/CellImageStaggerBehaviourRandom", mIcon16x16));

    Set("Animation.CellMark.Symbol.Triangle", new IMAGE_BRUSH_SVG("OdysseyAnimation/cell-mark-symbol-triangle", mIcon16x16));
    Set("Animation.CellMark.Symbol.Diamond", new IMAGE_BRUSH_SVG("OdysseyAnimation/cell-mark-symbol-diamond", mIcon16x16));
    Set("Animation.CellMark.Symbol.Circle", new IMAGE_BRUSH_SVG("OdysseyAnimation/cell-mark-symbol-circle", mIcon16x16));
    Set("Animation.CellMark.Symbol.Star", new IMAGE_BRUSH_SVG("OdysseyAnimation/cell-mark-symbol-star", mIcon16x16));
    Set("Animation.CellMark.Symbol.Filled.Triangle", new IMAGE_BRUSH_SVG("OdysseyAnimation/cell-mark-symbol-filled-triangle", mIcon16x16));
    Set("Animation.CellMark.Symbol.Filled.Diamond", new IMAGE_BRUSH_SVG("OdysseyAnimation/cell-mark-symbol-filled-diamond", mIcon16x16));
    Set("Animation.CellMark.Symbol.Filled.Circle", new IMAGE_BRUSH_SVG("OdysseyAnimation/cell-mark-symbol-filled-circle", mIcon16x16));
    Set("Animation.CellMark.Symbol.Filled.Star", new IMAGE_BRUSH_SVG("OdysseyAnimation/cell-mark-symbol-filled-star", mIcon16x16));
    Set("Animation.CellMark.Symbol.Cross", new IMAGE_BRUSH_SVG("OdysseyAnimation/cell-mark-symbol-cross", mIcon16x16));
    Set("Animation.CellMark.Symbol.Checkmark", new IMAGE_BRUSH_SVG("OdysseyAnimation/cell-mark-symbol-checkmark", mIcon16x16));
    Set("Animation.CellMark.Symbol.Invalid", new IMAGE_BRUSH_SVG("OdysseyAnimation/cell-mark-symbol-invalid", mIcon16x16));

    FSlateColor primary( FStyleColors::Primary );
    FSlateColor background( FStyleColors::Background );
    FSlateColor input( FStyleColors::Input );
    FSlateColor inputOutline( FStyleColors::InputOutline );
    Set("Animation.LighttableKey.Back", new FSlateRoundedBoxBrush( primary, FVector4( 8.0f, 8.0f, 8.0f, 8.0f ) ));
    Set("Animation.LighttableKey.Front", new FSlateRoundedBoxBrush( background, FVector4( 8.0f, 8.0f, 8.0f, 8.0f ) ));
    Set("Animation.LighttableKey.BackColor", primary);
    Set("Animation.LighttableKey.FrontColor", background);

    Set("Animation.Lighttable.OutOfPegs.Button.On", new IMAGE_BRUSH_SVG("OdysseyAnimation/out-of-pegs-on", mIcon16x16));
    Set("Animation.Lighttable.OutOfPegs.Button.Off", new IMAGE_BRUSH_SVG("OdysseyAnimation/out-of-pegs-off", mIcon16x16));

    // Inbetweening
    Set("Inbetweening.TableView", FTableViewStyle()
        .SetBackgroundBrush(FSlateNoResource())
    );

    Set("Inbetweening.TableRow", FTableRowStyle()
        .SetActiveBrush( FSlateNoResource() )
        .SetActiveHighlightedBrush( FSlateNoResource() )
        .SetActiveHoveredBrush( FSlateNoResource() )
        .SetEvenRowBackgroundBrush( FSlateNoResource() )
        .SetEvenRowBackgroundHoveredBrush(FSlateNoResource() )
        .SetOddRowBackgroundBrush(FSlateNoResource() )
        .SetOddRowBackgroundHoveredBrush(FSlateNoResource() )
        .SetInactiveBrush( FSlateNoResource() )
        .SetInactiveHighlightedBrush(FSlateNoResource() )
        .SetInactiveHoveredBrush( FSlateNoResource() )
        .SetSelectorFocusedBrush( FSlateNoResource() )
    );

    //Texture
    Set("Texture.AlphaLockedToggle", FCheckBoxStyle()
        //.Padding(FMargin(0,0,0,0))
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/alpha_unlocked", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/alpha_unlocked", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/alpha_unlocked", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/alpha_locked", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/alpha_locked", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH_SVG("OdysseyLayerStack/alpha_locked", mIcon16x16))
    );

    Set("Texture.ColoredToggle", FCheckBoxStyle()
        //.Padding(FMargin(0,0,0,0))
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH("OdysseyLayerStack/uncolored_16", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/uncolored_16", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/uncolored_16", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH("OdysseyLayerStack/colored_16", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/colored_16", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/colored_16", mIcon16x16))
    );

    Set("Texture.WireframeToggle", FCheckBoxStyle()
        //.Padding(FMargin(0,0,0,0))
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH("OdysseyLayerStack/wireframe_off_16", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/wireframe_off_16", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/wireframe_off_16", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH("OdysseyLayerStack/wireframe_on_16", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/wireframe_on_16", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/wireframe_on_16", mIcon16x16))
    );

    Set("CheckBox.BasicOnOff", FCheckBoxStyle()
        //.Padding(FMargin(0,0,0,0))
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(FSlateColorBrush(FLinearColor::Transparent))
        .SetUncheckedHoveredImage(FSlateColorBrush(FLinearColor(1, 1, 1, 0.25)))
        .SetUncheckedPressedImage(FSlateColorBrush(FLinearColor(1, 1, 1, 0.25)))
        .SetCheckedImage(FSlateColorBrush(FLinearColor(1, 1, 1, 0.25 )))
        .SetCheckedHoveredImage(FSlateColorBrush(FLinearColor(1, 1, 1, 0.25)))
        .SetCheckedPressedImage(FSlateColorBrush(FLinearColor::Transparent))
    );

    Set("Sequencer.AnimationTimelineTrack.DisplayLayersToggle", FCheckBoxStyle()
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetBorderBackgroundColor(FSlateColor::UseSubduedForeground())
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(*FAppStyle::Get().GetBrush("TreeArrow_Collapsed"))
        .SetUncheckedHoveredImage(*FAppStyle::Get().GetBrush("TreeArrow_Collapsed_Hovered"))
        .SetUncheckedPressedImage(*FAppStyle::Get().GetBrush("TreeArrow_Collapsed_Hovered"))
        .SetCheckedImage(*FAppStyle::Get().GetBrush("TreeArrow_Expanded"))
        .SetCheckedHoveredImage(*FAppStyle::Get().GetBrush("TreeArrow_Expanded_Hovered"))
        .SetCheckedPressedImage(*FAppStyle::Get().GetBrush("TreeArrow_Expanded_Hovered"))
    );

    Set("Sequencer.AnimationTimelineTrack.PrePostBehaviourOverlay", new IMAGE_BRUSH("OdysseyAnimation/Sequencer/PrePostBehaviourOverlay", FVector2D(16.f, 16.f), FLinearColor::White, ESlateBrushTileType::Both));

    Set("Shapes.None", new IMAGE_BRUSH_SVG("OdysseyShapes/none", mIcon16x16));
    Set("Shapes.Line", new IMAGE_BRUSH_SVG("OdysseyShapes/line", mIcon16x16));
    Set("Shapes.Freehand_Filled", new IMAGE_BRUSH_SVG("OdysseyShapes/freehand_filled", mIcon16x16));
    Set("Shapes.Rectangle_Filled", new IMAGE_BRUSH_SVG("OdysseyShapes/rectangle_filled", mIcon16x16));
    Set("Shapes.Polygon_Filled", new IMAGE_BRUSH_SVG("OdysseyShapes/polygon_filled", mIcon16x16));
    Set("Shapes.Ellipse_Filled", new IMAGE_BRUSH_SVG("OdysseyShapes/ellipse_filled", mIcon16x16));
    Set("Shapes.Bezier_3pts_Filled", new IMAGE_BRUSH_SVG("OdysseyShapes/bezier_3pts_filled", mIcon16x16));
    Set("Shapes.Freehand_Empty", new IMAGE_BRUSH_SVG("OdysseyShapes/freehand_empty", mIcon16x16));
    Set("Shapes.Rectangle_Empty", new IMAGE_BRUSH_SVG("OdysseyShapes/rectangle_empty", mIcon16x16));
    Set("Shapes.Polygon_Empty", new IMAGE_BRUSH_SVG("OdysseyShapes/polygon_empty", mIcon16x16));
    Set("Shapes.Ellipse_Empty", new IMAGE_BRUSH_SVG("OdysseyShapes/ellipse_empty", mIcon16x16));
    Set("Shapes.Bezier_3pts_Empty", new IMAGE_BRUSH_SVG("OdysseyShapes/bezier_3pts_empty", mIcon16x16));

    Set("Liquify.Push", new IMAGE_BRUSH_SVG("PainterEditor/ToolsTab/liquify_mode_push", mIcon16x16));
    Set("Liquify.Twirl", new IMAGE_BRUSH_SVG("PainterEditor/ToolsTab/liquify_mode_twirl", mIcon16x16));
    Set("Liquify.Pinch", new IMAGE_BRUSH_SVG("PainterEditor/ToolsTab/liquify_mode_pinch", mIcon16x16));
    Set("Liquify.Expand", new IMAGE_BRUSH_SVG("PainterEditor/ToolsTab/liquify_mode_expand", mIcon16x16));
    Set("Liquify.Crystals", new IMAGE_BRUSH_SVG("PainterEditor/ToolsTab/liquify_mode_crystals", mIcon16x16));
    Set("Liquify.Edge", new IMAGE_BRUSH_SVG("PainterEditor/ToolsTab/liquify_mode_edge", mIcon16x16));
    Set("Liquify.Reconstruct", new IMAGE_BRUSH_SVG("PainterEditor/ToolsTab/liquify_mode_reconstruct", mIcon16x16));
    Set("Liquify.Adjust", new IMAGE_BRUSH_SVG("PainterEditor/ToolsTab/liquify_mode_adjust", mIcon16x16));

    Set("ViewportDrawingEditor.MainTab.ModeTool.Selection", new IMAGE_BRUSH_SVG("OdysseyViewportDrawing/SelectionModeTool", mIcon16x16));
    Set("ViewportDrawingEditor.MainTab.ModeTool.Paint", new IMAGE_BRUSH_SVG("OdysseyViewportDrawing/PaintModeTool", mIcon16x16));

    Set( "PaletteEditor.Tab.Colors", new IMAGE_BRUSH_SVG( "PaletteEditor/tab_colors", mIcon16x16 ) );
}



void
FOdysseyStyleDefault::SetupOdysseyCommands()
{
    //Corresponds to the commands in FOdysseyCommands
    Set("OdysseyCommands.OpenOdysseyAboutWindow", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/Commands/open-about-window", mIcon20x20));
    Set("OdysseyCommands.OpenOdysseyAboutWindow.Small", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/Commands/open-about-window", mIcon20x20));
    Set("OdysseyCommands.OpenOdysseyDocumentation", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/Commands/go-to-user-documentation", mIcon20x20));
    Set("OdysseyCommands.OpenOdysseyDocumentation.Small", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/Commands/go-to-user-documentation", mIcon20x20));
}

void
FOdysseyStyleDefault::SetupOdysseyAboutWindow()
{
    //---

    static const FVector2D Icon30x30(30.0f, 30.0f);

    Set("Odyssey.About.Facebook", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/AboutWindow/facebook", Icon30x30));
    Set("Odyssey.About.LinkedIn", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/AboutWindow/linkedin", Icon30x30));
    Set("Odyssey.About.Youtube", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/AboutWindow/youtube", Icon30x30));
    Set("Odyssey.About.Twitter", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/AboutWindow/twitter", Icon30x30));
    Set("Odyssey.About.Instagram", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/AboutWindow/instagram", Icon30x30));
    Set("Odyssey.About.Discord", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/AboutWindow/discord", Icon30x30));
    Set("Odyssey.About.UserDoc", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/AboutWindow/user-doc", Icon30x30));
    Set("Odyssey.About.Git", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/AboutWindow/github", Icon30x30));
    Set("Odyssey.About.Praxinos", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/AboutWindow/praxinos", mIcon128x128));
    Set("Odyssey.About.Odyssey", new IMAGE_BRUSH_SVG("OdysseyCoreEditor/AboutWindow/odyssey", mIcon128x128));

    Set("Odyssey.About.UnderlineText", FTextBlockStyle(FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalUnderlinedText"))
        .SetFontSize(10)
        .SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f))
    );
    Set("Odyssey.About.SmallText", FTextBlockStyle(FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("SmallText"))
        .SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f))
    );
    Set("Odyssey.About.BigText", FTextBlockStyle(FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
        .SetFontSize(15)
        .SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f))
    );

    Set("Odyssey.About.UnderlineSubduedText", FTextBlockStyle(FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalUnderlinedText"))
        .SetFontSize(10)
        .SetColorAndOpacity(FLinearColor(FColor(128, 128, 128)))
    );
    Set("Odyssey.About.SmallSubduedText", FTextBlockStyle(FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("SmallText"))
        .SetColorAndOpacity(FLinearColor(FColor(128, 128, 128)))
    );
}

/* FSlateOdysseyStyle static initialization
 *****************************************************************************/

TSharedPtr< FOdysseyStyleDefault > FSlateOdysseyStyle::smStyleDefaultInstance = nullptr;

//static
const FSlateStyleSet&
FSlateOdysseyStyle::Get()
{
    return *smStyleDefaultInstance;
}

/*static*/
void
FSlateOdysseyStyle::Initialize()
{
    smStyleDefaultInstance = Create();
    SetStyle( smStyleDefaultInstance.ToSharedRef() );
}

/*static*/
void
FSlateOdysseyStyle::Shutdown()
{
    ResetToDefault();
    ensure( smStyleDefaultInstance.IsUnique() );
    smStyleDefaultInstance.Reset();
}

/*static*/
TSharedRef< class FOdysseyStyleDefault >
FSlateOdysseyStyle::Create()
{
    TSharedRef< class FOdysseyStyleDefault > NewStyle = MakeShareable( new FOdysseyStyleDefault() );
    NewStyle->Initialize();
    return NewStyle;
}

//---

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef DEFAULT_FONT
#undef ICON_FONT
