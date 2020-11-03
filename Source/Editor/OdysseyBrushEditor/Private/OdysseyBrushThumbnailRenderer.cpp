// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyBrushThumbnailRenderer.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Engine/EngineTypes.h"
#include "Editor/UnrealEdEngine.h"
#include "ObjectTools.h"

UOdysseyBrushThumbnailRenderer::UOdysseyBrushThumbnailRenderer( const  FObjectInitializer&  ObjectInitializer )
    : Super( ObjectInitializer )
{
}

/*void
UOdysseyBrushThumbnailRenderer::Draw( UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas )
{
    FObjectThumbnail outThumbnail = FObjectThumbnail();
    ThumbnailTools::AssetHasCustomThumbnail( Object->GetFullName(), outThumbnail );
    
    if( !outThumbnail.IsEmpty() )
       DrawCustomThumbnail( Object, X, Y, Width, Height, RenderTarget, Canvas );
    else
       DrawDefaultThumbnail( X, Y, Width, Height, Canvas );
}

void
UOdysseyBrushThumbnailRenderer::DrawDefaultThumbnail( int32 X, int32 Y, uint32 Width, uint32 Height, FCanvas* Canvas )
{
    UE_LOG(LogTemp, Display, TEXT("DEFAULT"));

    UTexture* defaultTexture = LoadObject< UTexture >( nullptr, TEXT( "/Iliad/BrushThumbnails/OdysseyBrushDefaultThumbnail_512.OdysseyBrushDefaultThumbnail_512" ), nullptr, LOAD_None, nullptr );

    if( defaultTexture )
    {
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
            defaultTexture->Resource,
            true);
    }
}

void
UOdysseyBrushThumbnailRenderer::DrawCustomThumbnail( UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas )
{
    UE_LOG(LogTemp, Display, TEXT("CUSTOM"));
    UE_LOG(LogTemp, Display, TEXT("RenderTarget: %p"), RenderTarget);

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
        static_cast< FTextureRenderTargetResource* >(RenderTarget),
        true);
}
*/
