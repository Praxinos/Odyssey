// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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
    InitSequencer();
    InitTracks();
    InitCommands();
    InitToolbar();
    InitMenu();
    InitNotes();
}

void
FEposTracksEditorStyle::InitSequencer()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Epos" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    Set( "Sequencer.Tracks.CinematicBoard", new IMAGE_BRUSH( "track-board-16x", Icon16x16 ) );
    SetContentRoot( FPaths::EngineContentDir() / TEXT( "Editor/Slate" ) );
    Set( "Sequencer.Tracks.SingleCameraCut", new IMAGE_BRUSH( "Sequencer/Dropdown_Icons/Icon_Camera_Cut_Track_16x", Icon16x16 ) ); // same as FEditorStyle::GetBrush( "Sequencer.Tracks.CameraCut" ) );
    SetContentRoot( IPluginManager::Get().FindPlugin( "Epos" )->GetBaseDir() / TEXT( "Resources" ) );
    Set( "Sequencer.Tracks.Note", new IMAGE_BRUSH( "note-16x", Icon16x16 ) );
}

void
FEposTracksEditorStyle::InitTracks()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Epos" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    Set( "PilotCamera", new IMAGE_BRUSH( "camera-pilot-16x", Icon16x16 ) );
    Set( "EjectCamera", new IMAGE_BRUSH( "camera-eject-16x", Icon16x16 ) );
    Set( "SnapCameraToViewport", new IMAGE_BRUSH( "camera-snap-to-viewport-24x", Icon16x16 ) );

    Set( "DetachPlane", new IMAGE_BRUSH( "plane-detach-24x", Icon16x16 ) );
    Set( "CreateDrawing", new IMAGE_BRUSH( "drawing-create-24x", Icon16x16 ) );

    Set( "LighttableOff", new IMAGE_BRUSH( "lighttable-off-16x", Icon16x16 ) );
    Set( "LighttableOn", new IMAGE_BRUSH( "lighttable-on-16x", Icon16x16 ) );

    Set( "Take", new IMAGE_BRUSH( "take-16x", Icon16x16 ) );

    Set( "NoteSettings", new IMAGE_BRUSH( "note-16x", Icon16x16 ) );

    //---

    SetContentRoot( FPaths::EngineContentDir() / TEXT( "Editor/Slate" ) );
    {
        Set( "HyperlinkSpinBox", FSpinBoxStyle( FEditorStyle::GetWidgetStyle<FSpinBoxStyle>( "Sequencer.HyperlinkSpinBox" ) )
             .SetBackgroundBrush( BORDER_BRUSH( "Old/HyperlinkDotted", FMargin( 0, 0, 0, 3 / 16.0f ), FSlateColor::UseForeground() ) )
             .SetForegroundColor( FSlateColor::UseForeground() )
        );
    }
}

void
FEposTracksEditorStyle::InitCommands()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Epos" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    Set( "EposTracksEditorCommands.NewSectionWithBoardAtCurrentFrame", new IMAGE_BRUSH( "sequence-board-16x", Icon16x16 ) );
    Set( "EposTracksEditorCommands.NewSectionWithShotAtCurrentFrame", new IMAGE_BRUSH( "sequence-shot-16x", Icon16x16 ) );

    Set( "EposTracksEditorCommands.ArrangeShotsManually", new IMAGE_BRUSH( "arrange-shots-manually-16x", Icon16x16 ) );
    Set( "EposTracksEditorCommands.ArrangeShotsOnOneRow", new IMAGE_BRUSH( "arrange-shots-on-one-row-16x", Icon16x16 ) );
    Set( "EposTracksEditorCommands.ArrangeShotsOnTwoRows", new IMAGE_BRUSH( "arrange-shots-on-two-rows-16x", Icon16x16 ) );
}

void
FEposTracksEditorStyle::InitToolbar()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Epos" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    SetCoreContentRoot( FPaths::EngineContentDir() / TEXT( "Slate" ) );
    {
        FToolBarStyle SectionToolBarStyle = FStarshipCoreStyle::GetCoreStyle().GetWidgetStyle<FToolBarStyle>( "SlimToolBar" );

        SectionToolBarStyle.SetBackground( FAppStyle::Get().GetWidgetStyle< FComboButtonStyle >( "ComboButton" ).ButtonStyle.Normal ); // To have the same background as the '+' button on each side
        SectionToolBarStyle.SetIconSize( Icon16x16 );
        SectionToolBarStyle.SetShowLabels( false );
        SectionToolBarStyle.SetSettingsComboButtonStyle( SectionToolBarStyle.SettingsComboButton.SetDownArrowImage( CORE_IMAGE_BRUSH_SVG( "Starship/Common/ellipsis-vertical-narrow", FVector2D( 4, 16 ) ) ) );
        Set( "SectionFloatingToolBar", SectionToolBarStyle );

        //-

        FToolBarStyle SectionTitleToolBarStyle = FStarshipCoreStyle::GetCoreStyle().GetWidgetStyle<FToolBarStyle>( "SlimToolBar" );

        SectionTitleToolBarStyle.SetBackground( FSlateNoResource() );
        SectionTitleToolBarStyle.SetButtonPadding( FMargin( 2, 0 ) );
        SectionTitleToolBarStyle.SetBackgroundPadding( FMargin( 0 ) );
        SectionTitleToolBarStyle.SetIconSize( Icon16x16 );
        SectionTitleToolBarStyle.SetSettingsComboButtonStyle( SectionTitleToolBarStyle.SettingsComboButton.SetDownArrowImage( CORE_IMAGE_BRUSH_SVG( "Starship/Common/ellipsis-vertical-narrow", FVector2D( 4, 16 ) ) ) );
        SectionTitleToolBarStyle.SetShowLabels( false );
        Set( "SectionTitleToolBar", SectionTitleToolBarStyle );
    }
}

