// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Styles/EposSequenceEditorStyle.h"

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

TSharedPtr<FEposSequenceEditorStyle> FEposSequenceEditorStyle::smSingleton;

//---

//static
void
FEposSequenceEditorStyle::Register()
{
    FSlateStyleRegistry::RegisterSlateStyle( Get() );
}

//static
void
FEposSequenceEditorStyle::Unregister()
{
    FSlateStyleRegistry::UnRegisterSlateStyle( Get() );
}

//---

FEposSequenceEditorStyle::FEposSequenceEditorStyle()
    : FSlateStyleSet( "EposSequenceEditorStyle" )
{
    SetParentStyleName( FAppStyle::Get().GetStyleSetName() );

    InitSequencer();
    InitCommands();
    InitAbout();
    InitViewport();
    InitImportExport();
}

void
FEposSequenceEditorStyle::InitSequencer()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    // tab icons
    Set( "EposSequenceEditor.Tabs.Sequencer", new IMAGE_BRUSH_SVG( "sequencer-tab", Icon16x16 ) ); // Seems to be only in toolkit standalone mode, but the plugin seems to always be in world-centric mode

    // asset thumbnail
    Set( "ClassIcon.BoardSequence", new IMAGE_BRUSH_SVG( "sequence-board", Icon16x16 ) );
    Set( "ClassThumbnail.BoardSequence", new IMAGE_BRUSH_SVG( "sequence-board", Icon64x64 ) );
    Set( "ClassIcon.ShotSequence", new IMAGE_BRUSH_SVG( "sequence-shot", Icon16x16 ) );
    Set( "ClassThumbnail.ShotSequence", new IMAGE_BRUSH_SVG( "sequence-shot", Icon64x64 ) );
    Set( "ClassIcon.StoryNote", new IMAGE_BRUSH_SVG( "storynote", Icon16x16 ) );
    Set( "ClassThumbnail.StoryNote", new IMAGE_BRUSH_SVG( "storynote", Icon64x64 ) );

    //---

    Set( "Sequencer.InfoBar",
         FTextBlockStyle( FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>( "NormalText" ) )
         .SetFont( DEFAULT_FONT( "Regular", 10 ) )
         .SetColorAndOpacity( FLinearColor( FColor( 96, 96, 96 ) ) )
    );

    Set( "Settings", new IMAGE_BRUSH_SVG( "epos-settings", Icon20x20 ) );
    Set( "Settings.Small", new IMAGE_BRUSH_SVG( "epos-settings", Icon20x20 ) );

    Set( "Help", new IMAGE_BRUSH_SVG( "epos-help", Icon20x20 ) );
    Set( "Help.Small", new IMAGE_BRUSH_SVG( "epos-help", Icon20x20 ) );
}

