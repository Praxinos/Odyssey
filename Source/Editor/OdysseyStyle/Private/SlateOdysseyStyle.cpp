// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SlateOdysseyStyle.h"

#include "Misc/CommandLine.h"
#include "Styling/CoreStyle.h"
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
    const FVector2D mIcon32x32;
    const FVector2D mIcon40x40;
    const FVector2D mIcon48x48;
    const FVector2D mIcon64x64;
    const FVector2D mIcon36x24;
    const FVector2D mIcon128x128;

    const TSharedRef< FLinearColor > mDefaultForeground_LinearRef;
    const FSlateColor mDefaultForeground;

    FButtonStyle mButton;

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
    , mIcon32x32( 32.0f, 32.0f )
    , mIcon40x40( 40.0f, 40.0f )
    , mIcon48x48( 48.0f, 48.0f )
    , mIcon64x64( 64.0f, 64.0f )
    , mIcon36x24( 36.0f, 24.0f )
    , mIcon128x128( 128.0f, 128.0f )

    , mDefaultForeground_LinearRef( MakeShareable( new FLinearColor( 0.72f, 0.72f, 0.72f, 1.f ) ) )
    , mDefaultForeground( mDefaultForeground_LinearRef )

    , mButton()

    , mCoreTableRowStyle()
{
}

//---

void
FOdysseyStyleDefault::Initialize()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Iliad" )->GetBaseDir() / TEXT( "Resources" ) / TEXT( "OdysseyAssetResources" ) );
    SetCoreContentRoot( IPluginManager::Get().FindPlugin( "Iliad" )->GetBaseDir() / TEXT( "Resources" ) / TEXT( "OdysseyAssetResources" ) );

    // Avoid polluting the game texture atlas with non-core editor style items when not the editor (or a standalone application, like UFE)
    if( !IncludeOdysseyEditorSpecificStyles() )
        return;

    SetupGeneralStyles();
    SetupClassIconsAndThumbnails();
}

void
FOdysseyStyleDefault::SetupGeneralStyles()
{
    mCoreTableRowStyle = FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");
}

