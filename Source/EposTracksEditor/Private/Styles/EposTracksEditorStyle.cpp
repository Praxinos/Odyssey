// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Styles/EposTracksEditorStyle.h"

#include "Brushes/SlateBorderBrush.h"
#include "Brushes/SlateBoxBrush.h"
#include "Brushes/SlateImageBrush.h"
#include "Fonts/SlateFontInfo.h"
#include "Interfaces/IPluginManager.h"
#include "Layout/Margin.h"
#include "Misc/Paths.h"
#include "Styling/SlateStyleRegistry.h"

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

#define CORE_IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( RootToCoreContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define CORE_BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( RootToCoreContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

//---

TSharedPtr<FEposTracksEditorStyle> FEposTracksEditorStyle::smSingleton;

//---

FEposTracksEditorStyle::FEposTracksEditorStyle()
    : FSlateStyleSet( "EposTracksEditorStyle" )
{
    const FVector2D Icon8x8( 8.0f, 8.0f );
    const FVector2D Icon14x14( 14.0f, 14.0f );
    const FVector2D Icon16x16( 16.0f, 16.0f );
    const FVector2D Icon24x24( 24.0f, 24.0f );
    const FVector2D Icon48x48( 48.0f, 48.0f );
    const FVector2D Icon64x64( 64.0f, 64.0f );

    TSharedPtr<IPlugin> epos_plugin = IPluginManager::Get().FindPlugin( "Epos" );
    check( epos_plugin.IsValid() );
    SetContentRoot( epos_plugin->GetBaseDir() / TEXT( "Resources" ) );
    SetCoreContentRoot( FPaths::EngineContentDir() / TEXT( "Slate" ) );

    //---

    Set( "Sequencer.Tracks.CinematicBoard", new IMAGE_BRUSH( "track-board-16x", Icon16x16 ) );
    Set( "Sequencer.Tracks.Note", new IMAGE_BRUSH( "note-16x", Icon16x16 ) );

    //---

    Set( "EposTracksEditorCommands.NewSectionWithBoardAtCurrentFrame", new IMAGE_BRUSH( "sequence-board-16x", Icon16x16 ) );
    Set( "EposTracksEditorCommands.NewSectionWithShotAtCurrentFrame", new IMAGE_BRUSH( "sequence-shot-16x", Icon16x16 ) );

    Set( "EposTracksEditorCommands.ArrangeShotsManually", new IMAGE_BRUSH( "arrange-shots-manually-16x", Icon16x16 ) );
    Set( "EposTracksEditorCommands.ArrangeShotsOnOneRow", new IMAGE_BRUSH( "arrange-shots-on-one-row-16x", Icon16x16 ) );
    Set( "EposTracksEditorCommands.ArrangeShotsOnTwoRows", new IMAGE_BRUSH( "arrange-shots-on-two-rows-16x", Icon16x16 ) );

    //---

    Set( "EposTracksEditor.LighttableOff", new IMAGE_BRUSH( "lighttable-off-16x", Icon16x16 ) );
    Set( "EposTracksEditor.LighttableOn", new IMAGE_BRUSH( "lighttable-on-16x", Icon16x16 ) );

    Set( "EposTracksEditor.NewSectionWithBoardBeforeSection", new IMAGE_BRUSH( "sequence-board-16x", Icon16x16 ) );
    Set( "EposTracksEditor.NewSectionWithShotBeforeSection", new IMAGE_BRUSH( "sequence-shot-16x", Icon16x16 ) );

    Set( "EposTracksEditor.NewSectionWithBoardAfterSection", new IMAGE_BRUSH( "sequence-board-16x", Icon16x16 ) );
    Set( "EposTracksEditor.NewSectionWithShotAfterSection", new IMAGE_BRUSH( "sequence-shot-16x", Icon16x16 ) );

    Set( "EposTracksEditor.CreateCamera", new IMAGE_BRUSH( "camera-create-24x", Icon24x24 ) );
    Set( "EposTracksEditor.PilotCamera", new IMAGE_BRUSH( "camera-pilot-16x", Icon16x16 ) );
    Set( "EposTracksEditor.EjectCamera", new IMAGE_BRUSH( "camera-eject-16x", Icon16x16 ) );
    Set( "EposTracksEditor.CreatePlane", new IMAGE_BRUSH( "plane-create-24x", Icon24x24 ) );
    Set( "EposTracksEditor.DetachPlane", new IMAGE_BRUSH( "plane-detach-24x", Icon16x16 ) );
    Set( "EposTracksEditor.CreateDrawing", new IMAGE_BRUSH( "drawing-create-24x", Icon16x16 ) );
    Set( "EposTracksEditor.SnapCameraToViewport", new IMAGE_BRUSH( "camera-snap-to-viewport-24x", Icon16x16 ) );

    SetContentRoot( FPaths::EngineContentDir() / TEXT( "Editor/Slate" ) );
    Set( "EposTracksEditor.HyperlinkSpinBox", FSpinBoxStyle( FEditorStyle::GetWidgetStyle<FSpinBoxStyle>( "Sequencer.HyperlinkSpinBox" ) )
         .SetBackgroundBrush( BORDER_BRUSH( "Old/HyperlinkDotted", FMargin( 0, 0, 0, 3 / 16.0f ), FSlateColor::UseForeground() ) )
         .SetForegroundColor( FSlateColor::UseForeground() )
    );
    SetContentRoot( epos_plugin->GetBaseDir() / TEXT( "Resources" ) );

    //---

    // Sequencer & Curve Editor Toolbar Style ( Grabs core default, copies brushes, changes separator and block spacing )
    Set( "EposSection.ToolBar.Background", new CORE_BOX_BRUSH( "Common/GroupBorder", FMargin( 4.0f / 16.0f ) ) );
    Set( "EposSection.ToolBar.Icon", new CORE_IMAGE_BRUSH( "Icons/icon_tab_toolbar_16px", Icon16x16 ) );
    Set( "EposSection.ToolBar.Expand", new CORE_IMAGE_BRUSH( "Icons/toolbar_expand_16x", Icon16x16 ) );
    Set( "EposSection.ToolBar.SubMenuIndicator", new CORE_IMAGE_BRUSH( "Common/SubmenuArrow", Icon8x8 ) );
    Set( "EposSection.ToolBar.SToolBarComboButtonBlock.Padding", FEditorStyle::GetMargin( "Sequencer.ToolBar.SToolBarComboButtonBlock.Padding" ) );
    Set( "EposSection.ToolBar.SToolBarButtonBlock.Padding", FEditorStyle::GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.Padding" ) );
    Set( "EposSection.ToolBar.SToolBarCheckComboButtonBlock.Padding", FEditorStyle::GetMargin( "Sequencer.ToolBar.SToolBarCheckComboButtonBlock.Padding" ) );
    Set( "EposSection.ToolBar.SToolBarButtonBlock.CheckBox.Padding", FEditorStyle::GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.CheckBox.Padding" ) );
    Set( "EposSection.ToolBar.SToolBarComboButtonBlock.ComboButton.Color", FEditorStyle::GetSlateColor( "Sequencer.ToolBar.SToolBarComboButtonBlock.ComboButton.Color" ) );

    // Used only for WidgetBlocks
    Set( "EposSection.ToolBar.Block.IndentedPadding", FEditorStyle::GetMargin( "Sequencer.ToolBar.Block.IndentedPadding" ) );
    Set( "EposSection.ToolBar.Block.Padding", FEditorStyle::GetMargin( "Sequencer.ToolBar.Block.Padding" ) );

    Set( "EposSection.ToolBar.Separator", FEditorStyle::GetSlateColor( "Sequencer.ToolBar.Separator" ) );
    Set( "EposSection.ToolBar.Separator.Padding", FEditorStyle::GetMargin( "Sequencer.ToolBar.Separator.Padding" ) );

    Set( "EposSection.ToolBar.Label", FEditorStyle::GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Label" ) );
    Set( "EposSection.ToolBar.EditableText", FEditorStyle::GetWidgetStyle<FEditableTextBoxStyle>( "Sequencer.ToolBar.EditableText" ) );
    Set( "EposSection.ToolBar.Keybinding", FEditorStyle::GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Keybinding" ) );
    Set( "EposSection.ToolBar.Heading", FEditorStyle::GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Heading" ) );
    Set( "EposSection.ToolBar.CheckBox", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.CheckBox" ) );
    Set( "EposSection.Toolbar.Check", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.Check" ) );
    Set( "EposSection.ToolBar.RadioButton", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.RadioButton" ) );
    Set( "EposSection.ToolBar.ToggleButton", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.ToggleButton" ) );
    Set( "EposSection.ToolBar.Button", FEditorStyle::GetWidgetStyle<FButtonStyle>( "Sequencer.ToolBar.Button" ) );

    //-

    // Sequencer & Curve Editor Toolbar Style ( Grabs core default, copies brushes, changes separator and block spacing )
    Set( "EposSectionTitle.ToolBar.Background", new FSlateNoResource() );
    Set( "EposSectionTitle.ToolBar.Icon", new CORE_IMAGE_BRUSH( "Icons/icon_tab_toolbar_16px", Icon16x16 ) );
    Set( "EposSectionTitle.ToolBar.Expand", new CORE_IMAGE_BRUSH( "Icons/toolbar_expand_16x", Icon16x16 ) );
    Set( "EposSectionTitle.ToolBar.SubMenuIndicator", new CORE_IMAGE_BRUSH( "Common/SubmenuArrow", Icon8x8 ) );
    Set( "EposSectionTitle.ToolBar.SToolBarComboButtonBlock.Padding", FEditorStyle::GetMargin( "Sequencer.ToolBar.SToolBarComboButtonBlock.Padding" ) );
    Set( "EposSectionTitle.ToolBar.SToolBarButtonBlock.Padding", FEditorStyle::GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.Padding" ) );
    Set( "EposSectionTitle.ToolBar.SToolBarCheckComboButtonBlock.Padding", FEditorStyle::GetMargin( "Sequencer.ToolBar.SToolBarCheckComboButtonBlock.Padding" ) );
    Set( "EposSectionTitle.ToolBar.SToolBarButtonBlock.CheckBox.Padding", FEditorStyle::GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.CheckBox.Padding" ) );
    Set( "EposSectionTitle.ToolBar.SToolBarComboButtonBlock.ComboButton.Color", FEditorStyle::GetSlateColor( "Sequencer.ToolBar.SToolBarComboButtonBlock.ComboButton.Color" ) );

    // Used only for WidgetBlocks
    Set( "EposSectionTitle.ToolBar.Block.IndentedPadding", FEditorStyle::GetMargin( "Sequencer.ToolBar.Block.IndentedPadding" ) );
    Set( "EposSectionTitle.ToolBar.Block.Padding", FEditorStyle::GetMargin( "Sequencer.ToolBar.Block.Padding" ) );

    Set( "EposSectionTitle.ToolBar.Separator", FEditorStyle::GetSlateColor( "Sequencer.ToolBar.Separator" ) );
    Set( "EposSectionTitle.ToolBar.Separator.Padding", FEditorStyle::GetMargin( "Sequencer.ToolBar.Separator.Padding" ) );

    Set( "EposSectionTitle.ToolBar.Label", FEditorStyle::GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Label" ) );
    Set( "EposSectionTitle.ToolBar.EditableText", FEditorStyle::GetWidgetStyle<FEditableTextBoxStyle>( "Sequencer.ToolBar.EditableText" ) );
    Set( "EposSectionTitle.ToolBar.Keybinding", FEditorStyle::GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Keybinding" ) );
    Set( "EposSectionTitle.ToolBar.Heading", FEditorStyle::GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Heading" ) );
    Set( "EposSectionTitle.ToolBar.CheckBox", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.CheckBox" ) );
    Set( "EposSectionTitle.Toolbar.Check", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.Check" ) );
    Set( "EposSectionTitle.ToolBar.RadioButton", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.RadioButton" ) );
    Set( "EposSectionTitle.ToolBar.ToggleButton", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.ToggleButton" ) );
    Set( "EposSectionTitle.ToolBar.Button", FEditorStyle::GetWidgetStyle<FButtonStyle>( "Sequencer.ToolBar.Button" ) );

    //---

    // MenuBar
    {
        SetContentRoot( FPaths::EngineContentDir() / TEXT( "Editor/Slate" ) );

        Set("Menu.Background", new BOX_BRUSH("Old/Menu_Background", FMargin(8.0f / 64.0f)));
        Set("Menu.Icon", new IMAGE_BRUSH("Icons/icon_tab_toolbar_16px", Icon16x16));
        Set("Menu.Expand", new IMAGE_BRUSH("Icons/toolbar_expand_16x", Icon16x16));
        Set("Menu.SubMenuIndicator", new IMAGE_BRUSH("Common/SubmenuArrow", Icon8x8));
        Set("Menu.SToolBarComboButtonBlock.Padding", FEditorStyle::GetMargin( "Menu.SToolBarComboButtonBlock.Padding" ) );
        Set("Menu.SToolBarButtonBlock.Padding", FEditorStyle::GetMargin( "Menu.SToolBarButtonBlock.Padding" ) );
        Set("Menu.SToolBarCheckComboButtonBlock.Padding", FEditorStyle::GetMargin( "Menu.SToolBarCheckComboButtonBlock.Padding" ) );
        Set("Menu.SToolBarButtonBlock.CheckBox.Padding", FEditorStyle::GetMargin( "Menu.SToolBarButtonBlock.CheckBox.Padding" ) );
        Set("Menu.SToolBarComboButtonBlock.ComboButton.Color", FEditorStyle::GetSlateColor( "Menu.SToolBarComboButtonBlock.ComboButton.Color" ) );

        Set("Menu.Block.IndentedPadding", FEditorStyle::GetMargin( "Menu.Block.IndentedPadding" ) );
        Set("Menu.Block.Padding", FEditorStyle::GetMargin( "Menu.Block.Padding" ) );

        Set("Menu.Separator", new BOX_BRUSH("Old/Button", 4.0f / 32.0f));
        Set("Menu.Separator.Padding", FEditorStyle::GetMargin( "Menu.Separator.Padding" ) );

        Set("Menu.Label", FEditorStyle::GetWidgetStyle<FTextBlockStyle>( "Menu.Label" ) );
        Set("Menu.Label.Padding", FEditorStyle::GetMargin( "Menu.Label.Padding" ) );
        Set("Menu.Label.ContentPadding", FEditorStyle::GetMargin( "Menu.Label.ContentPadding" ) );
        Set("Menu.EditableText", FEditorStyle::GetWidgetStyle<FEditableTextBoxStyle>( "Menu.EditableText" ) );
        Set("Menu.Keybinding", FEditorStyle::GetWidgetStyle<FTextBlockStyle>( "Menu.Keybinding" ) );

        Set("Menu.Heading", FEditorStyle::GetWidgetStyle<FTextBlockStyle>( "Menu.Heading" ) );

        Set("Menu.CheckBox", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Menu.CheckBox" ) );
        Set("Menu.Check", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Menu.Check" ) );
        Set("Menu.RadioButton", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Menu.RadioButton" ) );
        Set("Menu.ToggleButton", FEditorStyle::GetWidgetStyle<FCheckBoxStyle>( "Menu.ToggleButton" ) );
        Set("Menu.Button", FEditorStyle::GetWidgetStyle<FButtonStyle>( "Menu.Button" ) );

        Set("Menu.Button.Checked", new BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, FEditorStyle::GetSlateColor( "SelectionColor_Pressed" ) ));
        Set("Menu.Button.Checked_Hovered", new BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, FEditorStyle::GetSlateColor( "SelectionColor_Pressed" ) ));
        Set("Menu.Button.Checked_Pressed", new BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, FEditorStyle::GetSlateColor( "SelectionColor" ) ));

        /* The style of a menu bar button when it has a sub menu open */
        Set("Menu.Button.SubMenuOpen", new BORDER_BRUSH("Common/Selection", FMargin(4.f / 16.f), FLinearColor(0.10f, 0.10f, 0.10f)));

        SetContentRoot( epos_plugin->GetBaseDir() / TEXT( "Resources" ) );
    }

    //---

    Set( "EposNotes.TableView.Row", FEditorStyle::GetWidgetStyle<FTableRowStyle>( "TableView.Row" ) );

    FSlateStyleRegistry::RegisterSlateStyle( *this );
}

FEposTracksEditorStyle::~FEposTracksEditorStyle()
{
    FSlateStyleRegistry::UnRegisterSlateStyle( *this );
}

//---

//static
TSharedRef<FEposTracksEditorStyle>
FEposTracksEditorStyle::Get()
{
    if( !smSingleton.IsValid() )
        smSingleton = MakeShareable( new FEposTracksEditorStyle );

    return smSingleton.ToSharedRef();
}

//---

#undef IMAGE_BRUSH