void
FEposSequenceEditorStyle::InitCommands()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    Set( "EposSequenceCommands.NewStoryboardWithSettings", new IMAGE_BRUSH_SVG( "new-storyboard-with-settings", Icon20x20 ) );
    Set( "EposSequenceCommands.NewStoryboardWithSettings.Small", new IMAGE_BRUSH_SVG( "new-storyboard-with-settings", Icon20x20 ) );
    Set( "EposSequenceCommands.NewStoryboardImportImageSequence", new IMAGE_BRUSH_SVG( "new-storyboard-import-image-sequence", Icon20x20 ) );
    Set( "EposSequenceCommands.NewStoryboardImportImageSequence.Small", new IMAGE_BRUSH_SVG( "new-storyboard-import-image-sequence", Icon20x20 ) );

    Set( "EposSequenceCommands.ToggleStoryboardViewportCommand", new IMAGE_BRUSH_SVG( "toggle-storyboard-viewport-type", Icon20x20 ) );

    Set( "EposSequenceCommands.CreateCameraAtCurrentTime", new IMAGE_BRUSH_SVG( "camera-create", Icon20x20 ) );
    Set( "EposSequenceCommands.CreateCameraAtCurrentTime.Small", new IMAGE_BRUSH_SVG( "camera-create", Icon20x20 ) );
    Set( "EposSequenceCommands.SnapCameraToViewportAtCurrentTime", new IMAGE_BRUSH_SVG( "camera-snap-to-viewport", Icon20x20 ) );         // inside popup (and maybe big toolbar ?) but Icon..x.. is not used ?
    Set( "EposSequenceCommands.SnapCameraToViewportAtCurrentTime.Small", new IMAGE_BRUSH_SVG( "camera-snap-to-viewport", Icon20x20 ) );   // in the toolbar, stretch to the size Icon..x..
    //Set( "EposSequenceCommands.PilotCameraAtCurrentTime", new IMAGE_BRUSH_SVG( "camera-pilot", Icon20x20 ) );
    //Set( "EposSequenceCommands.PilotCameraAtCurrentTime.Small", new IMAGE_BRUSH_SVG( "camera-pilot", Icon20x20 ) );
    //Set( "EposSequenceCommands.EjectCameraAtCurrentTime", new IMAGE_BRUSH_SVG( "camera-eject", Icon20x20 ) );
    //Set( "EposSequenceCommands.EjectCameraAtCurrentTime.Small", new IMAGE_BRUSH_SVG( "camera-eject", Icon20x20 ) ); // Not used in a widget, only in shortcuts for the moment
    Set( "EposSequenceCommands.GotoPreviousCameraPosition", new IMAGE_BRUSH_SVG( "camera-previous", Icon20x20 ) );
    Set( "EposSequenceCommands.GotoPreviousCameraPosition.Small", new IMAGE_BRUSH_SVG( "camera-previous", Icon20x20 ) );
    Set( "EposSequenceCommands.GotoNextCameraPosition", new IMAGE_BRUSH_SVG( "camera-next", Icon20x20 ) );
    Set( "EposSequenceCommands.GotoNextCameraPosition.Small", new IMAGE_BRUSH_SVG( "camera-next", Icon20x20 ) );

    Set( "EposSequenceCommands.CreateAnimationAtCurrentTime", new IMAGE_BRUSH_SVG( "animation-create", Icon20x20 ) );
    Set( "EposSequenceCommands.CreateAnimationAtCurrentTime.Small", new IMAGE_BRUSH_SVG( "animation-create", Icon20x20 ) );
    Set( "EposSequenceCommands.DetachAnimationAtCurrentTime", new IMAGE_BRUSH_SVG( "animation-detach", Icon20x20 ) );
    Set( "EposSequenceCommands.DetachAnimationAtCurrentTime.Small", new IMAGE_BRUSH_SVG( "animation-detach", Icon20x20 ) );

    Set( "EposSequenceCommands.CreateDrawingAtCurrentTime", new IMAGE_BRUSH_SVG( "drawing-create", Icon20x20 ) );
    Set( "EposSequenceCommands.CreateDrawingAtCurrentTime.Small", new IMAGE_BRUSH_SVG( "drawing-create", Icon20x20 ) );
    Set( "EposSequenceCommands.GotoPreviousDrawing", new IMAGE_BRUSH_SVG( "drawing-previous", Icon20x20 ) );
    Set( "EposSequenceCommands.GotoPreviousDrawing.Small", new IMAGE_BRUSH_SVG( "drawing-previous", Icon20x20 ) );
    Set( "EposSequenceCommands.GotoNextDrawing", new IMAGE_BRUSH_SVG( "drawing-next", Icon20x20 ) );
    Set( "EposSequenceCommands.GotoNextDrawing.Small", new IMAGE_BRUSH_SVG( "drawing-next", Icon20x20 ) );

    Set( "EposSequenceCommands.OpenSequenceEditorSettings", new IMAGE_BRUSH_SVG( "epos-settings", Icon20x20 ) );
    Set( "EposSequenceCommands.OpenSequenceEditorSettings.Small", new IMAGE_BRUSH_SVG( "epos-settings", Icon20x20 ) );
    Set( "EposSequenceCommands.OpenTrackEditorSettings", new IMAGE_BRUSH_SVG( "epos-settings", Icon20x20 ) );
    Set( "EposSequenceCommands.OpenTrackEditorSettings.Small", new IMAGE_BRUSH_SVG( "epos-settings", Icon20x20 ) );
    Set( "EposSequenceCommands.OpenNamingConventionEditorSettings", new IMAGE_BRUSH_SVG( "epos-settings", Icon20x20 ) );
    Set( "EposSequenceCommands.OpenNamingConventionEditorSettings.Small", new IMAGE_BRUSH_SVG( "epos-settings", Icon20x20 ) );

    Set( "EposSequenceCommands.StoryboardViewportResetPanZoomRotate", new IMAGE_BRUSH_SVG( "viewport-reset-transform", Icon20x20 ) );
    Set( "EposSequenceCommands.StoryboardViewportResetPanZoomRotate.Small", new IMAGE_BRUSH_SVG( "viewport-reset-transform", Icon20x20 ) );
}

