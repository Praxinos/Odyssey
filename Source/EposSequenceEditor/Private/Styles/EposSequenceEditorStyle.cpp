// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Styles/EposSequenceEditorStyle.h"

#include "Brushes/SlateBorderBrush.h"
#include "Brushes/SlateBoxBrush.h"
#include "Brushes/SlateImageBrush.h"
#include "Fonts/SlateFontInfo.h"
#include "Interfaces/IPluginManager.h"
#include "Layout/Margin.h"
#include "Misc/Paths.h"
#include "Styling/SlateStyleRegistry.h"

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BORDER_BRUSH(RelativePath, ...) FSlateBorderBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)

//---

TSharedPtr<FEposSequenceEditorStyle> FEposSequenceEditorStyle::smSingleton;

//---

FEposSequenceEditorStyle::FEposSequenceEditorStyle()
    : FSlateStyleSet( "EposSequenceEditorStyle" )
{
    const FVector2D Icon16x16( 16.0f, 16.0f );
    const FVector2D Icon24x24( 24.0f, 24.0f );
    const FVector2D Icon48x48( 48.0f, 48.0f );
    const FVector2D Icon64x64( 64.0f, 64.0f );

    TSharedPtr<IPlugin> epos_plugin = IPluginManager::Get().FindPlugin( "Epos" );
    check( epos_plugin.IsValid() );
    SetContentRoot( epos_plugin->GetBaseDir() / TEXT( "Resources" ) );

    //---

    // tab icons
    Set( "EposSequenceEditor.Tabs.Sequencer", new IMAGE_BRUSH( "sequencer-tab-16x", Icon16x16 ) ); // Seems to be only in toolkit standalone mode, but the plugin seems to always be in world-centric mode

    // asset thumbnail
    Set( "ClassIcon.BoardSequence", new IMAGE_BRUSH( "sequence-board-16x", Icon16x16 ) );
    Set( "ClassThumbnail.BoardSequence", new IMAGE_BRUSH( "sequence-board-64x", Icon64x64 ) );
    Set( "ClassIcon.ShotSequence", new IMAGE_BRUSH( "sequence-shot-16x", Icon16x16 ) );
    Set( "ClassThumbnail.ShotSequence", new IMAGE_BRUSH( "sequence-shot-64x", Icon64x64 ) );

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

    Set( "EposSequenceCommands.OpenSequenceEditorSettings", new IMAGE_BRUSH( "epos-settings-24x", Icon48x48 ) );
    Set( "EposSequenceCommands.OpenSequenceEditorSettings.Small", new IMAGE_BRUSH( "epos-settings-24x", Icon24x24 ) );
    Set( "EposSequenceCommands.OpenTrackEditorSettings", new IMAGE_BRUSH( "epos-settings-24x", Icon48x48 ) );
    Set( "EposSequenceCommands.OpenTrackEditorSettings.Small", new IMAGE_BRUSH( "epos-settings-24x", Icon24x24 ) );

    Set( "EposSequenceCommands.OpenAboutWindow", new IMAGE_BRUSH( "epos-about-24x", Icon48x48 ) );
    Set( "EposSequenceCommands.OpenAboutWindow.Small", new IMAGE_BRUSH( "epos-about-24x", Icon24x24 ) );

    //---

    Set( "EposSequenceEditor.ToolBar.Heading",
         FTextBlockStyle( FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Heading" ) )
         .SetFont( DEFAULT_FONT( "Regular", 10 ) )
         .SetColorAndOpacity( FLinearColor( 0.4f, 0.4, 0.4f, 1.0f ) )
         );

    Set( "EposSequenceEditor.Settings", new IMAGE_BRUSH( "epos-settings-24x", Icon48x48 ) );
    Set( "EposSequenceEditor.Settings.Small", new IMAGE_BRUSH( "epos-settings-24x", FVector2D( 20.f, 20.f ) ) );

    Set( "EposSequenceEditor.Help", new IMAGE_BRUSH( "epos-help-24x", Icon48x48 ) );
    Set( "EposSequenceEditor.Help.Small", new IMAGE_BRUSH( "epos-help-24x", FVector2D( 20.f, 20.f ) ) );

    //---

    Set( "About.Facebook", new IMAGE_BRUSH( "About/facebook", FVector2D( 30, 30 ) ) );
    Set( "About.LinkedIn", new IMAGE_BRUSH( "About/linkedin", FVector2D( 30, 30 ) ) );
    Set( "About.Youtube", new IMAGE_BRUSH( "About/youtube", FVector2D( 30, 30 ) ) );
    Set( "About.Twitter", new IMAGE_BRUSH( "About/twitter", FVector2D( 30, 30 ) ) );
    Set( "About.Instagram", new IMAGE_BRUSH( "About/instagram", FVector2D( 30, 30 ) ) );
    Set( "About.Discord", new IMAGE_BRUSH( "About/discord", FVector2D( 30, 30 ) ) );
    Set( "About.UserDoc", new IMAGE_BRUSH( "About/user-doc", FVector2D( 30, 30 ) ) );
    Set( "About.Git", new IMAGE_BRUSH( "About/git", FVector2D( 30, 30 ) ) );
    Set( "About.Praxinos", new IMAGE_BRUSH( "About/praxinos", FVector2D( 128, 128 ) ) );
    Set( "About.Epos", new IMAGE_BRUSH( "About/epos", FVector2D( 128, 128 ) ) );

    Set( "About.UnderlineText", FTextBlockStyle( FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>( "NormalUnderlinedText" ) )
                                .SetFontSize( 10 )
                                .SetColorAndOpacity( FLinearColor( 1.f, 1.f, 1.f ) ) );
    Set( "About.SmallText", FTextBlockStyle( FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>( "SmallText" ) )
                            .SetColorAndOpacity( FLinearColor( 1.f, 1.f, 1.f ) ) );
    Set( "About.BigText", FTextBlockStyle( FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>( "NormalText" ) )
                          .SetFontSize( 15 )
                          .SetColorAndOpacity( FLinearColor( 1.f, 1.f, 1.f ) ) );

    //---

    SetContentRoot( FPaths::EnginePluginsDir() / TEXT( "MovieScene/LevelSequenceEditor/Content" ) );

    Set( "EposSequenceEditor.CinematicViewportPlayMarker", new IMAGE_BRUSH( "CinematicViewportPlayMarker", FVector2D( 11, 6 ) ) );
    Set( "EposSequenceEditor.CinematicViewportRangeStart", new BORDER_BRUSH( "CinematicViewportRangeStart", FMargin( 1.f, .3f, 0.f, .6f ) ) );
    Set( "EposSequenceEditor.CinematicViewportRangeEnd", new BORDER_BRUSH( "CinematicViewportRangeEnd", FMargin( 0.f, .3f, 1.f, .6f ) ) );

    Set( "EposSequenceEditor.CinematicViewportTransportRangeKey", new IMAGE_BRUSH( "CinematicViewportTransportRangeKey", FVector2D( 7.f, 7.f ) ) );

    Set( "FilmOverlay.DefaultThumbnail", new IMAGE_BRUSH( "DefaultFilmOverlayThumbnail", FVector2D( 36, 24 ) ) );

    Set( "FilmOverlay.Disabled", new IMAGE_BRUSH( "FilmOverlay.Disabled", FVector2D( 36, 24 ) ) );
    Set( "FilmOverlay.2x2Grid", new IMAGE_BRUSH( "FilmOverlay.2x2Grid", FVector2D( 36, 24 ) ) );
    Set( "FilmOverlay.3x3Grid", new IMAGE_BRUSH( "FilmOverlay.3x3Grid", FVector2D( 36, 24 ) ) );
    Set( "FilmOverlay.Crosshair", new IMAGE_BRUSH( "FilmOverlay.Crosshair", FVector2D( 36, 24 ) ) );
    Set( "FilmOverlay.Rabatment", new IMAGE_BRUSH( "FilmOverlay.Rabatment", FVector2D( 36, 24 ) ) );

    //---

    FSlateStyleRegistry::RegisterSlateStyle( *this );
}

FEposSequenceEditorStyle::~FEposSequenceEditorStyle()
{
    FSlateStyleRegistry::UnRegisterSlateStyle( *this );
}

//---

//static
TSharedRef<FEposSequenceEditorStyle>
FEposSequenceEditorStyle::Get()
{
    if( !smSingleton.IsValid() )
        smSingleton = MakeShareable( new FEposSequenceEditorStyle );

    return smSingleton.ToSharedRef();
}

#undef IMAGE_BRUSH
