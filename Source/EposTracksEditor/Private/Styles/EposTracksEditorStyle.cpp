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

    Set( "Sequencer.Tracks.CinematicBoard", new IMAGE_BRUSH( "IconCinematicBoardTrack_16x", Icon16x16 ) );

    Set( "EposTracksEditor.NewSectionWithBoardAtCurrentFrame", new IMAGE_BRUSH( "IconNewSectionWithBoardAtCurrentFrame_16x", Icon16x16 ) );
    Set( "EposTracksEditor.NewSectionWithShotAtCurrentFrame", new IMAGE_BRUSH( "IconNewSectionWithShotAtCurrentFrame_16x", Icon16x16 ) );

    Set( "EposTracksEditor.NewSectionWithBoardBeforeSection", new IMAGE_BRUSH( "IconNewSectionWithBoardAtCurrentFrame_16x", Icon16x16 ) );
    Set( "EposTracksEditor.NewSectionWithShotBeforeSection", new IMAGE_BRUSH( "IconNewSectionWithShotAtCurrentFrame_16x", Icon16x16 ) );

    Set( "EposTracksEditor.NewSectionWithBoardAfterSection", new IMAGE_BRUSH( "IconNewSectionWithBoardAtCurrentFrame_16x", Icon16x16 ) );
    Set( "EposTracksEditor.NewSectionWithShotAfterSection", new IMAGE_BRUSH( "IconNewSectionWithShotAtCurrentFrame_16x", Icon16x16 ) );

    //---

    Set( "EposTracksEditor.ArrangeShotsManually", new IMAGE_BRUSH( "IconArrangeShotsManually_16x", Icon16x16 ) );
    Set( "EposTracksEditor.ArrangeShotsOnOneRow", new IMAGE_BRUSH( "IconArrangeShotsOnOneRow_16x", Icon16x16 ) );
    Set( "EposTracksEditor.ArrangeShotsOnTwoRows", new IMAGE_BRUSH( "IconArrangeShotsOnTwoRows_16x", Icon16x16 ) );

    FCheckBoxStyle lighttable_style = FCheckBoxStyle()
        //.SetCheckBoxType( ESlateCheckBoxType::ToggleButton )
        .SetUncheckedImage( IMAGE_BRUSH( "LighttableOff_16x", Icon16x16 ) )
        .SetUncheckedHoveredImage( IMAGE_BRUSH( "LighttableOff_16x", Icon16x16 ) )
        .SetUncheckedPressedImage( IMAGE_BRUSH( "LighttableOff_16x", Icon16x16 ) )
        .SetCheckedImage( IMAGE_BRUSH( "LighttableOn_16x", Icon16x16 ) )
        .SetCheckedHoveredImage( IMAGE_BRUSH( "LighttableOn_16x", Icon16x16 ) )
        .SetCheckedPressedImage( IMAGE_BRUSH( "LighttableOn_16x", Icon16x16 ) );

    Set( "EposTracksEditor.Lighttable", lighttable_style );

    //---

    Set( "EposTracksEditor.CreateCamera", new IMAGE_BRUSH( "IconCreateCamera_24x", Icon24x24 ) );
    Set( "EposTracksEditor.CreatePlane", new IMAGE_BRUSH( "IconCreatePlane_24x", Icon24x24 ) );
    Set( "EposTracksEditor.DetachPlane", new IMAGE_BRUSH( "IconDetachPlane_24x", Icon16x16 ) );

    // Sequencer & Curve Editor Toolbar Style ( Grabs core default, copies brushes, changes separator and block spacing )
    Set( "EposSectionThumbnail.ToolBar.Background", new CORE_BOX_BRUSH( "Common/GroupBorder", FMargin( 4.0f / 16.0f ) ) );
    Set( "EposSectionThumbnail.ToolBar.Icon", new CORE_IMAGE_BRUSH( "Icons/icon_tab_toolbar_16px", Icon16x16 ) );
    Set( "EposSectionThumbnail.ToolBar.Expand", new CORE_IMAGE_BRUSH( "Icons/toolbar_expand_16x", Icon16x16 ) );
    Set( "EposSectionThumbnail.ToolBar.SubMenuIndicator", new CORE_IMAGE_BRUSH( "Common/SubmenuArrow", Icon8x8 ) );
    Set( "EposSectionThumbnail.ToolBar.SToolBarComboButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarComboButtonBlock.Padding" ) );
    Set( "EposSectionThumbnail.ToolBar.SToolBarButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.Padding" ) );
    Set( "EposSectionThumbnail.ToolBar.SToolBarCheckComboButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarCheckComboButtonBlock.Padding" ) );
    Set( "EposSectionThumbnail.ToolBar.SToolBarButtonBlock.CheckBox.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.CheckBox.Padding" ) );
    Set( "EposSectionThumbnail.ToolBar.SToolBarComboButtonBlock.ComboButton.Color", FEditorStyle::Get().GetSlateColor( "Sequencer.ToolBar.SToolBarComboButtonBlock.ComboButton.Color" ) );

    // Used only for WidgetBlocks
    Set( "EposSectionThumbnail.ToolBar.Block.IndentedPadding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Block.IndentedPadding" ) );
    Set( "EposSectionThumbnail.ToolBar.Block.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Block.Padding" ) );

    Set( "EposSectionThumbnail.ToolBar.Separator", FEditorStyle::Get().GetSlateColor( "Sequencer.ToolBar.Separator" ) );
    Set( "EposSectionThumbnail.ToolBar.Separator.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Separator.Padding" ) );

    Set( "EposSectionThumbnail.ToolBar.Label", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Label" ) );
    Set( "EposSectionThumbnail.ToolBar.EditableText", FEditorStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>( "Sequencer.ToolBar.EditableText" ) );
    Set( "EposSectionThumbnail.ToolBar.Keybinding", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Keybinding" ) );
    Set( "EposSectionThumbnail.ToolBar.Heading", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Heading" ) );
    Set( "EposSectionThumbnail.ToolBar.CheckBox", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.CheckBox" ) );
    Set( "EposSectionThumbnail.Toolbar.Check", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.Check" ) );
    Set( "EposSectionThumbnail.ToolBar.RadioButton", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.RadioButton" ) );
    Set( "EposSectionThumbnail.ToolBar.ToggleButton", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.ToggleButton" ) );
    Set( "EposSectionThumbnail.ToolBar.Button", FEditorStyle::Get().GetWidgetStyle<FButtonStyle>( "Sequencer.ToolBar.Button" ) );

    //-

    // Sequencer & Curve Editor Toolbar Style ( Grabs core default, copies brushes, changes separator and block spacing )
    Set( "EposSectionPlane.ToolBar.Background", new CORE_BOX_BRUSH( "Common/GroupBorder", FMargin( 4.0f / 16.0f ) ) );
    Set( "EposSectionPlane.ToolBar.Icon", new CORE_IMAGE_BRUSH( "Icons/icon_tab_toolbar_16px", Icon16x16 ) );
    Set( "EposSectionPlane.ToolBar.Expand", new CORE_IMAGE_BRUSH( "Icons/toolbar_expand_16x", Icon16x16 ) );
    Set( "EposSectionPlane.ToolBar.SubMenuIndicator", new CORE_IMAGE_BRUSH( "Common/SubmenuArrow", Icon8x8 ) );
    Set( "EposSectionPlane.ToolBar.SToolBarComboButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarComboButtonBlock.Padding" ) );
    Set( "EposSectionPlane.ToolBar.SToolBarButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.Padding" ) );
    Set( "EposSectionPlane.ToolBar.SToolBarCheckComboButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarCheckComboButtonBlock.Padding" ) );
    Set( "EposSectionPlane.ToolBar.SToolBarButtonBlock.CheckBox.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.CheckBox.Padding" ) );
    Set( "EposSectionPlane.ToolBar.SToolBarComboButtonBlock.ComboButton.Color", FEditorStyle::Get().GetSlateColor( "Sequencer.ToolBar.SToolBarComboButtonBlock.ComboButton.Color" ) );

    // Used only for WidgetBlocks
    Set( "EposSectionPlane.ToolBar.Block.IndentedPadding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Block.IndentedPadding" ) );
    Set( "EposSectionPlane.ToolBar.Block.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Block.Padding" ) );

    Set( "EposSectionPlane.ToolBar.Separator", FEditorStyle::Get().GetSlateColor( "Sequencer.ToolBar.Separator" ) );
    Set( "EposSectionPlane.ToolBar.Separator.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Separator.Padding" ) );

    Set( "EposSectionPlane.ToolBar.Label", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Label" ) );
    Set( "EposSectionPlane.ToolBar.EditableText", FEditorStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>( "Sequencer.ToolBar.EditableText" ) );
    Set( "EposSectionPlane.ToolBar.Keybinding", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Keybinding" ) );
    Set( "EposSectionPlane.ToolBar.Heading", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Heading" ) );
    Set( "EposSectionPlane.ToolBar.CheckBox", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.CheckBox" ) );
    Set( "EposSectionPlane.Toolbar.Check", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.Check" ) );
    Set( "EposSectionPlane.ToolBar.RadioButton", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.RadioButton" ) );
    Set( "EposSectionPlane.ToolBar.ToggleButton", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.ToggleButton" ) );
    Set( "EposSectionPlane.ToolBar.Button", FEditorStyle::Get().GetWidgetStyle<FButtonStyle>( "Sequencer.ToolBar.Button" ) );

    //-

    // Sequencer & Curve Editor Toolbar Style ( Grabs core default, copies brushes, changes separator and block spacing )
    Set( "EposSectionPlanesFooter.ToolBar.Background", new CORE_BOX_BRUSH( "Common/GroupBorder", FMargin( 4.0f / 16.0f ) ) );
    Set( "EposSectionPlanesFooter.ToolBar.Icon", new CORE_IMAGE_BRUSH( "Icons/icon_tab_toolbar_16px", Icon16x16 ) );
    Set( "EposSectionPlanesFooter.ToolBar.Expand", new CORE_IMAGE_BRUSH( "Icons/toolbar_expand_16x", Icon16x16 ) );
    Set( "EposSectionPlanesFooter.ToolBar.SubMenuIndicator", new CORE_IMAGE_BRUSH( "Common/SubmenuArrow", Icon8x8 ) );
    Set( "EposSectionPlanesFooter.ToolBar.SToolBarComboButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarComboButtonBlock.Padding" ) );
    Set( "EposSectionPlanesFooter.ToolBar.SToolBarButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.Padding" ) );
    Set( "EposSectionPlanesFooter.ToolBar.SToolBarCheckComboButtonBlock.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarCheckComboButtonBlock.Padding" ) );
    Set( "EposSectionPlanesFooter.ToolBar.SToolBarButtonBlock.CheckBox.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.SToolBarButtonBlock.CheckBox.Padding" ) );
    Set( "EposSectionPlanesFooter.ToolBar.SToolBarComboButtonBlock.ComboButton.Color", FEditorStyle::Get().GetSlateColor( "Sequencer.ToolBar.SToolBarComboButtonBlock.ComboButton.Color" ) );

    // Used only for WidgetBlocks
    Set( "EposSectionPlanesFooter.ToolBar.Block.IndentedPadding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Block.IndentedPadding" ) );
    Set( "EposSectionPlanesFooter.ToolBar.Block.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Block.Padding" ) );

    Set( "EposSectionPlanesFooter.ToolBar.Separator", FEditorStyle::Get().GetSlateColor( "Sequencer.ToolBar.Separator" ) );
    Set( "EposSectionPlanesFooter.ToolBar.Separator.Padding", FEditorStyle::Get().GetMargin( "Sequencer.ToolBar.Separator.Padding" ) );

    Set( "EposSectionPlanesFooter.ToolBar.Label", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Label" ) );
    Set( "EposSectionPlanesFooter.ToolBar.EditableText", FEditorStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>( "Sequencer.ToolBar.EditableText" ) );
    Set( "EposSectionPlanesFooter.ToolBar.Keybinding", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Keybinding" ) );
    Set( "EposSectionPlanesFooter.ToolBar.Heading", FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>( "Sequencer.ToolBar.Heading" ) );
    Set( "EposSectionPlanesFooter.ToolBar.CheckBox", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.CheckBox" ) );
    Set( "EposSectionPlanesFooter.Toolbar.Check", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.Check" ) );
    Set( "EposSectionPlanesFooter.ToolBar.RadioButton", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.RadioButton" ) );
    Set( "EposSectionPlanesFooter.ToolBar.ToggleButton", FEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "Sequencer.ToolBar.ToggleButton" ) );
    Set( "EposSectionPlanesFooter.ToolBar.Button", FEditorStyle::Get().GetWidgetStyle<FButtonStyle>( "Sequencer.ToolBar.Button" ) );

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
