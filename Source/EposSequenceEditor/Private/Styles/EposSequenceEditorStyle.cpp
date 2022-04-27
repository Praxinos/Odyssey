// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

namespace
{
static const FVector2D Icon8x8( 8.0f, 8.0f );
static const FVector2D Icon14x14( 14.0f, 14.0f );
static const FVector2D Icon16x16( 16.0f, 16.0f );
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
    InitSequencer();
    InitCommands();
    InitAbout();
    InitViewport();
}

void
FEposSequenceEditorStyle::InitSequencer()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Epos" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    // tab icons
    Set( "EposSequenceEditor.Tabs.Sequencer", new IMAGE_BRUSH_SVG( "sequencer-tab", Icon16x16 ) ); // Seems to be only in toolkit standalone mode, but the plugin seems to always be in world-centric mode

    // asset thumbnail
    Set( "ClassIcon.BoardSequence", new IMAGE_BRUSH( "sequence-board-16x", Icon16x16 ) );
    Set( "ClassThumbnail.BoardSequence", new IMAGE_BRUSH( "sequence-board-64x", Icon64x64 ) );
    Set( "ClassIcon.ShotSequence", new IMAGE_BRUSH( "sequence-shot-16x", Icon16x16 ) );
    Set( "ClassThumbnail.ShotSequence", new IMAGE_BRUSH( "sequence-shot-64x", Icon64x64 ) );

    //---

    Set( "Sequencer.InfoBar",
         FTextBlockStyle( FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Heading" ) )
         .SetFont( DEFAULT_FONT( "Regular", 10 ) )
         .SetColorAndOpacity( FLinearColor( 0.4f, 0.4, 0.4f, 1.0f ) )
    );

    Set( "Settings", new IMAGE_BRUSH_SVG( "epos-settings", Icon48x48 ) );
    Set( "Settings.Small", new IMAGE_BRUSH_SVG( "epos-settings", FVector2D( 20.f, 20.f ) ) );

    Set( "Help", new IMAGE_BRUSH_SVG( "epos-help", Icon48x48 ) );
    Set( "Help.Small", new IMAGE_BRUSH_SVG( "epos-help", FVector2D( 20.f, 20.f ) ) );
}

