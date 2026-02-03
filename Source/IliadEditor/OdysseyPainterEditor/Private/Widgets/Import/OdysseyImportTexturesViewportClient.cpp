// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyImportTexturesViewportClient.h"
#include "OdysseyPainterEditorSettings.h"
#include "ImageUtils.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "Texture2DPreview.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// FOdysseyImportTexturesViewportClient
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyImportTexturesViewportClient::~FOdysseyImportTexturesViewportClient()
{
}

FOdysseyImportTexturesViewportClient::FOdysseyImportTexturesViewportClient(uint32 iCanvasWidth, uint32 iCanvasHeight)
    : mCanvasWidth(iCanvasWidth)
    , mCanvasHeight(iCanvasHeight)
{
    const UOdysseyPainterEditorSettings& settings = *GetDefault< UOdysseyPainterEditorSettings >();
    mCheckerboardTexture = FImageUtils::CreateCheckerboardTexture( settings.CheckerColorOne, settings.CheckerColorTwo, settings.CheckerSize );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Draw Viewport
void
FOdysseyImportTexturesViewportClient::Draw( FViewport* iViewport, FCanvas* ioCanvas )
{
    const UOdysseyPainterEditorSettings& settings = *GetDefault<UOdysseyPainterEditorSettings>();
    ioCanvas->Clear(settings.BackgroundColor);


    FIntRect canvasPadding(10.f, 10.f, -10.f, -10.f);
    const FIntRect& canvasRect = ioCanvas->GetViewRect() + canvasPadding;
    float zoomFactor = FMath::Min(float(canvasRect.Width()) / mCanvasWidth, float(canvasRect.Height()) / mCanvasHeight);

    // Draw background Checker
    if (mCheckerboardTexture)
    {
        float width = mCanvasWidth * zoomFactor;
        float height = mCanvasHeight * zoomFactor;

        FCanvasTileItem tileItem(
            FVector2D( canvasRect.Min.X + (canvasRect.Width() - width) / 2.f, canvasRect.Min.Y + (canvasRect.Height() - height) / 2.f),
            mCheckerboardTexture->GetResource(),
            FVector2D(width, height),
            FVector2D(0, 0),
            FVector2D( width / mCheckerboardTexture->GetSurfaceWidth(), height / mCheckerboardTexture->GetSurfaceHeight() ),
            FLinearColor::White
        );
        tileItem.BlendMode = SE_BLEND_Opaque;
        ioCanvas->DrawItem( tileItem );
    }

    if (mTexture && mTexture->GetResource())
    {
        float width = mTexture->GetSurfaceWidth() * zoomFactor;
        float height = mTexture->GetSurfaceHeight() * zoomFactor;

        //TileItem
        FCanvasTileItem tileItem(
            FVector2D( canvasRect.Min.X + (canvasRect.Width() - width) / 2.f, canvasRect.Min.Y + (canvasRect.Height() - height) / 2.f),
            mTexture->GetResource(),
            FVector2D(width, height),
            FVector2D(0, 0),
            FVector2D( 1.f, 1.f ),
            FLinearColor::White
        );
        uint32 result = (uint32)SE_BLEND_RGBA_MASK_START;
        result += ( 1 << 0 );
        result += ( 1 << 1 );
        result += ( 1 << 2 );
        result += ( 1 << 3 );
        tileItem.BlendMode = (ESimpleElementBlendMode)result;
        ioCanvas->DrawItem( tileItem );
    }
}

void
FOdysseyImportTexturesViewportClient::SetTexture(UTexture* iTexture)
{
    mTexture = iTexture;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------ FGCObject API
void
FOdysseyImportTexturesViewportClient::AddReferencedObjects( FReferenceCollector& ioCollector )
{
    ioCollector.AddReferencedObject( mCheckerboardTexture );
    ioCollector.AddReferencedObject( mTexture );
}

FString FOdysseyImportTexturesViewportClient::GetReferencerName() const
{
    return TEXT("FOdysseyImportTexturesViewportClient");
}

#undef LOCTEXT_NAMESPACE
