// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseyPainterEditorSettings.h"

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
{
}