void
FEposSequenceEditorStyle::InitCommands()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Epos" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    Set( "EposSequenceCommands.NewStoryboardWithSettings", new IMAGE_BRUSH( "new-storyboard-with-settings-16x", Icon16x16 ) );
    Set( "EposSequenceCommands.NewStoryboardWithSettings.Small", new IMAGE_BRUSH( "new-storyboard-with-settings-16x", Icon16x16 ) );

    Set( "EposSequenceCommands.CreateCameraAtCurrentTime", new IMAGE_BRUSH( "camera-create-24x", Icon48x48 ) );
    Set( "EposSequenceCommands.CreateCameraAtCurrentTime.Small", new IMAGE_BRUSH( "camera-create-24x", Icon24x24 ) );
    Set( "EposSequenceCommands.SnapCameraToViewportAtCurrentTime", new IMAGE_BRUSH( "camera-snap-to-viewport-24x", Icon48x48 ) );         // inside popup (and maybe big toolbar ?) but Icon..x.. is not used ?
    Set( "EposSequenceCommands.SnapCameraToViewportAtCurrentTime.Small", new IMAGE_BRUSH( "camera-snap-to-viewport-24x", Icon24x24 ) );   // in the toolbar, stretch to the size Icon..x..
    //Set( "EposSequenceCommands.PilotCameraAtCurrentTime", new IMAGE_BRUSH( "camera-pilot-16x", Icon48x48 ) );
    //Set( "EposSequenceCommands.PilotCameraAtCurrentTime.Small", new IMAGE_BRUSH( "camera-pilot-16x", Icon24x24 ) );
    //Set( "EposSequenceCommands.EjectCameraAtCurrentTime", new IMAGE_BRUSH( "camera-eject-16x", Icon48x48 ) );
    //Set( "EposSequenceCommands.EjectCameraAtCurrentTime.Small", new IMAGE_BRUSH( "camera-eject-16x", Icon24x24 ) ); // Not used in a widget, only in shortcuts for the moment
    Set( "EposSequenceCommands.GotoPreviousCameraPosition", new IMAGE_BRUSH( "camera-previous-24x", Icon48x48 ) );
    Set( "EposSequenceCommands.GotoPreviousCameraPosition.Small", new IMAGE_BRUSH( "camera-previous-24x", Icon24x24 ) );
    Set( "EposSequenceCommands.GotoNextCameraPosition", new IMAGE_BRUSH( "camera-next-24x", Icon48x48 ) );
    Set( "EposSequenceCommands.GotoNextCameraPosition.Small", new IMAGE_BRUSH( "camera-next-24x", Icon24x24 ) );

    Set( "EposSequenceCommands.CreatePlaneAtCurrentTime", new IMAGE_BRUSH( "plane-create-24x", Icon48x48 ) );
    Set( "EposSequenceCommands.CreatePlaneAtCurrentTime.Small", new IMAGE_BRUSH( "plane-create-24x", Icon24x24 ) );
    Set( "EposSequenceCommands.DetachPlaneAtCurrentTime", new IMAGE_BRUSH( "plane-detach-24x", Icon48x48 ) );
    Set( "EposSequenceCommands.DetachPlaneAtCurrentTime.Small", new IMAGE_BRUSH( "plane-detach-24x", Icon24x24 ) );

    Set( "EposSequenceCommands.CreateDrawingAtCurrentTime", new IMAGE_BRUSH( "drawing-create-24x", Icon48x48 ) );
    Set( "EposSequenceCommands.CreateDrawingAtCurrentTime.Small", new IMAGE_BRUSH( "drawing-create-24x", Icon24x24 ) );
    Set( "EposSequenceCommands.GotoPreviousDrawing", new IMAGE_BRUSH( "drawing-previous-24x", Icon48x48 ) );
    Set( "EposSequenceCommands.GotoPreviousDrawing.Small", new IMAGE_BRUSH( "drawing-previous-24x", Icon24x24 ) );
    Set( "EposSequenceCommands.GotoNextDrawing", new IMAGE_BRUSH( "drawing-next-24x", Icon48x48 ) );
    Set( "EposSequenceCommands.GotoNextDrawing.Small", new IMAGE_BRUSH( "drawing-next-24x", Icon24x24 ) );

    Set( "EposSequenceCommands.OpenSequenceEditorSettings", new IMAGE_BRUSH_SVG( "epos-settings", Icon48x48 ) );
    Set( "EposSequenceCommands.OpenSequenceEditorSettings.Small", new IMAGE_BRUSH_SVG( "epos-settings", Icon24x24 ) );
    Set( "EposSequenceCommands.OpenTrackEditorSettings", new IMAGE_BRUSH_SVG( "epos-settings", Icon48x48 ) );
    Set( "EposSequenceCommands.OpenTrackEditorSettings.Small", new IMAGE_BRUSH_SVG( "epos-settings", Icon24x24 ) );
    Set( "EposSequenceCommands.OpenNamingConventionEditorSettings", new IMAGE_BRUSH_SVG( "epos-settings", Icon48x48 ) );
    Set( "EposSequenceCommands.OpenNamingConventionEditorSettings.Small", new IMAGE_BRUSH_SVG( "epos-settings", Icon24x24 ) );

    Set( "EposSequenceCommands.OpenAboutWindow", new IMAGE_BRUSH_SVG( "epos-about", Icon48x48 ) );
    Set( "EposSequenceCommands.OpenAboutWindow.Small", new IMAGE_BRUSH_SVG( "epos-about", Icon24x24 ) );
}

void
FEposSequenceEditorStyle::InitAbout()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Epos" )->GetBaseDir() / TEXT( "Resources" ) );

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
                                .SetColorAndOpacity( FLinearColor( 1.f, 1.f, 1.f ) ) );
    Set( "About.SmallText", FTextBlockStyle( FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>( "SmallText" ) )
                            .SetColorAndOpacity( FLinearColor( 1.f, 1.f, 1.f ) ) );
    Set( "About.BigText", FTextBlockStyle( FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>( "NormalText" ) )
                          .SetFontSize( 15 )
                          .SetColorAndOpacity( FLinearColor( 1.f, 1.f, 1.f ) ) );

}

void
FEposSequenceEditorStyle::InitViewport()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Epos" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    Set( "Notes.Viewport.Settings", new IMAGE_BRUSH_SVG( "note", Icon24x24 ) );

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
        Set( "FilmOverlay.2x2Grid", new IMAGE_BRUSH( "FilmOverlay.2x2Grid", FVector2D( 36, 24 ) ) );
        Set( "FilmOverlay.3x3Grid", new IMAGE_BRUSH( "FilmOverlay.3x3Grid", FVector2D( 36, 24 ) ) );
        Set( "FilmOverlay.Crosshair", new IMAGE_BRUSH( "FilmOverlay.Crosshair", FVector2D( 36, 24 ) ) );
        Set( "FilmOverlay.Rabatment", new IMAGE_BRUSH( "FilmOverlay.Rabatment", FVector2D( 36, 24 ) ) );
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
