// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyImportTexturesViewportClient.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "CanvasRender.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "InputKeyEventArgs.h"
#include "Texture2DPreview.h"
#include "TextureResource.h"
#include "UnrealClient.h"

#include "OdysseyAntiAliasing.h"
#include "OdysseyPainterEditorSettings.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// FOdysseyImportTexturesViewportClient
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyImportTexturesViewportClient::~FOdysseyImportTexturesViewportClient()
{
}

FOdysseyImportTexturesViewportClient::FOdysseyImportTexturesViewportClient(uint32 iCanvasWidth, uint32 iCanvasHeight, uint32 iMaxWidth, uint32 iMaxHeight)
    : mCanvasWidth(iCanvasWidth)
    , mCanvasHeight(iCanvasHeight)
    , mMaxWidth(iMaxWidth)
    , mMaxHeight(iMaxHeight)
    , mHUD(MakeShared<FOdysseyHUDElement>())
{
    const UOdysseyPainterEditorSettings& settings = *GetDefault< UOdysseyPainterEditorSettings >();
    mCheckerboardTexture = FImageUtils::CreateCheckerboardTexture( settings.GetCheckerColorOne(), settings.GetCheckerColorTwo(), settings.GetCheckerSize() );

    mNearestNeighborTexture = MakeUnique<FTexture>();
    mNearestNeighborTexture->SamplerStateRHI = Odyssey::GetSamplerStateForAntiAliasing(EOdysseyAntiAliasing::NearestNeighbor);
    mBilinearTexture = MakeUnique<FTexture>();
    mBilinearTexture->SamplerStateRHI = Odyssey::GetSamplerStateForAntiAliasing(EOdysseyAntiAliasing::Bilinear);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Draw Viewport
void
FOdysseyImportTexturesViewportClient::Draw( FViewport* iViewport, FCanvas* ioCanvas )
{
    if (!mTranformInitialized)
        InitTransform(iViewport);

    const UOdysseyPainterEditorSettings& settings = *GetDefault<UOdysseyPainterEditorSettings>();
    ioCanvas->Clear(settings.GetBackgroundColor());

    ioCanvas->PushRelativeTransform(mTransform); //

    // Draw background Checker
    if (mCheckerboardTexture)
    {
        FCanvasTileItem tileItem(
            FVector2D(0, 0),
            mCheckerboardTexture->GetResource(),
            FVector2D(mCanvasWidth, mCanvasHeight),
            FVector2D(0, 0),
            FVector2D( mCanvasWidth / mCheckerboardTexture->GetSurfaceWidth(), mCanvasHeight / mCheckerboardTexture->GetSurfaceHeight() ),
            FLinearColor::White
        );
        tileItem.BlendMode = SE_BLEND_Opaque;
        ioCanvas->DrawItem( tileItem );
    }

    if (mTexture && mTexture->GetResource())
    {
        float width = mTexture->GetSurfaceWidth();
        float height = mTexture->GetSurfaceHeight();

        mNearestNeighborTexture->TextureRHI = mTexture->GetResource()->TextureRHI;
        mBilinearTexture->TextureRHI = mTexture->GetResource()->TextureRHI;
        FTexture* tileTexture = GetZoom() <= 1.5 ? mBilinearTexture.Get() : mNearestNeighborTexture.Get();

        //TileItem
        FCanvasTileItem tileItem(
            FVector2D( 0, 0 ),
            tileTexture,
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

    FOdysseyHUDElement::FDrawHUDParams params;
    params.mCanvas = ioCanvas;
    params.mTextureWidth = mTexture->GetSurfaceWidth();
    params.mTextureHeight = mTexture->GetSurfaceHeight();
    params.mTextureToHUD = FOdysseyHUDElement::FDrawHUDParams::FTextureToHUD::CreateLambda(
        [](const FVector2D& iPosition)
        {
            return iPosition;
        }
    );
    params.mHUDToTexture = FOdysseyHUDElement::FDrawHUDParams::FTextureToHUD::CreateLambda(
        [](const FVector2D& iPosition)
        {
            return iPosition;
        }
    );

    mHUD->Draw(params);

    ioCanvas->PopTransform();
}

float
FOdysseyImportTexturesViewportClient::GetZoom() const
{
    FVector dummy(1.f, 1.f, 1.f);
    return mTransform.TransformVector(dummy).X;
}

void
FOdysseyImportTexturesViewportClient::SetZoom(float iZoom, const FVector2D& iZoomPosition )
{
    //Zooms in the center of the viewport
    float zoom = FMath::Max(iZoom, mMinZoom);
    float oldZoom = GetZoom();

    mTransform *= FTranslationMatrix(FVector(-iZoomPosition, 0));
    mTransform *= FScaleMatrix(zoom / oldZoom);
    mTransform *= FTranslationMatrix(FVector(iZoomPosition, 0));
}

bool
FOdysseyImportTexturesViewportClient::InputKey(const FInputKeyEventArgs& iEventArgs)
{
    if( iEventArgs.Event == EInputEvent::IE_Repeat )
        return false;

    if( iEventArgs.Event == EInputEvent::IE_Pressed )
    {
        mInitialTransform = mTransform;
        mInitialMousePosition = FVector2D(iEventArgs.Viewport->GetMouseX(), iEventArgs.Viewport->GetMouseY());
        if ( iEventArgs.Key == EKeys::LeftMouseButton )
        {
            mIsPanning = true;
        }
        else if (iEventArgs.Key == EKeys::RightMouseButton)
        {
            mIsZooming = true;
        }
        else if (iEventArgs.Key == EKeys::MouseScrollUp)
        {
            //ZoomIn
            float sliderPos = FMath::Loge(GetZoom());
            sliderPos += 0.1f;
            float newZoom = FMath::Exp(sliderPos);
            SetZoom(newZoom, mInitialMousePosition);
        }
        else if (iEventArgs.Key == EKeys::MouseScrollDown)
        {
            //ZoomOut
            float sliderPos = FMath::Loge(GetZoom());
            sliderPos -= 0.1f;
            float newZoom = FMath::Exp(sliderPos);
            SetZoom(newZoom, mInitialMousePosition);
        }
    }

    if( iEventArgs.Event == EInputEvent::IE_Released )
    {
        if ( iEventArgs.Key == EKeys::LeftMouseButton )
        {
            mIsPanning = false;
        }
        else if (iEventArgs.Key == EKeys::RightMouseButton)
        {
            mIsZooming = false;
        }
    }
    return true; //consume event
}

void
FOdysseyImportTexturesViewportClient::CapturedMouseMove( FViewport* InViewport, int32 InMouseX, int32 InMouseY )
{
    FVector2D mousePosition(InMouseX, InMouseY);
    FVector2D delta = mousePosition - mInitialMousePosition;

    if (mIsPanning)
    {
        mTransform = mInitialTransform * FTranslationMatrix(FVector(delta, 0));
    }
    else if (mIsZooming)
    {
        float smoothness = 200.f; //TODO: do a Setting to let the user change it at will

        if (delta.X > KINDA_SMALL_NUMBER || delta.X < KINDA_SMALL_NUMBER)
        {
            float zoom = FMath::Max( FMath::Exp(delta.X / smoothness), mMinZoom );
            mTransform = mInitialTransform;
            mTransform *= FTranslationMatrix(FVector(-mInitialMousePosition, 0));
            mTransform *= FScaleMatrix(zoom);
            mTransform *= FTranslationMatrix(FVector(mInitialMousePosition, 0));
        }
    }
}

void
FOdysseyImportTexturesViewportClient::InitTransform(FViewport* InViewport)
{
    FIntRect canvasPadding(10.f, 10.f, -10.f, -10.f);
    const FIntRect& canvasRect = FIntRect(0, 0, InViewport->GetSizeXY().X, InViewport->GetSizeXY().Y) + canvasPadding;

    uint32 maxWidth = FMath::Max(mMaxWidth, mCanvasWidth);
    uint32 maxheight = FMath::Max(mMaxHeight, mCanvasHeight);

    mMinZoom = FMath::Min(float(canvasRect.Width()) / maxWidth, float(canvasRect.Height()) / maxheight);

    float canvasCenter_x = mCanvasWidth  / 2.f;
    float canvasCenter_y = mCanvasHeight / 2.f;

    float viewportCenter_x = InViewport->GetSizeXY().X  / 2.f;
    float viewportCenter_y = InViewport->GetSizeXY().Y / 2.f;

    mTransform = FMatrix::Identity;
    mTransform *= FTranslationMatrix(FVector(-canvasCenter_x, -canvasCenter_y,0));
    mTransform *= FScaleMatrix(mMinZoom); //Zoom
    mTransform *= FTranslationMatrix(FVector(viewportCenter_x, viewportCenter_y,0));

    mTranformInitialized = true;
}

void
FOdysseyImportTexturesViewportClient::SetTexture(UTexture* iTexture)
{
    mTexture = iTexture;
}

TSharedRef<FOdysseyHUDElement>
FOdysseyImportTexturesViewportClient::GetHUD() const
{
    return mHUD;
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
