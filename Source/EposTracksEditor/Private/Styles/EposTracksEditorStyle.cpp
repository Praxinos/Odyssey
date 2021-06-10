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

//---

TSharedPtr<FEposTracksEditorStyle> FEposTracksEditorStyle::smSingleton;

//---

FEposTracksEditorStyle::FEposTracksEditorStyle()
    : FSlateStyleSet( "EposTracksEditorStyle" )
{
    const FVector2D Icon16x16( 16.0f, 16.0f );
    const FVector2D Icon24x24( 24.0f, 24.0f );
    const FVector2D Icon48x48( 48.0f, 48.0f );
    const FVector2D Icon64x64( 64.0f, 64.0f );

    TSharedPtr<IPlugin> epos_plugin = IPluginManager::Get().FindPlugin( "Epos" );
    check( epos_plugin.IsValid() );
    SetContentRoot( epos_plugin->GetBaseDir() / TEXT( "Resources" ) );

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
