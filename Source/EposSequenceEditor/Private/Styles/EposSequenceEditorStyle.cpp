// IDDN FR.001.250001.004.S.X.2019.000.00000
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

    // tab icons
    Set( "EposSequenceEditor.Tabs.Sequencer", new IMAGE_BRUSH( "SequencerTab_16x", Icon16x16 ) ); // Seems to be only in toolkit standalone mode, but the plugin seems to always be in world-centric mode

    // asset thumbnail
    Set( "ClassIcon.BoardSequence", new IMAGE_BRUSH( "BoardSequence_16x", Icon16x16 ) );
    Set( "ClassThumbnail.BoardSequence", new IMAGE_BRUSH( "BoardSequence_64x", Icon64x64 ) );
    Set( "ClassIcon.ShotSequence", new IMAGE_BRUSH( "ShotSequence_16x", Icon16x16 ) );
    Set( "ClassThumbnail.ShotSequence", new IMAGE_BRUSH( "ShotSequence_64x", Icon64x64 ) );

    Set( "BoardSequenceEditor.NewStoryboardWithSettings", new IMAGE_BRUSH( "NewStoryboardWithSettings", Icon16x16 ) );
    Set( "BoardSequenceEditor.NewStoryboardWithSettings.Small", new IMAGE_BRUSH( "NewStoryboardWithSettings", Icon16x16 ) );

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

    Set( "BoardSequenceEditor.NewSectionWithBoardAtCurrentFrame", new IMAGE_BRUSH( "IconNewSectionWithBoardAtCurrentFrame_24x", Icon48x48 ) );
    Set( "BoardSequenceEditor.NewSectionWithBoardAtCurrentFrame.Small", new IMAGE_BRUSH( "IconNewSectionWithBoardAtCurrentFrame_24x", Icon24x24 ) );
    Set( "BoardSequenceEditor.NewSectionWithShotAtCurrentFrame", new IMAGE_BRUSH( "IconNewSectionWithShotAtCurrentFrame_24x", Icon48x48 ) );
    Set( "BoardSequenceEditor.NewSectionWithShotAtCurrentFrame.Small", new IMAGE_BRUSH( "IconNewSectionWithShotAtCurrentFrame_24x", Icon24x24 ) );

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

//---

#undef IMAGE_BRUSH
