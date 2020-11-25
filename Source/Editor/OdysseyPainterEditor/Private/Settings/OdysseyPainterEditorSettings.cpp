// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorSettings.h"

#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "EngineUtils.h"

FBrushDefaults::FBrushDefaults()
    : DefaultBrush( nullptr )
{
    DefaultBrush = LoadObject<UOdysseyBrush>(nullptr, TEXT("/Iliad/Brushes/Drawing_Tools/Penbrush1.Penbrush1"));
}

UOdysseyPainterEditorSettings::UOdysseyPainterEditorSettings( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
    , Background( kOdysseyPainterEditorBackground_Checkered )
    , BackgroundColor( FColor( 127, 127, 127 ) )
    , CheckerColorOne( FColor( 166, 166, 166 ) )
    , CheckerColorTwo( FColor( 134, 134, 134 ) )
    , CheckerSize( 10 )
    , Rotation( EKeys::R )
    , Pan( EKeys::P )
    , PickColor( EKeys::LeftAlt )
    , FitToViewport( true )
    , TextureBorderColor( FColor::White )
    , TextureBorderEnabled( false )
    , BrushDefaults()
{
}
