// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "Styles/EposEditorStyle.h"

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

TSharedPtr<FEposEditorStyle> FEposEditorStyle::smSingleton;

//---

FEposEditorStyle::FEposEditorStyle()
    : FSlateStyleSet( "EposEditorStyle" )
{
    const FVector2D Icon16x16( 16.0f, 16.0f );
    const FVector2D Icon24x24( 24.0f, 24.0f );
    const FVector2D Icon48x48( 48.0f, 48.0f );
    const FVector2D Icon64x64( 64.0f, 64.0f );

    TSharedPtr<IPlugin> epos_plugin = IPluginManager::Get().FindPlugin( "Epos" );
    check( epos_plugin.IsValid() );
    SetContentRoot( epos_plugin->GetBaseDir() / TEXT( "Resources" ) );

    // tab icons
    Set( "EposEditor.Tabs.Sequencer", new IMAGE_BRUSH( "SequencerTab_16x", Icon16x16 ) ); // Seems to be only in toolkit standalone mode, but the plugin seems to always be in world-centric mode

    // asset thumbnail
    Set( "ClassIcon.BoardSequence", new IMAGE_BRUSH( "BoardSequence_16x", Icon16x16 ) );
    Set( "ClassThumbnail.BoardSequence", new IMAGE_BRUSH( "BoardSequence_64x", Icon64x64 ) );
    Set( "ClassIcon.ShotSequence", new IMAGE_BRUSH( "ShotSequence_16x", Icon16x16 ) );
    Set( "ClassThumbnail.ShotSequence", new IMAGE_BRUSH( "ShotSequence_64x", Icon64x64 ) );

    Set( "Sequencer.Tracks.CinematicBoard", new IMAGE_BRUSH( "IconCinematicBoardTrack_16x", Icon16x16 ) );

    Set( "ShotSequenceEditor.CreateCamera", new IMAGE_BRUSH( "IconCreateCamera_24x", Icon48x48 ) );
    Set( "ShotSequenceEditor.CreateCamera.Small", new IMAGE_BRUSH( "IconCreateCamera_24x", Icon24x24 ) );
    Set( "ShotSequenceEditor.CreatePlane", new IMAGE_BRUSH( "IconCreatePlane_24x", Icon48x48 ) );
    Set( "ShotSequenceEditor.CreatePlane.Small", new IMAGE_BRUSH( "IconCreatePlane_24x", Icon24x24 ) );
    Set( "ShotSequenceEditor.SnapCameraToViewport", new IMAGE_BRUSH( "IconSnapCameraToViewport_24x", Icon48x48 ) );
    Set( "ShotSequenceEditor.SnapCameraToViewport.Small", new IMAGE_BRUSH( "IconSnapCameraToViewport_24x", Icon24x24 ) );

    Set( "BoardSequenceEditor.ArrangeShotsManually", new IMAGE_BRUSH( "IconArrangeShotsManually_24x", Icon48x48 ) );
    Set( "BoardSequenceEditor.ArrangeShotsManually.Small", new IMAGE_BRUSH( "IconArrangeShotsManually_24x", Icon24x24 ) );
    Set( "BoardSequenceEditor.ArrangeShotsOnOneRow", new IMAGE_BRUSH( "IconArrangeShotsOnOneRow_24x", Icon48x48 ) );
    Set( "BoardSequenceEditor.ArrangeShotsOnOneRow.Small", new IMAGE_BRUSH( "IconArrangeShotsOnOneRow_24x", Icon24x24 ) );
    Set( "BoardSequenceEditor.ArrangeShotsOnTwoRows", new IMAGE_BRUSH( "IconArrangeShotsOnTwoRows_24x", Icon48x48 ) );          // inside popup (and maybe big toolbar ?) but Icon..x.. is not used ?
    Set( "BoardSequenceEditor.ArrangeShotsOnTwoRows.Small", new IMAGE_BRUSH( "IconArrangeShotsOnTwoRows_24x", Icon24x24 ) );    // in the toolbar, stretch to the size Icon..x..

    FSlateStyleRegistry::RegisterSlateStyle( *this );
}

FEposEditorStyle::~FEposEditorStyle()
{
    FSlateStyleRegistry::UnRegisterSlateStyle( *this );
}

//---

//static
TSharedRef<FEposEditorStyle>
FEposEditorStyle::Get()
{
    if( !smSingleton.IsValid() )
        smSingleton = MakeShareable( new FEposEditorStyle );

    return smSingleton.ToSharedRef();
}

//---

#undef IMAGE_BRUSH
