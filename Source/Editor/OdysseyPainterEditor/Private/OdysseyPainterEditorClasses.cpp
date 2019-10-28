// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "OdysseyPainterEditorSettings.h"


UOdysseyPainterEditorSettings::UOdysseyPainterEditorSettings( const FObjectInitializer& ObjectInitializer )
    : Super(ObjectInitializer)
    , Background(OdysseyPainterEditorBackground_Checkered)
    , BackgroundColor(FColor( 127, 127, 127 ) )
    , CheckerColorOne(FColor(166, 166, 166))
    , CheckerColorTwo(FColor(134, 134, 134))
    , CheckerSize( 10 )
    , FitToViewport(true)
    , TextureBorderColor(FColor::White)
    , TextureBorderEnabled(false)
{ }
