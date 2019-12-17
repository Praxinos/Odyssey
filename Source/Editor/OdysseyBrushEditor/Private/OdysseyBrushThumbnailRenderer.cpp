// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyBrushThumbnailRenderer.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Engine/EngineTypes.h"
#include "Engine/Texture2D.h"

UOdysseyBrushThumbnailRenderer::UOdysseyBrushThumbnailRenderer( const  FObjectInitializer&  ObjectInitializer )
    : Super( ObjectInitializer )
{
}

void
UOdysseyBrushThumbnailRenderer::Draw( UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas )
{
    DrawDefaultThumbnail( X, Y, Width, Height, RenderTarget, Canvas );
}

void
UOdysseyBrushThumbnailRenderer::DrawDefaultThumbnail( int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas )
{
    static  UTexture2D* DefaultTexture = nullptr;
    if( DefaultTexture == nullptr )
        DefaultTexture = LoadObject< UTexture2D >( nullptr, TEXT( "/Iliad/BrushThumbnails/OdysseyBrushDefaultThumbnail_512.OdysseyBrushDefaultThumbnail_512" ), nullptr, LOAD_None, nullptr );

    Canvas->DrawTile(
        (float)X,
        (float)Y,
        (float)Width,
        (float)Height,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        FLinearColor::White,
        DefaultTexture->Resource,
        true);
}
