// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Styles/EposTracksEditorStyle.h"

#include "Brushes/SlateBorderBrush.h"
#include "Brushes/SlateBoxBrush.h"
#include "Brushes/SlateImageBrush.h"
#include "Fonts/SlateFontInfo.h"
#include "Interfaces/IPluginManager.h"
#include "Layout/Margin.h"
#include "Misc/Paths.h"
#include "SequencerSectionPainter.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/StarshipCoreStyle.h"
#include "Styling/ToolBarStyle.h"

#define PARENT_IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( GetParentStyle()->GetContentRootDir() / RelativePath + TEXT(".png"), __VA_ARGS__ )

namespace
{
static const FVector2D Icon8x8( 8.0f, 8.0f );
static const FVector2D Icon14x14( 14.0f, 14.0f );
static const FVector2D Icon16x16( 16.0f, 16.0f );
static const FVector2D Icon24x16( 24.0f, 16.0f );
static const FVector2D Icon24x24( 24.0f, 24.0f );
static const FVector2D Icon48x48( 48.0f, 48.0f );
static const FVector2D Icon64x64( 64.0f, 64.0f );
}

//---

TSharedPtr<FEposTracksEditorStyle> FEposTracksEditorStyle::smSingleton;

//---

//static
void
FEposTracksEditorStyle::Register()
{
    FSlateStyleRegistry::RegisterSlateStyle( Get() );
}

//static
void
FEposTracksEditorStyle::Unregister()
{
    FSlateStyleRegistry::UnRegisterSlateStyle( Get() );
}

//---

FEposTracksEditorStyle::FEposTracksEditorStyle()
    : FSlateStyleSet( "EposTracksEditorStyle" )
{
    SetParentStyleName( FAppStyle::Get().GetStyleSetName() ); // Mainly to not have to duplicate all "Menu.*"

    InitSequencer();
    InitTracks();
    InitCommands();
    InitToolbar();
    InitNotes();
}

void
FEposTracksEditorStyle::InitSequencer()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    Set( "Sequencer.Tracks.CinematicBoard", new IMAGE_BRUSH_SVG( "track-board", Icon16x16 ) );
    Set( "Sequencer.Tracks.SingleCameraCut", new PARENT_IMAGE_BRUSH( "Sequencer/Dropdown_Icons/Icon_Camera_Cut_Track_16x", Icon16x16 ) ); // same as FAppStyle::Get().GetBrush( "Sequencer.Tracks.CameraCut" ) );
    Set( "Sequencer.Tracks.Note", new IMAGE_BRUSH_SVG( "track-note", Icon16x16 ) );
}

void
FEposTracksEditorStyle::InitTracks()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    Set( "CreateCamera", new IMAGE_BRUSH_SVG( "camera-create.large", Icon24x16 ) );
    Set( "PilotCamera", new IMAGE_BRUSH_SVG( "camera-pilot", Icon16x16 ) );
    Set( "EjectCamera", new IMAGE_BRUSH_SVG( "camera-eject", Icon16x16 ) );
    Set( "SnapCameraToViewport", new IMAGE_BRUSH_SVG( "camera-snap-to-viewport", Icon16x16 ) );

    Set( "CreateAnimationCut", new IMAGE_BRUSH_SVG( "animationcut-create", Icon16x16 ) );

    Set( "CreateAnimation", new IMAGE_BRUSH_SVG( "animation-create.large", Icon24x16 ) );
    Set( "DetachAnimation", new IMAGE_BRUSH_SVG( "animation-detach", Icon16x16 ) );

    Set( "LighttableOff", new IMAGE_BRUSH_SVG( "lighttable-off", Icon16x16 ) );
    Set( "LighttableOn", new IMAGE_BRUSH_SVG( "lighttable-on", Icon16x16 ) );

    Set( "Take", new IMAGE_BRUSH_SVG( "take", Icon16x16 ) );

    Set( "NoteSettings", new IMAGE_BRUSH_SVG( "note", Icon16x16 ) );
    Set( "CreateNote", new IMAGE_BRUSH_SVG( "note-create.large", Icon24x16 ) );

    //---

    SetContentRoot( FPaths::EngineContentDir() / TEXT( "Editor" ) / TEXT( "Slate" ) );
    {
        Set( "HyperlinkSpinBox", FSpinBoxStyle( FAppStyle::Get().GetWidgetStyle<FSpinBoxStyle>( "Sequencer.HyperlinkSpinBox" ) )
             .SetBackgroundBrush( BORDER_BRUSH( "Old/HyperlinkDotted", FMargin( 0, 0, 0, 3 / 16.0f ), FSlateColor::UseForeground() ) )
             .SetForegroundColor( FSlateColor::UseForeground() )
        );
    }
}