void
FEposSequenceEditorStyle::InitAbout()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    static const FVector2D Icon30x30( 30.0f, 30.0f );
    static const FVector2D Icon128x128( 128.0f, 128.0f );

    Set( "About.Facebook", new IMAGE_BRUSH_SVG( "About/facebook", Icon30x30 ) );
    Set( "About.LinkedIn", new IMAGE_BRUSH_SVG( "About/linkedin", Icon30x30 ) );
    Set( "About.Youtube", new IMAGE_BRUSH_SVG( "About/youtube", Icon30x30 ) );
    Set( "About.Twitter", new IMAGE_BRUSH_SVG( "About/twitter", Icon30x30 ) );
    Set( "About.Instagram", new IMAGE_BRUSH_SVG( "About/instagram", Icon30x30 ) );
    Set( "About.Discord", new IMAGE_BRUSH_SVG( "About/discord", Icon30x30 ) );
    Set( "About.UserDoc", new IMAGE_BRUSH_SVG( "About/user-doc", Icon30x30 ) );
    Set( "About.Git", new IMAGE_BRUSH_SVG( "About/github", Icon30x30 ) );
    Set( "About.Praxinos", new IMAGE_BRUSH_SVG( "About/praxinos", Icon128x128 ) );
    Set( "About.Epos", new IMAGE_BRUSH_SVG( "About/epos", Icon128x128 ) );

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
}

void
FEposSequenceEditorStyle::InitViewport()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    Set( "Viewport.Settings", new IMAGE_BRUSH_SVG( "viewport-settings", Icon24x24 ) );

    Set( "Viewport.ResetTransform", new IMAGE_BRUSH_SVG( "viewport-reset-transform", Icon24x24 ) );

    //-

    Set( "Notes.Overlay.Text", FTextBlockStyle( FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>( "NormalText" ) )
                                                .SetShadowOffset( FVector2D( 1.0f, 1.0f ) )
                                                .SetShadowColorAndOpacity( FLinearColor( 0.0f, 0.0f, 0.0f ) ) );
    Set( "Notes.Overlay.ListView", FTableViewStyle( FAppStyle::Get().GetWidgetStyle<FTableViewStyle>( "ListView" ) )
                                   .SetBackgroundBrush( FSlateNoResource() ) );
    Set( "Notes.Overlay.TableView.Row", FTableRowStyle( FAppStyle::Get().GetWidgetStyle<FTableRowStyle>( "TableView.NoHoverTableRow" ) )
                                        .SetEvenRowBackgroundBrush( FSlateColorBrush( FLinearColor( FColor( 0, 0, 0, 96 ) ) ) )
                                        .SetOddRowBackgroundBrush( FSlateColorBrush( FLinearColor( FColor( 0, 0, 0, 64 ) ) ) )
                                        .SetActiveBrush( FSlateNoResource() )
                                        .SetInactiveBrush( FSlateNoResource() ) );

    //-

    Set( "Notes.Viewport.ListView", FTableViewStyle( FAppStyle::Get().GetWidgetStyle<FTableViewStyle>( "ListView" ) )
                                    .SetBackgroundBrush( FSlateNoResource() ) );
    Set( "Notes.Viewport.TableView.Row", FTableRowStyle( FAppStyle::Get().GetWidgetStyle<FTableRowStyle>( "TableView.NoHoverTableRow" ) )
                                         .SetEvenRowBackgroundBrush( FSlateColorBrush( FLinearColor( FColor( 20, 20, 20 ) ) ) )
                                         .SetOddRowBackgroundBrush( FSlateColorBrush( FLinearColor( FColor( 15, 15, 15 ) ) ) )
                                         .SetActiveBrush( FSlateNoResource() )
                                         .SetInactiveBrush( FSlateNoResource() ) );

    Set( "Notes.Viewport.Text.NoNotes", FTextBlockStyle( FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>( "NormalText" ) )
                                        .SetFont( DEFAULT_FONT( "Italic", 10 ) )
                                        .SetColorAndOpacity( FLinearColor( FColor( 32, 32, 32 ) ) ) );


    Set( "Viewport.Toolbar.SpinBox", FSpinBoxStyle( FAppStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBox") )
        .SetTextPadding(FMargin(0.f))
        .SetInsetPadding(FMargin(0.f))
    );
    Set( "Viewport.Toolbar.SpinBox.ComboButton", FComboButtonStyle( FAppStyle::Get().GetWidgetStyle< FComboButtonStyle >( "ComboButton" ) )
        .SetContentPadding(FMargin(0))
        .SetDownArrowPadding(FMargin(0))
    );
    Set( "Viewport.Toolbar.SpinBox.Button", FButtonStyle( FAppStyle::Get().GetWidgetStyle< FButtonStyle >( "EditorViewportToolBar.Button" ) )
        .SetNormalPadding(FMargin(4.f, 0.f, 3.0f, 0.f))
        .SetPressedPadding(FMargin(4.f, 0.f, 3.0f, 0.f))
    );

    //---

    SetContentRoot( FPaths::EnginePluginsDir() / TEXT( "MovieScene/LevelSequenceEditor/Content" ) );
    {
        Set( "CinematicViewportPlayMarker", new IMAGE_BRUSH( "CinematicViewportPlayMarker", FVector2D( 11, 6 ) ) );
        Set( "CinematicViewportRangeStart", new BORDER_BRUSH( "CinematicViewportRangeStart", FMargin( 1.f, .3f, 0.f, .6f ) ) );
        Set( "CinematicViewportRangeEnd", new BORDER_BRUSH( "CinematicViewportRangeEnd", FMargin( 0.f, .3f, 1.f, .6f ) ) );

        Set( "CinematicViewportTransportRangeKey", new IMAGE_BRUSH( "CinematicViewportTransportRangeKey", FVector2D( 7.f, 7.f ) ) );

        //---

        Set( "FilmOverlay.DefaultThumbnail", new IMAGE_BRUSH( "DefaultFilmOverlayThumbnail", FVector2D( 36, 24 ) ) );

        Set( "FilmOverlay.Disabled", new IMAGE_BRUSH( "FilmOverlay.Disabled", FVector2D( 36, 24 ) ) );
        Set( "FilmOverlay.Grid2x2", new IMAGE_BRUSH( "FilmOverlay.2x2Grid", FVector2D( 36, 24 ) ) );
        Set( "FilmOverlay.Grid3x3", new IMAGE_BRUSH( "FilmOverlay.3x3Grid", FVector2D( 36, 24 ) ) );
        Set( "FilmOverlay.Crosshair", new IMAGE_BRUSH( "FilmOverlay.Crosshair", FVector2D( 36, 24 ) ) );
        Set( "FilmOverlay.Rabatment", new IMAGE_BRUSH( "FilmOverlay.Rabatment", FVector2D( 36, 24 ) ) );
    }
}

