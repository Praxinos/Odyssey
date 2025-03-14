// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorSettings.h"

#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "EngineUtils.h"

FBrushDefaults::FBrushDefaults()
    : DefaultBrush(FSoftObjectPath(TEXT("/Odyssey/Brushes/Bitmap_Tools/OB_Penbrush1.OB_Penbrush1")))
{
}

UOdysseyPainterEditorSettings::UOdysseyPainterEditorSettings( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
    , Background( kOdysseyPainterEditorBackground_Checkered )
    , BackgroundColor( FColor( 127, 127, 127 ) )
    , CheckerColorOne( FColor( 255, 255, 255 ) )
    , CheckerColorTwo( FColor( 247, 247, 247 ) )
    , CheckerSize( 16 )
    , FitToViewport( true )
    , TextureBorderColor( FColor::White )
    , TextureBorderEnabled( false )
    , BrushDefaults()
{
}

//Static
UOdysseyPainterEditorSettings* UOdysseyPainterEditorSettings::Get()
{
    return CastChecked<UOdysseyPainterEditorSettings>(UOdysseyPainterEditorSettings::StaticClass()->GetDefaultObject());
}