void
FOdysseyStyleDefault::SetupClassIconsAndThumbnails()
{
#if WITH_EDITOR
    // Generic Button styles
    Set( "Button.NoPadding", FButtonStyle()
        .SetNormal ( FSlateNoResource() )
        .SetPressed( FSlateNoResource() )
        .SetHovered( FSlateNoResource() )
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

    // OdysseyTexture
    Set( "ClassIcon.OdysseyTexture", new IMAGE_BRUSH( "OdysseyTexture/OdysseyTextureDefaultThumbnail_16", mIcon16x16 ) );
    Set( "ClassThumbnail.OdysseyTexture", new IMAGE_BRUSH( "OdysseyTexture/OdysseyTextureDefaultThumbnail_64", mIcon64x64 ) );
    Set( "ClassIcon.Texture2D", new IMAGE_BRUSH( "OdysseyTexture/OdysseyTextureDefaultThumbnail_16", mIcon16x16 ) );
    Set( "ClassThumbnail.Texture2D", new IMAGE_BRUSH( "OdysseyTexture/OdysseyTextureDefaultThumbnail_64", mIcon64x64 ) );
    
    Set( "ClassIcon.OdysseyFlipbook", new IMAGE_BRUSH("OdysseyFlipbook/OdysseyFlipbook_16x", mIcon16x16));
    Set( "ClassThumbnail.OdysseyFlipbook", new IMAGE_BRUSH("OdysseyFlipbook/OdysseyFlipbook_64x", mIcon64x64));
    
    Set( "OdysseyTexture.ExportTexture_16", new IMAGE_BRUSH("OdysseyTexture/ExportTexture_16", mIcon16x16));
    Set( "OdysseyTexture.ExportTexture_64", new IMAGE_BRUSH("OdysseyTexture/ExportTexture_64", mIcon64x64));

    // OdysseyLogo
    Set( "OdysseyLogo.Iliad16", new IMAGE_BRUSH("OdysseyTexture/Iliad16", mIcon16x16));
    Set( "OdysseyLogo.PraxinosLogo16", new IMAGE_BRUSH("Logo/Praxinos_Logo_16", mIcon16x16));

    // OdysseyBrush
    Set( "ClassIcon.OdysseyBrush", new IMAGE_BRUSH( "Brush/OdysseyBrushDefaultThumbnail_16", mIcon16x16 ) );
    Set( "ClassThumbnail.OdysseyBrush", new IMAGE_BRUSH( "Brush/OdysseyBrushDefaultThumbnail_64", mIcon64x64 ) );
    Set( "ClassIcon.OdysseyBrushAssetBase", new IMAGE_BRUSH( "Brush/OdysseyBrushDefaultThumbnail_16", mIcon16x16 ) );
    Set( "ClassThumbnail.OdysseyBrushAssetBase", new IMAGE_BRUSH( "Brush/OdysseyBrushDefaultThumbnail_64", mIcon64x64 ) );

    Set( "OdysseyBrush.EditBrush16", new IMAGE_BRUSH( "Brush/edit_brush_16", mIcon16x16 ) );
    Set( "OdysseyBrush.EditBrush64", new IMAGE_BRUSH( "Brush/edit_brush_64", mIcon64x64 ) );

    // OdysseyPalette
    Set("ClassThumbnail.OdysseyPalette", new IMAGE_BRUSH("OdysseyPalette/OdysseyPaletteDefaultThumbnail_64", mIcon64x64));
    Set("OdysseyPalette.EntryColor", new IMAGE_BRUSH("OdysseyPalette/OdysseyPaletteEntryColor_16", mIcon16x16));
    Set("OdysseyPalette.EntryMaterial", new IMAGE_BRUSH("OdysseyPalette/OdysseyPaletteEntryMaterial_16", mIcon16x16));
    Set("OdysseyPalette.PaletteTab", new IMAGE_BRUSH("OdysseyPalette/OdysseyPaletteTab_16", mIcon16x16));


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

    //About
    Set( "About.Facebook", new IMAGE_BRUSH( "About/facebook", FVector2D( 30, 30 ) ) );
    Set( "About.LinkedIn", new IMAGE_BRUSH( "About/linkedin", FVector2D( 30, 30 ) ) );
    Set( "About.Youtube", new IMAGE_BRUSH( "About/youtube", FVector2D( 30, 30 ) ) );
    Set( "About.Twitter", new IMAGE_BRUSH( "About/twitter", FVector2D( 30, 30 ) ) );
    Set( "About.Instagram", new IMAGE_BRUSH( "About/instagram", FVector2D( 30, 30 ) ) );
    Set( "About.Discord", new IMAGE_BRUSH( "About/discord", FVector2D( 30, 30 ) ) );
    Set( "About.UserDoc", new IMAGE_BRUSH( "About/userDoc", FVector2D( 30, 30 ) ) );
    Set( "About.Git", new IMAGE_BRUSH( "About/git", FVector2D( 30, 30 ) ) );
    Set( "About.Praxinos", new IMAGE_BRUSH( "About/praxinos", FVector2D( 128, 128 ) ) );
    Set( "About.Iliad", new IMAGE_BRUSH( "About/iliad", FVector2D( 128, 128 ) ) );
    Set( "About.Manual16", new IMAGE_BRUSH("About/Manual_16", mIcon16x16));
    Set( "About.MorePencils16", new IMAGE_BRUSH("About/More_Pencils_16", mIcon16x16));
    Set( "About.Discord2_16", new IMAGE_BRUSH("About/Discord2_16", mIcon16x16));

    Set( "About.UnderlineText", FTextBlockStyle( FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>( "NormalUnderlinedText" ) )
         .SetFontSize( 10 )
         .SetColorAndOpacity( FLinearColor( 1.f, 1.f, 1.f ) )
    );
    Set( "About.SmallText", FTextBlockStyle( FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>( "SmallText" ) )
         .SetColorAndOpacity( FLinearColor( 1.f, 1.f, 1.f ) )
    );
    Set( "About.BigText", FTextBlockStyle( FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>( "NormalText" ) )
         .SetFontSize( 15 )
         .SetColorAndOpacity( FLinearColor( 1.f, 1.f, 1.f ) )
    );

    Set( "About.UnderlineSubduedText", FTextBlockStyle( FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>( "NormalUnderlinedText" ) )
         .SetFontSize( 10 )
         .SetColorAndOpacity( FLinearColor( FColor( 128, 128, 128 ) ) )
    );
    Set( "About.SmallSubduedText", FTextBlockStyle( FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>( "SmallText" ) )
         .SetColorAndOpacity( FLinearColor( FColor( 128, 128, 128 ) ) )
    );
    Set( "About.BigSubduedText", FTextBlockStyle( FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>( "NormalText" ) )
         .SetFontSize( 15 )
         .SetColorAndOpacity( FLinearColor( FColor( 128, 128, 128 ) ) )
    );

    //IliadPainterEditor Commands
    // Example of how to define icons for Actions / Commands
    /* Set( "IliadPainterEditor.ZoomIn", new IMAGE_BRUSH( "PainterEditor/arrow_down_64", mIcon64x64 ) );
    Set( "IliadPainterEditor.ZoomOut", new IMAGE_BRUSH( "PainterEditor/arrow_up_64", mIcon64x64 ) ); */

    //PainterEditor
    Set( "PainterEditor.OpenPaintEditor64", new IMAGE_BRUSH( "PainterEditor/open_paint_editor_64", mIcon64x64 ) );
    Set( "PainterEditor.OpenPaintEditor16", new IMAGE_BRUSH( "PainterEditor/open_paint_editor_16", mIcon16x16 ) );

    Set( "PainterEditor.Notes64", new IMAGE_BRUSH( "PainterEditor/notes_64", mIcon64x64 ) );
    Set( "PainterEditor.Notes16", new IMAGE_BRUSH( "PainterEditor/notes_16", mIcon16x16 ) );

    Set( "PainterEditor.Spark32", new IMAGE_BRUSH( "PainterEditor/spark_32", mIcon32x32 ) );
    Set( "PainterEditor.Spark20", new IMAGE_BRUSH( "PainterEditor/spark_20", mIcon20x20 ) );
    Set( "PainterEditor.Spark16", new IMAGE_BRUSH( "PainterEditor/spark_16", mIcon16x16 ) );

    Set( "PainterEditor.Tools64", new IMAGE_BRUSH( "PainterEditor/tools_64", mIcon64x64 ) );
    Set( "PainterEditor.Tools20", new IMAGE_BRUSH( "PainterEditor/tools_20", mIcon20x20 ) );
    Set( "PainterEditor.Tools16", new IMAGE_BRUSH( "PainterEditor/tools_16", mIcon16x16 ) );

    Set( "PainterEditor.Layers64", new IMAGE_BRUSH( "PainterEditor/layers_64", mIcon64x64 ) );
    Set( "PainterEditor.Layers20", new IMAGE_BRUSH( "PainterEditor/layers_20", mIcon20x20 ) );
    Set( "PainterEditor.Layers16", new IMAGE_BRUSH( "PainterEditor/layers_16", mIcon16x16 ) );

    Set( "PainterEditor.ColorSliders64", new IMAGE_BRUSH( "PainterEditor/color_sliders_64", mIcon64x64 ) );
    Set( "PainterEditor.ColorSliders16", new IMAGE_BRUSH( "PainterEditor/color_sliders_16", mIcon16x16 ) );
    Set( "PainterEditor.ColorSliders_2_64", new IMAGE_BRUSH( "PainterEditor/color_sliders_2_64", mIcon64x64 ) );
    Set( "PainterEditor.ColorSliders_2_20", new IMAGE_BRUSH( "PainterEditor/color_sliders_2_20", mIcon20x20 ) );
    Set( "PainterEditor.ColorSliders_2_16", new IMAGE_BRUSH( "PainterEditor/color_sliders_2_16", mIcon16x16 ) );

    Set( "PainterEditor.ColorSelector64", new IMAGE_BRUSH( "PainterEditor/color_selector_64", mIcon64x64 ) );
    Set( "PainterEditor.ColorSelector16", new IMAGE_BRUSH( "PainterEditor/color_selector_16", mIcon16x16 ) );
    Set( "PainterEditor.ColorSelectorBis64", new IMAGE_BRUSH( "PainterEditor/color_selector_bis_64", mIcon64x64 ) );
    Set( "PainterEditor.ColorSelectorBis16", new IMAGE_BRUSH( "PainterEditor/color_selector_bis_16", mIcon16x16 ) );

    Set( "PainterEditor.StrokeOptions64", new IMAGE_BRUSH( "PainterEditor/tool_options_64", mIcon64x64 ) );
    Set( "PainterEditor.StrokeOptions20", new IMAGE_BRUSH( "PainterEditor/tool_options_64", mIcon20x20 ) );
    Set( "PainterEditor.StrokeOptions16", new IMAGE_BRUSH( "PainterEditor/tool_options_16", mIcon16x16 ) );

    Set( "PainterEditor.BrushExposedParameters64", new IMAGE_BRUSH( "PainterEditor/brush_exposed_parameters_64", mIcon64x64 ) );
    Set( "PainterEditor.BrushExposedParameters20", new IMAGE_BRUSH( "PainterEditor/brush_exposed_parameters_20", mIcon20x20 ) );
    Set( "PainterEditor.BrushExposedParameters16", new IMAGE_BRUSH( "PainterEditor/brush_exposed_parameters_16", mIcon16x16 ) );

    Set( "PainterEditor.BrushSelector64", new IMAGE_BRUSH( "PainterEditor/brush_selector_64", mIcon64x64 ) );
    Set( "PainterEditor.BrushSelector20", new IMAGE_BRUSH( "PainterEditor/brush_selector_20", mIcon20x20 ) );
    Set( "PainterEditor.BrushSelector16", new IMAGE_BRUSH( "PainterEditor/brush_selector_16", mIcon16x16 ) );

    Set( "PainterEditor.Navigator64", new IMAGE_BRUSH( "PainterEditor/navigator_64", mIcon64x64 ) );
    Set( "PainterEditor.Navigator16", new IMAGE_BRUSH( "PainterEditor/navigator_16", mIcon16x16 ) );

    Set( "PainterEditor.BrushPreview64", new IMAGE_BRUSH( "PainterEditor/brush_preview_64", mIcon64x64 ) );
    Set( "PainterEditor.BrushPreview20", new IMAGE_BRUSH( "PainterEditor/brush_preview_20", mIcon20x20 ) );
    Set( "PainterEditor.BrushPreview16", new IMAGE_BRUSH( "PainterEditor/brush_preview_16", mIcon16x16 ) );

    Set( "PainterEditor.PerformanceTools64", new IMAGE_BRUSH( "PainterEditor/performance_tools_64", mIcon64x64 ) );
    Set( "PainterEditor.PerformanceTools16", new IMAGE_BRUSH( "PainterEditor/performance_tools_16", mIcon16x16 ) );

    Set( "PainterEditor.TopBar64", new IMAGE_BRUSH( "PainterEditor/top_bar_64", mIcon64x64 ) );
    Set( "PainterEditor.TopBar20", new IMAGE_BRUSH( "PainterEditor/top_bar_20", mIcon20x20 ) );
    Set( "PainterEditor.TopBar16", new IMAGE_BRUSH( "PainterEditor/top_bar_16", mIcon16x16 ) );

    Set( "PainterEditor.BotBar64", new IMAGE_BRUSH( "PainterEditor/bot_bar_64", mIcon64x64 ) );
    Set( "PainterEditor.BotBar16", new IMAGE_BRUSH( "PainterEditor/bot_bar_16", mIcon16x16 ) );

    Set( "PainterEditor.Trash64", new IMAGE_BRUSH( "PainterEditor/trash_64", mIcon64x64 ) );
    Set( "PainterEditor.Trash16", new IMAGE_BRUSH( "PainterEditor/trash_16", mIcon16x16 ) );

    Set( "PainterEditor.Trash_2_64", new IMAGE_BRUSH( "PainterEditor/trash_2_64", mIcon64x64 ) );
    Set( "PainterEditor.Trash_2_16", new IMAGE_BRUSH( "PainterEditor/trash_2_16", mIcon16x16 ) );

    Set( "PainterEditor.Viewport64", new IMAGE_BRUSH( "PainterEditor/viewport_64", mIcon64x64 ) );
    Set( "PainterEditor.Viewport20", new IMAGE_BRUSH( "PainterEditor/viewport_20", mIcon20x20 ) );
    Set( "PainterEditor.Viewport16", new IMAGE_BRUSH( "PainterEditor/viewport_16", mIcon16x16 ) );

    Set( "PainterEditor.Draw64", new IMAGE_BRUSH( "PainterEditor/draw_64", mIcon64x64 ) );
    Set( "PainterEditor.Draw16", new IMAGE_BRUSH( "PainterEditor/draw_16", mIcon16x16 ) );

    Set( "PainterEditor.Mesh64", new IMAGE_BRUSH( "PainterEditor/mesh_64", mIcon64x64 ) );
    Set( "PainterEditor.Mesh20", new IMAGE_BRUSH( "PainterEditor/mesh_20", mIcon20x20 ) );
    Set( "PainterEditor.Mesh16", new IMAGE_BRUSH( "PainterEditor/mesh_16", mIcon16x16 ) );

    Set( "PainterEditor.Pan64", new IMAGE_BRUSH( "PainterEditor/pan_64", mIcon64x64 ) );
    Set( "PainterEditor.Pan16", new IMAGE_BRUSH( "PainterEditor/pan_16", mIcon16x16 ) );

    Set( "PainterEditor.Rotate64", new IMAGE_BRUSH( "PainterEditor/rotate_64", mIcon64x64 ) );
    Set( "PainterEditor.Rotate16", new IMAGE_BRUSH( "PainterEditor/rotate_16", mIcon16x16 ) );
    Set( "PainterEditor.Rotate_2_64", new IMAGE_BRUSH( "PainterEditor/rotate_2_64", mIcon64x64 ) );
    Set( "PainterEditor.Rotate_2_16", new IMAGE_BRUSH( "PainterEditor/rotate_2_16", mIcon16x16 ) );

    Set( "PainterEditor.RotateLeft64", new IMAGE_BRUSH( "PainterEditor/rotate_left_64", mIcon64x64 ) );
    Set( "PainterEditor.RotateLeft16", new IMAGE_BRUSH( "PainterEditor/rotate_left_16", mIcon16x16 ) );
    Set( "PainterEditor.RotateRight64", new IMAGE_BRUSH( "PainterEditor/rotate_right_64", mIcon64x64 ) );
    Set( "PainterEditor.RotateRight16", new IMAGE_BRUSH( "PainterEditor/rotate_right_16", mIcon16x16 ) );

    Set( "PainterEditor.ArrowDown64", new IMAGE_BRUSH( "PainterEditor/arrow_down_64", mIcon64x64 ) );
    Set( "PainterEditor.ArrowDown16", new IMAGE_BRUSH( "PainterEditor/arrow_down_16", mIcon16x16 ) );
    Set( "PainterEditor.ArrowUp64", new IMAGE_BRUSH( "PainterEditor/arrow_up_64", mIcon64x64 ) );
    Set( "PainterEditor.ArrowUp16", new IMAGE_BRUSH( "PainterEditor/arrow_up_16", mIcon16x16 ) );
    Set( "PainterEditor.ArrowLeft64", new IMAGE_BRUSH( "PainterEditor/arrow_left_64", mIcon64x64 ) );
    Set( "PainterEditor.ArrowLeft16", new IMAGE_BRUSH( "PainterEditor/arrow_left_16", mIcon16x16 ) );
    Set( "PainterEditor.ArrowRight64", new IMAGE_BRUSH( "PainterEditor/arrow_right_64", mIcon64x64 ) );
    Set( "PainterEditor.ArrowRight16", new IMAGE_BRUSH( "PainterEditor/arrow_right_16", mIcon16x16 ) );

    Set( "PainterEditor.ColorWheel64", new IMAGE_BRUSH( "PainterEditor/color_wheel_64", mIcon64x64 ) );
    Set( "PainterEditor.ColorWheel20", new IMAGE_BRUSH( "PainterEditor/color_wheel_20", mIcon20x20 ) );
    Set( "PainterEditor.ColorWheel16", new IMAGE_BRUSH( "PainterEditor/color_wheel_16", mIcon16x16 ) );

    Set( "PainterEditor.Trombone64", new IMAGE_BRUSH( "PainterEditor/trombone_64", mIcon64x64 ) );
    Set( "PainterEditor.Trombone20", new IMAGE_BRUSH( "PainterEditor/trombone_20", mIcon20x20 ) );
    Set( "PainterEditor.Trombone16", new IMAGE_BRUSH( "PainterEditor/trombone_16", mIcon16x16 ) );

    Set( "PainterEditor.UndoHistory64", new IMAGE_BRUSH( "PainterEditor/undo_history_64", mIcon64x64 ) );
    Set( "PainterEditor.UndoHistory16", new IMAGE_BRUSH( "PainterEditor/undo_history_16", mIcon16x16 ) );

    Set( "PainterEditor.RotateReset64", new IMAGE_BRUSH( "PainterEditor/rotate_reset_64", mIcon64x64 ) );
    Set( "PainterEditor.RotateReset16", new IMAGE_BRUSH( "PainterEditor/rotate_reset_16", mIcon16x16 ) );

    //PainterEditor - Top Bar
    Set( "PainterEditor.TopBar.Undo32", new IMAGE_BRUSH( "PainterEditor/undo_32", mIcon32x32 ) );
    Set( "PainterEditor.TopBar.Redo32", new IMAGE_BRUSH( "PainterEditor/redo_32", mIcon32x32 ) );
    Set( "PainterEditor.TopBar.Save32", new IMAGE_BRUSH( "PainterEditor/save_32", mIcon32x32 ) );
    Set( "PainterEditor.TopBar.Eraser32", new IMAGE_BRUSH( "PainterEditor/eraser_32", mIcon32x32 ) );
    Set( "PainterEditor.TopBar.Clear32", new IMAGE_BRUSH( "PainterEditor/clear_32", mIcon32x32 ) );
    Set( "PainterEditor.TopBar.VectorModeVertex32",new IMAGE_BRUSH("PainterEditor/vector_mode_vertex_32",mIcon32x32));
    Set( "PainterEditor.TopBar.VectorModeObject32",new IMAGE_BRUSH("PainterEditor/vector_mode_object_32",mIcon32x32));

    //PainterEditor - ToolsTab
    Set( "PainterEditor.ToolsTab.ObjectPickTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/object_pick_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.ObjectMoveTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/object_move_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.ObjectRotateTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/object_rotate_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.ObjectScaleTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/object_scale_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.TransformTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/transform_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.TransformTool32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/transform_tool_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.TransformTool16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/transform_tool_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.SceneScaleTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/scene_scale_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.ScenePanTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/scene_pan_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.SceneScaleTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/scene_scale_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PathCutTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/path_cut_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PathPushTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/path_push_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PathWidthTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/path_width_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PathSmoothTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/path_smooth_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PathKnotTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/path_knot_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Lasso64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/lasso_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Lasso32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/lasso_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.Lasso16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/lasso_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.Grid64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/grid_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.VectoEdit64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/vecto_edit_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.VectoEdit32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/vecto_edit_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.VectoEdit16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/vecto_edit_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.VectoPen64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/vecto_pen_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.VectoPen32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/vecto_pen_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.VectoPen16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/vecto_pen_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.DrawingTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/drawing_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.DrawingTool16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/drawing_tool_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.ClearLayer64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/clear_layer_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.ClearLayer32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/clear_layer_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.ClearLayer16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/clear_layer_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.ColorPickerTool64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/color_picker_tool_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PaintBucket64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/paint_bucket_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.PaintBucket32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/paint_bucket_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.PaintBucket16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/paint_bucket_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.Shredder64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/shredder_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Shredder32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/shredder_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.Shredder16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/shredder_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.Eraser64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/eraser_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Ellipse64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/ellipse_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Ellipse32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/ellipse_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.Ellipse16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/ellipse_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.Circle64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/circle_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Circle32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/circle_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.Circle16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/circle_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.Square64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/square_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Square32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/square_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.Square16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/square_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.Line64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/line_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Line32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/line_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.Line16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/line_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.Bezier64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/bezier_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Bezier32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/bezier_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.Bezier16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/bezier_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.Curve64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/curve_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.Curve32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/curve_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.Curve16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/curve_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.ColorPicker64", new IMAGE_BRUSH( "PainterEditor/ToolsTab/color_picker_64", mIcon64x64 ) );
    Set( "PainterEditor.ToolsTab.ColorPicker32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/color_picker_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.ColorPicker16", new IMAGE_BRUSH( "PainterEditor/ToolsTab/color_picker_16", mIcon16x16 ) );
    Set( "PainterEditor.ToolsTab.Polygon32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/polygon_32", mIcon32x32 ) );
    Set( "PainterEditor.ToolsTab.FreeHand32", new IMAGE_BRUSH( "PainterEditor/ToolsTab/freehand_32", mIcon32x32 ) );

    //OdysseyPainterEditorTools
    Set( "OdysseyPainterEditorTools.Tile", mCoreTableRowStyle
            
    );

    //OdysseyLayerStack
    Set( "OdysseyLayerStack.ImageLayer64", new IMAGE_BRUSH( "OdysseyLayerStack/image_layer_64", mIcon64x64 ) );
    Set( "OdysseyLayerStack.ImageLayer16", new IMAGE_BRUSH( "OdysseyLayerStack/image_layer_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.LayerBitmap64", new IMAGE_BRUSH( "OdysseyLayerStack/layer_bitmap_64", mIcon64x64 ) );
    Set( "OdysseyLayerStack.LayerBitmap16", new IMAGE_BRUSH( "OdysseyLayerStack/layer_bitmap_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.LayerVector64", new IMAGE_BRUSH( "OdysseyLayerStack/layer_vector_64", mIcon64x64 ) );
    Set( "OdysseyLayerStack.LayerVector16", new IMAGE_BRUSH( "OdysseyLayerStack/layer_vector_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.NotVisible64", new IMAGE_BRUSH( "OdysseyLayerStack/not_visible_64", mIcon64x64 ) );
    Set( "OdysseyLayerStack.NotVisible16", new IMAGE_BRUSH( "OdysseyLayerStack/not_visible_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.Visible64", new IMAGE_BRUSH( "OdysseyLayerStack/visible_64", mIcon64x64 ) );
    Set( "OdysseyLayerStack.Visible16", new IMAGE_BRUSH( "OdysseyLayerStack/visible_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.Locked64", new IMAGE_BRUSH( "OdysseyLayerStack/locked_64", mIcon64x64 ) );
    Set( "OdysseyLayerStack.Locked16", new IMAGE_BRUSH( "OdysseyLayerStack/locked_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.Unlocked64", new IMAGE_BRUSH( "OdysseyLayerStack/unlocked_64", mIcon64x64 ) );
    Set( "OdysseyLayerStack.Unlocked16", new IMAGE_BRUSH( "OdysseyLayerStack/unlocked_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.OptionsHeader64", new IMAGE_BRUSH( "OdysseyLayerStack/options_header_64", mIcon64x64 ) );
    Set( "OdysseyLayerStack.OptionsHeader16", new IMAGE_BRUSH( "OdysseyLayerStack/options_header_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.OptionsDisplayed64", new IMAGE_BRUSH( "OdysseyLayerStack/options_displayed_64", mIcon64x64 ) );
    Set( "OdysseyLayerStack.OptionsDisplayed16", new IMAGE_BRUSH( "OdysseyLayerStack/options_displayed_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.OptionsHidden64", new IMAGE_BRUSH( "OdysseyLayerStack/options_hidden_64", mIcon64x64 ) );
    Set( "OdysseyLayerStack.OptionsHidden16", new IMAGE_BRUSH( "OdysseyLayerStack/options_hidden_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.Blend64", new IMAGE_BRUSH( "OdysseyLayerStack/blend_64", mIcon64x64 ) );
    Set( "OdysseyLayerStack.Blend16", new IMAGE_BRUSH( "OdysseyLayerStack/blend_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.Blend_2_64", new IMAGE_BRUSH( "OdysseyLayerStack/blend_2_64", mIcon64x64 ) );
    Set( "OdysseyLayerStack.Blend_2_16", new IMAGE_BRUSH( "OdysseyLayerStack/blend_2_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.Merge64", new IMAGE_BRUSH( "OdysseyLayerStack/merge_64", mIcon64x64 ) );
    Set( "OdysseyLayerStack.Merge16", new IMAGE_BRUSH( "OdysseyLayerStack/merge_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.AlphaLocked16", new IMAGE_BRUSH( "OdysseyLayerStack/alpha_locked_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.AlphaUnlocked16", new IMAGE_BRUSH( "OdysseyLayerStack/alpha_unlocked_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.Colored16", new IMAGE_BRUSH( "OdysseyLayerStack/colored_16", mIcon16x16 ) );
    Set( "OdysseyLayerStack.Uncolored16", new IMAGE_BRUSH( "OdysseyLayerStack/uncolored_16", mIcon16x16 ) );

    Set("LayerStack.IsCollapsedToggle", FCheckBoxStyle()
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH("OdysseyLayerStack/options_hidden_16", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/options_hidden_16", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/options_hidden_16", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH("OdysseyLayerStack/options_displayed_16", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/options_displayed_16", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/options_displayed_16", mIcon16x16))
	);

    Set("LayerStack.IsActivatedToggle", FCheckBoxStyle()
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH("OdysseyLayerStack/not_visible_16", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/not_visible_16", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/not_visible_16", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH("OdysseyLayerStack/visible_16", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/visible_16", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/visible_16", mIcon16x16))
	);

    Set("LayerStack.IsLockedToggle", FCheckBoxStyle()
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH("OdysseyLayerStack/unlocked_16", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/unlocked_16", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/unlocked_16", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH("OdysseyLayerStack/locked_16", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/locked_16", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/locked_16", mIcon16x16))
	);
    
    //OdysseyViewportDrawingEditor
    Set("OdysseyViewportDrawingEditMode.SetOdysseyBrushSettingsView", new IMAGE_BRUSH("PainterEditor/brush_selector_16", mIcon16x16 ));
	Set("OdysseyViewportDrawingEditMode.SetOdysseyLayerStackView", new IMAGE_BRUSH("PainterEditor/layers_16", mIcon16x16 ));
	Set("OdysseyViewportDrawingEditMode.SetOdysseyPainterEditorToolOptionsView", new IMAGE_BRUSH("PainterEditor/tool_options_16", mIcon16x16 ));
	Set("OdysseyViewportDrawingEditMode.SetOdysseyPainterEditorToolsView", new IMAGE_BRUSH("PainterEditor/tools_16", mIcon16x16 ));

    Set("OdysseyViewportDrawingEditMode.OdysseyViewportDrawingIcon16",new IMAGE_BRUSH("OdysseyViewportDrawing/viewport_drawing_icon_16",mIcon16x16));
    Set("OdysseyViewportDrawingEditMode.OdysseyViewportDrawingIcon40",new IMAGE_BRUSH("OdysseyViewportDrawing/viewport_drawing_icon_40",mIcon40x40));

    //LayerStack
    {
        FSlateColor selectedRow(FStyleColors::Select.GetSpecifiedColor().CopyWithNewOpacity(0.3f));
        FSlateColor selectedInactiveRow(FStyleColors::SelectInactive.GetSpecifiedColor().CopyWithNewOpacity(0.3f));
        FSlateColor selectedParentRow(FStyleColors::SelectParent.GetSpecifiedColor().CopyWithNewOpacity(0.3f));
        Set ("OdysseyLayerStack.CurrentLayerBackgroundBrush", new FSlateColorBrush(FStyleColors::Select));
        Set ("OdysseyLayerStack.CurrentLayerInactiveBackgroundBrush", new FSlateColorBrush(FStyleColors::SelectInactive));
        Set( "OdysseyLayerStack.AlternatedRows", mCoreTableRowStyle
                .SetOddRowBackgroundBrush(FSlateColorBrush(FStyleColors::Header))
                .SetSelectorFocusedBrush(FSlateNoResource())
                .SetActiveBrush(FSlateColorBrush(selectedRow))
                .SetActiveHoveredBrush(FSlateColorBrush(selectedRow))
                .SetInactiveBrush(FSlateColorBrush(selectedInactiveRow))
                .SetInactiveHoveredBrush(FSlateColorBrush(selectedInactiveRow))
                .SetActiveHighlightedBrush(FSlateColorBrush(selectedParentRow)) // This is the parent hightlight
                .SetInactiveHighlightedBrush(FSlateColorBrush(selectedParentRow))// This is the parent highlight
        );
    }
    
    

    Set("OdysseySpinBoxStyle.DarkSpinBox",FSpinBoxStyle()
        .SetBackgroundBrush(BOX_BRUSH("SpinBox/Spinbox",FMargin(4.0f/16.0f), FLinearColor( 0.05f, 0.05f, 0.05f )))
        .SetInactiveFillBrush(BOX_BRUSH("SpinBox/Spinbox_Fill",FMargin(4.0f/16.0f,4.0f/16.0f,8.0f/16.0f,4.0f/16.0f),FLinearColor(0.1f,0.1f,0.1f)))
        .SetHoveredBackgroundBrush(BOX_BRUSH("SpinBox/Spinbox_Hovered", FMargin(4.0f/16.0f), FLinearColor( 0.05f, 0.05f, 0.05f )))
        .SetActiveFillBrush(BOX_BRUSH("SpinBox/Spinbox_Fill_Hovered",FMargin(4.0f/16.0f), FLinearColor( 0.25f, 0.25f, 0.25f )))
        .SetArrowsImage(IMAGE_BRUSH("SpinBox/SpinArrows",mIcon12x12))
    );

    Set("OdysseySpinBoxStyle.TransparentSpinBox",FSpinBoxStyle()
        .SetBackgroundBrush(FSlateNoResource())
        .SetHoveredBackgroundBrush(FSlateNoResource())
        .SetActiveFillBrush(FSlateNoResource())
        .SetInactiveFillBrush(FSlateNoResource())
        .SetArrowsImage(FSlateNoResource())
    );

    Set("OdysseyCheckBoxStyle.TransparentCheckBox", FCheckBoxStyle()
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(FSlateNoResource())
        .SetUncheckedHoveredImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, FLinearColor(0.728f, 0.364f, 0.003f)))
        .SetUncheckedPressedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, FLinearColor(0.701f, 0.225f, 0.003f)))
        .SetCheckedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, FLinearColor(0.701f, 0.225f, 0.003f)))
        .SetCheckedHoveredImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, FLinearColor(0.728f, 0.364f, 0.003f)))
        .SetCheckedPressedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, FLinearColor(0.701f, 0.225f, 0.003f)))
    );

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
    Set("TimelineHeader.backgroundColorEven", FLinearColor(1.0f, 1.0f, 1.0f, 0.05f));
    Set("TimelineHeader.backgroundColorOdd", FLinearColor(0.0f, 0.0f, 0.0f, 0.05f));
    Set("TimelineHeader.ProxyDoneColor", FLinearColor(0.0f, 1.0f, 0.0f, 0.5f));
    Set("TimelineHeader.ProxyPendingColor", FLinearColor(1.0f, 0.6f, 0.0f, 0.5f));

    Set( "ClassIcon.OdysseyAnimation", new IMAGE_BRUSH( "OdysseyAnimation/OdysseyAnimationDefaultThumbnail_16", mIcon16x16 ) );
    Set( "ClassThumbnail.OdysseyAnimation", new IMAGE_BRUSH( "OdysseyAnimation/OdysseyAnimationDefaultThumbnail_64", mIcon64x64 ) );

    Set("Animation.Timeline.Tools.Selection", new IMAGE_BRUSH_SVG("OdysseyAnimation/Timeline/Tools/Selection", mIcon16x16));
    Set("Animation.Timeline.Tools.Move", new IMAGE_BRUSH_SVG("OdysseyAnimation/Timeline/Tools/Move", mIcon16x16));
    Set("Animation.Timeline.Tools.Cut", new IMAGE_BRUSH_SVG("OdysseyAnimation/Timeline/Tools/Cut", mIcon16x16));
    
    Set("Animation.LightTable64", new IMAGE_BRUSH("OdysseyAnimation/LightTable_64", mIcon64x64));
    Set("Animation.LightTable24", new IMAGE_BRUSH("OdysseyAnimation/LightTable_24", mIcon24x24));
    Set("Animation.LightTable16", new IMAGE_BRUSH("OdysseyAnimation/LightTable_16", mIcon16x16));

    Set("Animation.LightTableToggle", FCheckBoxStyle()
        //.Padding(FMargin(0,0,0,0))
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH("OdysseyAnimation/LightTableToggle_OFF_16", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH("OdysseyAnimation/LightTableToggle_OFF_16", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH("OdysseyAnimation/LightTableToggle_OFF_16", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH("OdysseyAnimation/LightTableToggle_ON_16", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH("OdysseyAnimation/LightTableToggle_ON_16", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH("OdysseyAnimation/LightTableToggle_ON_16", mIcon16x16))
	);

    Set("Animation.AlphaLockedToggle", FCheckBoxStyle()
        //.Padding(FMargin(0,0,0,0))
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH("OdysseyLayerStack/alpha_unlocked_16", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/alpha_unlocked_16", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/alpha_unlocked_16", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH("OdysseyLayerStack/alpha_locked_16", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/alpha_locked_16", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/alpha_locked_16", mIcon16x16))
	);

    Set("Animation.AddCellsHandleRight", new IMAGE_BRUSH("OdysseyAnimation/AddCellsHandleRight_16", mIcon16x16));
    Set("Animation.AddCellsHandleLeft", new IMAGE_BRUSH("OdysseyAnimation/AddCellsHandleLeft_16", mIcon16x16));
    Set("Animation.CellTimingHandle", new IMAGE_BRUSH("OdysseyAnimation/CellTimingHandle_16", mIcon16x16));
    Set("Animation.CellLengthHandle", new IMAGE_BRUSH("OdysseyAnimation/CellLengthHandle_16", mIcon16x16));
    Set("Animation.CellBreakIndicator", new IMAGE_BRUSH("OdysseyAnimation/CellBreakIndicator", FVector2D(1, 2), FLinearColor::White, ESlateBrushTileType::Vertical));

    Set("Animation.LayerImage.BehaviourColor", FLinearColor(1.f, 1.f, 1.f, 1.f));
    Set("Animation.LayerImage.Behaviour.None", new IMAGE_BRUSH("OdysseyAnimation/LayerImage/BehaviourNone", mIcon16x16));
    Set("Animation.LayerImage.Behaviour.Hold", new IMAGE_BRUSH("OdysseyAnimation/LayerImage/BehaviourHold", mIcon16x16));
    Set("Animation.LayerImage.Behaviour.Loop", new IMAGE_BRUSH("OdysseyAnimation/LayerImage/BehaviourLoop", mIcon16x16));
    Set("Animation.LayerImage.Behaviour.PingPong", new IMAGE_BRUSH("OdysseyAnimation/LayerImage/BehaviourPingPong", mIcon16x16));


    Set("Animation.CellImageStagger.PreviewReachColor", FLinearColor(0.2f, 0.3f, 1.0f, 0.5f));
    Set("Animation.CellImageStagger.ArrowPreviewReach", new IMAGE_BRUSH("OdysseyAnimation/CellImageStaggerArrowPreviewReach", mIcon32x32, FLinearColor::White, ESlateBrushTileType::Horizontal));
    Set("Animation.CellImageStagger.ArrowsColor", FLinearColor(1.f, 1.f, 1.f, 1.f));
    Set("Animation.CellImageStagger.ArrowTop", new IMAGE_BRUSH("OdysseyAnimation/CellImageStaggerArrowTop", FVector2D(10, 7), FLinearColor::White, ESlateBrushTileType::Horizontal));
    Set("Animation.CellImageStagger.ArrowBottom", new IMAGE_BRUSH("OdysseyAnimation/CellImageStaggerArrowBottom", FVector2D(10, 7), FLinearColor::White, ESlateBrushTileType::Horizontal));
    Set("Animation.CellImageStagger.StaggerLengthColor", FLinearColor(1.f, 1.f, 1.f, 1.f));
    Set("Animation.CellImageStagger.BehaviourColor", FLinearColor(1.f, 1.f, 1.f, 1.f));
    Set("Animation.CellImageStagger.Behaviour.Hold", new IMAGE_BRUSH("OdysseyAnimation/CellImageStaggerBehaviourHold", mIcon16x16));
    Set("Animation.CellImageStagger.Behaviour.Loop", new IMAGE_BRUSH("OdysseyAnimation/CellImageStaggerBehaviourLoop", mIcon16x16));
    Set("Animation.CellImageStagger.Behaviour.PingPong", new IMAGE_BRUSH("OdysseyAnimation/CellImageStaggerBehaviourPingPong", mIcon16x16));

    FSlateColor primary( FStyleColors::Primary );
    FSlateColor background( FStyleColors::Background );
    FSlateColor input( FStyleColors::Input );
    FSlateColor inputOutline( FStyleColors::InputOutline );
    Set("Animation.LightTableKey.Back", new FSlateRoundedBoxBrush( primary, FVector4( 8.0f, 8.0f, 8.0f, 8.0f ) ));
    Set("Animation.LightTableKey.Front", new FSlateRoundedBoxBrush( background, FVector4( 8.0f, 8.0f, 8.0f, 8.0f ) ));
    Set("Animation.LightTableKey.BackColor", primary);
    Set("Animation.LightTableKey.FrontColor", background);
    
    //Animation Command Icons
    //Icons are used automatically
    //Just use the prefix "OdysseyAnimationEditorCommands.[CommandName]"
    {
        Set("OdysseyAnimationEditorCommands.SetStaggerCellBehaviourHold", new IMAGE_BRUSH("OdysseyAnimation/CellImageStaggerBehaviourHold", mIcon16x16));
        Set("OdysseyAnimationEditorCommands.SetStaggerCellBehaviourLoop", new IMAGE_BRUSH("OdysseyAnimation/CellImageStaggerBehaviourLoop", mIcon16x16));
        Set("OdysseyAnimationEditorCommands.SetStaggerCellBehaviourPingPong", new IMAGE_BRUSH("OdysseyAnimation/CellImageStaggerBehaviourPingPong", mIcon16x16));
    }
    

    //Texture
    Set("Texture.AlphaLockedToggle", FCheckBoxStyle()
        //.Padding(FMargin(0,0,0,0))
        .SetPadding(FMargin(8)) //8 because left+right = 16 and top+bottom = 16
        .SetForegroundColor(FLinearColor(0, 0, 0, 0))
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetUncheckedImage(IMAGE_BRUSH("OdysseyLayerStack/alpha_unlocked_16", mIcon16x16))
        .SetUncheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/alpha_unlocked_16", mIcon16x16))
        .SetUncheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/alpha_unlocked_16", mIcon16x16))
        .SetCheckedImage(IMAGE_BRUSH("OdysseyLayerStack/alpha_locked_16", mIcon16x16))
        .SetCheckedHoveredImage(IMAGE_BRUSH("OdysseyLayerStack/alpha_locked_16", mIcon16x16))
        .SetCheckedPressedImage(IMAGE_BRUSH("OdysseyLayerStack/alpha_locked_16", mIcon16x16))
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
#endif
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
