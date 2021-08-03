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

#define CORE_IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( RootToCoreContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define CORE_BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( RootToCoreContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

//---

TSharedPtr<FEposTracksEditorStyle> FEposTracksEditorStyle::smSingleton;

//---

FEposTracksEditorStyle::FEposTracksEditorStyle()
    : FSlateStyleSet( "EposTracksEditorStyle" )
{
    const FVector2D Icon8x8( 8.0f, 8.0f );
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

    //---

    Set( "EposTracksEditorCommands.NewSectionWithBoardAtCurrentFrame", new IMAGE_BRUSH( "sequence-board-16x", Icon16x16 ) );
    Set( "EposTracksEditorCommands.NewSectionWithShotAtCurrentFrame", new IMAGE_BRUSH( "sequence-shot-16x", Icon16x16 ) );

    Set( "EposTracksEditorCommands.ArrangeShotsManually", new IMAGE_BRUSH( "arrange-shots-manually-16x", Icon16x16 ) );
    Set( "EposTracksEditorCommands.ArrangeShotsOnOneRow", new IMAGE_BRUSH( "arrange-shots-on-one-row-16x", Icon16x16 ) );
    Set( "EposTracksEditorCommands.ArrangeShotsOnTwoRows", new IMAGE_BRUSH( "arrange-shots-on-two-rows-16x", Icon16x16 ) );

    //---

    FCheckBoxStyle lighttable_style = FCheckBoxStyle()
        //.SetCheckBoxType( ESlateCheckBoxType::ToggleButton )
        .SetUncheckedImage( IMAGE_BRUSH( "lighttable-off-16x", Icon16x16 ) )
        .SetUncheckedHoveredImage( IMAGE_BRUSH( "lighttable-off-16x", Icon16x16 ) )
        .SetUncheckedPressedImage( IMAGE_BRUSH( "lighttable-off-16x", Icon16x16 ) )
        .SetCheckedImage( IMAGE_BRUSH( "lighttable-on-16x", Icon16x16 ) )
        .SetCheckedHoveredImage( IMAGE_BRUSH( "lighttable-on-16x", Icon16x16 ) )
        .SetCheckedPressedImage( IMAGE_BRUSH( "lighttable-on-16x", Icon16x16 ) );

    Set( "EposTracksEditor.Lighttable", lighttable_style );

    Set( "EposTracksEditor.NewSectionWithBoardBeforeSection", new IMAGE_BRUSH( "sequence-board-16x", Icon16x16 ) );
    Set( "EposTracksEditor.NewSectionWithShotBeforeSection", new IMAGE_BRUSH( "sequence-shot-16x", Icon16x16 ) );

    Set( "EposTracksEditor.NewSectionWithBoardAfterSection", new IMAGE_BRUSH( "sequence-board-16x", Icon16x16 ) );
    Set( "EposTracksEditor.NewSectionWithShotAfterSection", new IMAGE_BRUSH( "sequence-shot-16x", Icon16x16 ) );

    Set( "EposTracksEditor.CreateCamera", new IMAGE_BRUSH( "camera-create-24x", Icon24x24 ) );
    Set( "EposTracksEditor.CreatePlane", new IMAGE_BRUSH( "plane-create-24x", Icon24x24 ) );
    Set( "EposTracksEditor.DetachPlane", new IMAGE_BRUSH( "plane-detach-24x", Icon16x16 ) );
    Set( "EposTracksEditor.CreateDrawing", new IMAGE_BRUSH( "drawing-create-24x", Icon16x16 ) );
    Set( "EposTracksEditor.SnapCameraToViewport", new IMAGE_BRUSH( "camera-snap-to-viewport-24x", Icon16x16 ) );

    SetContentRoot( FPaths::EngineContentDir() / TEXT( "Editor/Slate" ) );
    Set( "EposTracksEditor.HyperlinkSpinBox", FSpinBoxStyle( FEditorStyle::Get().GetWidgetStyle<FSpinBoxStyle>( "Sequencer.HyperlinkSpinBox" ) )
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
    Set( "EposSection.ToolBar.SToolBarComboButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarComboButtonBlock.Padding" ) );
    Set( "EposSection.ToolBar.SToolBarButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.Padding" ) );
    Set( "EposSection.ToolBar.SToolBarCheckComboButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarCheckComboButtonBlock.Padding" ) );
    Set( "EposSection.ToolBar.SToolBarButtonBlock.CheckBox.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.CheckBox.Padding" ) );
    Set( "EposSection.ToolBar.SToolBarComboButtonBlock.ComboButton.Color", FEditorStyle::Get().GetSlateColor( "Sequencer.ToolBar.SToolBarComboButtonBlock.ComboButton.Color" ) );

    // Used only for WidgetBlocks
    Set( "EposSection.ToolBar.Block.IndentedPadding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Block.IndentedPadding" ) );
    Set( "EposSection.ToolBar.Block.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Block.Padding" ) );

    Set( "EposSection.ToolBar.Separator", FEditorStyle::Get().GetSlateColor( "Sequencer.ToolBar.Separator" ) );
    Set( "EposSection.ToolBar.Separator.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Separator.Padding" ) );

    Set( "EposSection.ToolBar.Label", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Label" ) );
    Set( "EposSection.ToolBar.EditableText", FEditorStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>( "Sequencer.ToolBar.EditableText" ) );
    Set( "EposSection.ToolBar.Keybinding", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Keybinding" ) );
    Set( "EposSection.ToolBar.Heading", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Heading" ) );
    Set( "EposSection.ToolBar.CheckBox", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.CheckBox" ) );
    Set( "EposSection.Toolbar.Check", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.Check" ) );
    Set( "EposSection.ToolBar.RadioButton", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.RadioButton" ) );
    Set( "EposSection.ToolBar.ToggleButton", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.ToggleButton" ) );
    Set( "EposSection.ToolBar.Button", FEditorStyle::Get().GetWidgetStyle<FButtonStyle>( "Sequencer.ToolBar.Button" ) );

    //-

    // Sequencer & Curve Editor Toolbar Style ( Grabs core default, copies brushes, changes separator and block spacing )
    Set( "EposSectionTitle.ToolBar.Background", new FSlateNoResource() );
    Set( "EposSectionTitle.ToolBar.Icon", new CORE_IMAGE_BRUSH( "Icons/icon_tab_toolbar_16px", Icon16x16 ) );
    Set( "EposSectionTitle.ToolBar.Expand", new CORE_IMAGE_BRUSH( "Icons/toolbar_expand_16x", Icon16x16 ) );
    Set( "EposSectionTitle.ToolBar.SubMenuIndicator", new CORE_IMAGE_BRUSH( "Common/SubmenuArrow", Icon8x8 ) );
    Set( "EposSectionTitle.ToolBar.SToolBarComboButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarComboButtonBlock.Padding" ) );
    Set( "EposSectionTitle.ToolBar.SToolBarButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.Padding" ) );
    Set( "EposSectionTitle.ToolBar.SToolBarCheckComboButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarCheckComboButtonBlock.Padding" ) );
    Set( "EposSectionTitle.ToolBar.SToolBarButtonBlock.CheckBox.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.CheckBox.Padding" ) );
    Set( "EposSectionTitle.ToolBar.SToolBarComboButtonBlock.ComboButton.Color", FEditorStyle::Get().GetSlateColor( "Sequencer.ToolBar.SToolBarComboButtonBlock.ComboButton.Color" ) );

    // Used only for WidgetBlocks
    Set( "EposSectionTitle.ToolBar.Block.IndentedPadding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Block.IndentedPadding" ) );
    Set( "EposSectionTitle.ToolBar.Block.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Block.Padding" ) );

    Set( "EposSectionTitle.ToolBar.Separator", FEditorStyle::Get().GetSlateColor( "Sequencer.ToolBar.Separator" ) );
    Set( "EposSectionTitle.ToolBar.Separator.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Separator.Padding" ) );

    Set( "EposSectionTitle.ToolBar.Label", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Label" ) );
    Set( "EposSectionTitle.ToolBar.EditableText", FEditorStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>( "Sequencer.ToolBar.EditableText" ) );
    Set( "EposSectionTitle.ToolBar.Keybinding", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Keybinding" ) );
    Set( "EposSectionTitle.ToolBar.Heading", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Heading" ) );
    Set( "EposSectionTitle.ToolBar.CheckBox", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.CheckBox" ) );
    Set( "EposSectionTitle.Toolbar.Check", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.Check" ) );
    Set( "EposSectionTitle.ToolBar.RadioButton", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.RadioButton" ) );
    Set( "EposSectionTitle.ToolBar.ToggleButton", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.ToggleButton" ) );
    Set( "EposSectionTitle.ToolBar.Button", FEditorStyle::Get().GetWidgetStyle<FButtonStyle>( "Sequencer.ToolBar.Button" ) );

    //---

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
