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
    Set( "EposSequenceEditor.Tabs.Sequencer", new IMAGE_BRUSH( "SequencerTab_16x", Icon16x16 ) ); // Seems to be only in toolkit standalone mode, but the plugin seems to always be in world-centric mode

    // asset thumbnail
    Set( "ClassIcon.BoardSequence", new IMAGE_BRUSH( "BoardSequence_16x", Icon16x16 ) );
    Set( "ClassThumbnail.BoardSequence", new IMAGE_BRUSH( "BoardSequence_64x", Icon64x64 ) );
    Set( "ClassIcon.ShotSequence", new IMAGE_BRUSH( "ShotSequence_16x", Icon16x16 ) );
    Set( "ClassThumbnail.ShotSequence", new IMAGE_BRUSH( "ShotSequence_64x", Icon64x64 ) );

    Set( "BoardSequenceEditor.NewStoryboardWithSettings", new IMAGE_BRUSH( "NewStoryboardWithSettings", Icon16x16 ) );
    Set( "BoardSequenceEditor.NewStoryboardWithSettings.Small", new IMAGE_BRUSH( "NewStoryboardWithSettings", Icon16x16 ) );

    //---

    Set( "ShotSequenceEditor.CreateCamera", new IMAGE_BRUSH( "IconCreateCamera_24x", Icon48x48 ) );
    Set( "ShotSequenceEditor.CreateCamera.Small", new IMAGE_BRUSH( "IconCreateCamera_24x", Icon24x24 ) );
    Set( "ShotSequenceEditor.SnapCameraToViewport", new IMAGE_BRUSH( "IconSnapCameraToViewport_24x", Icon48x48 ) );         // inside popup (and maybe big toolbar ?) but Icon..x.. is not used ?
    Set( "ShotSequenceEditor.SnapCameraToViewport.Small", new IMAGE_BRUSH( "IconSnapCameraToViewport_24x", Icon24x24 ) );   // in the toolbar, stretch to the size Icon..x..

    Set( "ShotSequenceEditor.CreatePlane", new IMAGE_BRUSH( "IconCreatePlane_24x", Icon48x48 ) );
    Set( "ShotSequenceEditor.CreatePlane.Small", new IMAGE_BRUSH( "IconCreatePlane_24x", Icon24x24 ) );
    Set( "ShotSequenceEditor.DetachPlane", new IMAGE_BRUSH( "IconDetachPlane_24x", Icon48x48 ) );
    Set( "ShotSequenceEditor.DetachPlane.Small", new IMAGE_BRUSH( "IconDetachPlane_24x", Icon24x24 ) );

    Set( "ShotSequenceEditor.CreateDrawing", new IMAGE_BRUSH( "IconCreateDrawing_24x", Icon48x48 ) );
    Set( "ShotSequenceEditor.CreateDrawing.Small", new IMAGE_BRUSH( "IconCreateDrawing_24x", Icon24x24 ) );
    Set( "ShotSequenceEditor.GotoPreviousDrawing", new IMAGE_BRUSH( "IconDrawingPrevious_24x", Icon48x48 ) );
    Set( "ShotSequenceEditor.GotoPreviousDrawing.Small", new IMAGE_BRUSH( "IconDrawingPrevious_24x", Icon24x24 ) );
    Set( "ShotSequenceEditor.GotoNextDrawing", new IMAGE_BRUSH( "IconDrawingNext_24x", Icon48x48 ) );
    Set( "ShotSequenceEditor.GotoNextDrawing.Small", new IMAGE_BRUSH( "IconDrawingNext_24x", Icon24x24 ) );

    //---

    Set( "BoardSequenceEditor.NewSectionWithBoardAtCurrentFrame", new IMAGE_BRUSH( "IconNewSectionWithBoardAtCurrentFrame_24x", Icon48x48 ) );
    Set( "BoardSequenceEditor.NewSectionWithBoardAtCurrentFrame.Small", new IMAGE_BRUSH( "IconNewSectionWithBoardAtCurrentFrame_24x", Icon24x24 ) );
    Set( "BoardSequenceEditor.NewSectionWithShotAtCurrentFrame", new IMAGE_BRUSH( "IconNewSectionWithShotAtCurrentFrame_24x", Icon48x48 ) );
    Set( "BoardSequenceEditor.NewSectionWithShotAtCurrentFrame.Small", new IMAGE_BRUSH( "IconNewSectionWithShotAtCurrentFrame_24x", Icon24x24 ) );

    //---

    Set( "EposSequenceEditor.Help", new IMAGE_BRUSH( "IconHelp_24x", Icon48x48 ) );
    Set( "EposSequenceEditor.Help.Small", new IMAGE_BRUSH( "IconHelp_24x", Icon24x24 ) );
    Set( "EposSequenceEditor.OpenAboutWindow", new IMAGE_BRUSH( "IconAbout_24x", Icon48x48 ) );
    Set( "EposSequenceEditor.OpenAboutWindow.Small", new IMAGE_BRUSH( "IconAbout_24x", Icon24x24 ) );

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