void
FEposTracksEditorStyle::InitMenu()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Epos" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    SetContentRoot( FPaths::EngineContentDir() / TEXT( "Editor/Slate" ) );
    {
        Set( "Menu.Background", new BOX_BRUSH( "Old/Menu_Background", FMargin( 8.0f / 64.0f ) ) );
        Set( "Menu.Icon", new IMAGE_BRUSH( "Icons/icon_tab_toolbar_16px", Icon16x16 ) );
        Set( "Menu.Expand", new IMAGE_BRUSH( "Icons/toolbar_expand_16x", Icon16x16 ) );
        Set( "Menu.SubMenuIndicator", new IMAGE_BRUSH( "Common/SubmenuArrow", Icon8x8 ) );
        Set( "Menu.SToolBarComboButtonBlock.Padding", FEditorStyle::GetMargin( "Menu.SToolBarComboButtonBlock.Padding" ) );
        Set( "Menu.SToolBarButtonBlock.Padding", FEditorStyle::GetMargin( "Menu.SToolBarButtonBlock.Padding" ) );
        Set( "Menu.SToolBarCheckComboButtonBlock.Padding", FEditorStyle::GetMargin( "Menu.SToolBarCheckComboButtonBlock.Padding" ) );
        Set( "Menu.SToolBarButtonBlock.CheckBox.Padding", FEditorStyle::GetMargin( "Menu.SToolBarButtonBlock.CheckBox.Padding" ) );
        Set( "Menu.SToolBarComboButtonBlock.ComboButton.Color", FEditorStyle::GetSlateColor( "Menu.SToolBarComboButtonBlock.ComboButton.Color" ) );

        Set( "Menu.Block.IndentedPadding", FEditorStyle::GetMargin( "Menu.Block.IndentedPadding" ) );
        Set( "Menu.Block.Padding", FEditorStyle::GetMargin( "Menu.Block.Padding" ) );

        Set( "Menu.Separator", new BOX_BRUSH( "Old/Button", 4.0f / 32.0f ) );
        Set( "Menu.Separator.Padding", FEditorStyle::GetMargin( "Menu.Separator.Padding" ) );

        Set( "Menu.Label", FEditorStyle::GetWidgetStyle<FTextBlockStyle>( "Menu.Label" ) );
        Set( "Menu.Label.Padding", FEditorStyle::GetMargin( "Menu.Label.Padding" ) );
        Set( "Menu.Label.ContentPadding", FEditorStyle::GetMargin( "Menu.Label.ContentPadding" ) );
        Set( "Menu.EditableText", FEditorStyle::GetWidgetStyle<FEditableTextBoxStyle>( "Menu.EditableText" ) );
        Set( "Menu.Keybinding", FEditorStyle::GetWidgetStyle<FTextBlockStyle>( "Menu.Keybinding" ) );

        Set( "Menu.Heading", FEditorStyle::GetWidgetStyle<FTextBlockStyle>( "Menu.Heading" ) );

        Set( "Menu.CheckBox", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Menu.CheckBox" ) );
        Set( "Menu.Check", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Menu.Check" ) );
        Set( "Menu.RadioButton", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Menu.RadioButton" ) );
        Set( "Menu.ToggleButton", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Menu.ToggleButton" ) );
        Set( "Menu.Button", FEditorStyle::GetWidgetStyle<FButtonStyle>( "Menu.Button" ) );

        Set( "Menu.Button.Checked", new BOX_BRUSH( "Common/RoundedSelection_16x", 4.0f / 16.0f, FEditorStyle::GetSlateColor( "SelectionColor_Pressed" ) ) );
        Set( "Menu.Button.Checked_Hovered", new BOX_BRUSH( "Common/RoundedSelection_16x", 4.0f / 16.0f, FEditorStyle::GetSlateColor( "SelectionColor_Pressed" ) ) );
        Set( "Menu.Button.Checked_Pressed", new BOX_BRUSH( "Common/RoundedSelection_16x", 4.0f / 16.0f, FEditorStyle::GetSlateColor( "SelectionColor" ) ) );

        /* The style of a menu bar button when it has a sub menu open */
        Set( "Menu.Button.SubMenuOpen", new BORDER_BRUSH( "Common/Selection", FMargin( 4.f / 16.f ), FLinearColor( 0.10f, 0.10f, 0.10f ) ) );
    }
}

void
FEposTracksEditorStyle::InitNotes()
{
    SetContentRoot( IPluginManager::Get().FindPlugin( "Epos" )->GetBaseDir() / TEXT( "Resources" ) );

    //---

    FLinearColor color( FSequencerSectionPainter::BlendColor( FColor( 90, 90, 150 ) ) ); // Same as the default one in UMovieSceneNoteTrack

    Set( "Notes.TableView.Row", FTableRowStyle( FEditorStyle::GetWidgetStyle<FTableRowStyle>( "TableView.Row" ) )
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