void
FEposTracksEditorStyle::InitCommands()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    Set( "EposTracksEditorCommands.NewSectionWithBoardAtCurrentFrame", new IMAGE_BRUSH_SVG( "sequence-board", Icon16x16 ) );
    Set( "EposTracksEditorCommands.NewSectionWithShotAtCurrentFrame", new IMAGE_BRUSH_SVG( "sequence-shot", Icon16x16 ) );

    Set( "EposTracksEditorCommands.ArrangeShotsManually", new IMAGE_BRUSH_SVG( "arrange-shots-manually", Icon16x16 ) );
    Set( "EposTracksEditorCommands.ArrangeShotsOnOneRow", new IMAGE_BRUSH_SVG( "arrange-shots-on-one-row", Icon16x16 ) );
    Set( "EposTracksEditorCommands.ArrangeShotsOnTwoRows", new IMAGE_BRUSH_SVG( "arrange-shots-on-two-rows", Icon16x16 ) );
}

void
FEposTracksEditorStyle::InitToolbar()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    SetCoreContentRoot( FPaths::EngineContentDir() / TEXT( "Slate" ) );
    {
        FToolBarStyle SectionToolBarStyle = FStarshipCoreStyle::GetCoreStyle().GetWidgetStyle<FToolBarStyle>( "SlimToolBar" );

        SectionToolBarStyle.SetBackground( FAppStyle::Get().GetWidgetStyle< FComboButtonStyle >( "ComboButton" ).ButtonStyle.Normal ); // To have the same background as the '+' button on each side
        SectionToolBarStyle.SetBackgroundPadding( 0 );
        SectionToolBarStyle.SetIconSize( Icon14x14 );
        //TODO: maybe replace SetSettingsComboButtonStyle() by SetWrapButtonStyle() like below (?)
        SectionToolBarStyle.SetSettingsComboButtonStyle( SectionToolBarStyle.SettingsComboButton.SetDownArrowImage( CORE_IMAGE_BRUSH_SVG( "Starship/Common/ellipsis-vertical-narrow", FVector2D( 4, 16 ) ) ) );
        SectionToolBarStyle.SetShowLabels( false );
        Set( "SectionFloatingToolBar", SectionToolBarStyle );

        SectionToolBarStyle.SetIconSize( FVector2D( 24, 16 ) );
        Set( "SectionFloatingToolBarLarge", SectionToolBarStyle );

        //-

        FToolBarStyle SectionTitleToolBarStyle = FStarshipCoreStyle::GetCoreStyle().GetWidgetStyle<FToolBarStyle>( "SlimToolBar" );

        SectionTitleToolBarStyle.SetBackground( FSlateNoResource() );
        SectionTitleToolBarStyle.SetButtonPadding( FMargin( 2, 0 ) );
        SectionTitleToolBarStyle.SetBackgroundPadding( 0 );
        SectionTitleToolBarStyle.SetIconSize( Icon16x16 );
        SectionTitleToolBarStyle.SetButtonStyle( SectionTitleToolBarStyle.ButtonStyle
                                                 .SetNormalPadding( FMargin( 5, 2 ) )
                                                 .SetPressedPadding( FMargin( 5, 3, 5, 1 ) )
        );
        SectionTitleToolBarStyle.SetWrapButtonStyle( SectionTitleToolBarStyle.WrapButtonStyle
                                                     .SetWrapButtonPadding( FMargin( 0.0 ) )
                                                     .SetComboButtonStyle( SectionTitleToolBarStyle.ComboButtonStyle
                                                                           .SetButtonStyle( SectionTitleToolBarStyle.ComboButtonStyle.ButtonStyle
                                                                                            .SetNormal( FSlateNoResource() ) ) )
        );
        SectionTitleToolBarStyle.SetShowLabels( false );
        Set( "SectionTitleToolBar", SectionTitleToolBarStyle );
    }
}

void
FEposTracksEditorStyle::InitNotes()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    FLinearColor color( FSequencerSectionPainter::BlendColor( FColor( 90, 90, 150 ) ) ); // Same as the default one in UMovieSceneNoteTrack

    Set( "Notes.TableView.Row", FTableRowStyle( FAppStyle::Get().GetWidgetStyle<FTableRowStyle>( "TableView.Row" ) )
                                .SetEvenRowBackgroundBrush( FSlateColorBrush( color ) )
                                .SetOddRowBackgroundBrush( FSlateColorBrush( color * 1.33 ) ) ); // a little brighter
}


//---

//static
const FEposTracksEditorStyle&
FEposTracksEditorStyle::Get()
{
    if( !smSingleton.IsValid() )
        smSingleton = MakeShareable( new FEposTracksEditorStyle );

    return *smSingleton;
}