void
FEposSequenceEditorStyle::InitImportExport()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    Set( "ExportImageSequence.TableRow", FTableRowStyle( FAppStyle::Get().GetWidgetStyle<FTableRowStyle>( "ProjectBrowser.TableRow" ) ) );

    // Nearly same as ProjectBrowser.ProjectTile.*
    Set( "ExportImageSequence.PanelItem.TopAreaBackground", new FSlateRoundedBoxBrush( EStyleColor::Header, FVector4( 4.0f, 4.0f, 0.0f, 0.0f ) ) );
    Set( "ExportImageSequence.PanelItem.TopAreaHoverBackground", new FSlateRoundedBoxBrush( FStyleColors::Hover, FVector4( 4.0f, 4.0f, 0.0f, 0.0f ) ) );
    Set( "ExportImageSequence.PanelItem.BottomAreaBackground", new FSlateRoundedBoxBrush( EStyleColor::Header, FVector4( 0.0f, 0.0f, 4.0f, 4.0f ) ) );
    Set( "ExportImageSequence.PanelItem.BottomAreaHoverBackground", new FSlateRoundedBoxBrush( FStyleColors::Hover, FVector4( 0.0f, 0.0f, 4.0f, 4.0f ) ) );
    Set( "ExportImageSequence.PanelItem.ThumbnailAreaBackground", new FSlateRoundedBoxBrush( COLOR( "#474747FF" ), FVector4( 0.0f, 0.0f, 0.0f, 0.0f ) ) );

    //---

    Set( "ImportImageSequence.TableRow", FTableRowStyle( FAppStyle::Get().GetWidgetStyle<FTableRowStyle>( "ProjectBrowser.TableRow" ) ) );

    auto ModifyColorV = []( const FLinearColor& iColor, float iRatio ) -> FLinearColor
    {
        FLinearColor hsv( iColor.LinearRGBToHSV() );
        hsv.B *= iRatio;
        return hsv.HSVToLinearRGB();
    };

    FSlateColor primary( EStyleColor::Primary );
    FSlateColor primary_hover( EStyleColor::PrimaryHover );

    FSlateColor even( primary.GetSpecifiedColor() );
    FSlateColor odd( ModifyColorV( primary.GetSpecifiedColor(), .25f ) );
    FSlateColor even_hover( primary_hover.GetSpecifiedColor() );
    FSlateColor odd_hover( ModifyColorV( primary_hover.GetSpecifiedColor(), .25f ) );

    Set( "ImportImageSequence.PanelItem.TopAreaBackground.Even", new FSlateRoundedBoxBrush( even, FVector4( 4.0f, 4.0f, 0.0f, 0.0f ) ) );
    Set( "ImportImageSequence.PanelItem.TopAreaBackground.Odd", new FSlateRoundedBoxBrush( odd, FVector4( 4.0f, 4.0f, 0.0f, 0.0f ) ) );
    Set( "ImportImageSequence.PanelItem.TopAreaBackground.Even.Hover", new FSlateRoundedBoxBrush( even_hover, FVector4( 4.0f, 4.0f, 0.0f, 0.0f ) ) );
    Set( "ImportImageSequence.PanelItem.TopAreaBackground.Odd.Hover", new FSlateRoundedBoxBrush( odd_hover, FVector4( 4.0f, 4.0f, 0.0f, 0.0f ) ) );
    Set( "ImportImageSequence.PanelItem.BottomAreaBackground.Even", new FSlateRoundedBoxBrush( even, FVector4( 0.0f, 0.0f, 4.0f, 4.0f ) ) );
    Set( "ImportImageSequence.PanelItem.BottomAreaBackground.Odd", new FSlateRoundedBoxBrush( odd, FVector4( 0.0f, 0.0f, 4.0f, 4.0f ) ) );
    Set( "ImportImageSequence.PanelItem.BottomAreaBackground.Even.Hover", new FSlateRoundedBoxBrush( even_hover, FVector4( 0.0f, 0.0f, 4.0f, 4.0f ) ) );
    Set( "ImportImageSequence.PanelItem.BottomAreaBackground.Odd.Hover", new FSlateRoundedBoxBrush( odd_hover, FVector4( 0.0f, 0.0f, 4.0f, 4.0f ) ) );
    Set( "ImportImageSequence.PanelItem.ThumbnailAreaBackground.Even", new FSlateRoundedBoxBrush( even, FVector4( 0.0f, 0.0f, 0.0f, 0.0f ) ) );
    Set( "ImportImageSequence.PanelItem.ThumbnailAreaBackground.Odd", new FSlateRoundedBoxBrush( odd, FVector4( 0.0f, 0.0f, 0.0f, 0.0f ) ) );
    Set( "ImportImageSequence.PanelItem.ThumbnailAreaBackground.Even.Hover", new FSlateRoundedBoxBrush( even_hover, FVector4( 0.0f, 0.0f, 0.0f, 0.0f ) ) );
    Set( "ImportImageSequence.PanelItem.ThumbnailAreaBackground.Odd.Hover", new FSlateRoundedBoxBrush( odd_hover, FVector4( 0.0f, 0.0f, 0.0f, 0.0f ) ) );

    Set( "ImportImageSequence.PanelItem.BoardAreaBackground.Even", new FSlateNoResource() );
    Set( "ImportImageSequence.PanelItem.BoardAreaBackground.Odd", new FSlateNoResource() );
    Set( "ImportImageSequence.PanelItem.ShotAreaBackground.Even", new FSlateNoResource() );
    Set( "ImportImageSequence.PanelItem.ShotAreaBackground.Odd", new FSlateNoResource() );
    Set( "ImportImageSequence.PanelItem.PanelAreaBackground", new FSlateNoResource() );

    //---

    SetContentRoot( FPaths::EngineContentDir() / TEXT( "Editor" ) / TEXT( "Slate" ) );
    {
        Set( "ExportImageSequence.PanelItem.DropShadow", new BOX_BRUSH( "Starship/ContentBrowser/drop-shadow", FMargin( 4.0f / 64.0f ) ) );
        Set( "ImportImageSequence.PanelItem.DropShadow", new BOX_BRUSH( "Starship/ContentBrowser/drop-shadow", FMargin( 4.0f / 64.0f ) ) );
    }
}

//---

//static
const FEposSequenceEditorStyle&
FEposSequenceEditorStyle::Get()
{
    if( !smSingleton.IsValid() )
        smSingleton = MakeShareable( new FEposSequenceEditorStyle );

    return *smSingleton;
}
