// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorTiledViewportClient.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "CanvasRender.h"
#include "Engine/Texture2D.h"
#include "EngineModule.h"
#include "ImageUtils.h"
#include "InputKeyEventArgs.h"
#include "Texture2DPreview.h"
#include "TextureResource.h"
#include "UnrealClient.h"

#include "OdysseyAntiAliasing.h"
#include "OdysseyBlendShader.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyRenderingAbility.h"
#include "OdysseyRenderingType.h"
#include "OdysseyTextureRenderingAbility.h"
#include "OdysseyVirtualRenderTargetData.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorTiledViewportClient
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorTiledViewportClient::~FOdysseyPainterEditorTiledViewportClient()
{
    ENQUEUE_RENDER_COMMAND(DeleteResource)([this](FRHICommandListImmediate& RHICmdList)
    {
        RenderTargetResource.ReleaseResource();
    });
    ReleaseFence.BeginFence();
    ReleaseFence.Wait();
}

FOdysseyPainterEditorTiledViewportClient::FOdysseyPainterEditorTiledViewportClient()
    : mCanvasWidth(256)
    , mCanvasHeight(256)
    , RenderTargetData(MakeShared<FOdysseyVirtualRenderTargetData>())
    , RenderTargetResource(RenderTargetData)
{
    const UOdysseyPainterEditorSettings& settings = *GetDefault< UOdysseyPainterEditorSettings >();
    mCheckerboardTexture = FImageUtils::CreateCheckerboardTexture( settings.GetCheckerColorOne(), settings.GetCheckerColorTwo(), settings.GetCheckerSize() );

    RenderTargetData->NumLayers = 1;
    RenderTargetData->NumMips = 1;
    RenderTargetData->Width = 256;
    RenderTargetData->Height = 256;
    RenderTargetData->WidthInBlocks = 1;
    RenderTargetData->HeightInBlocks = 1;
    RenderTargetData->TileSize = 256;
    RenderTargetData->TileBorderSize = 0; // A BorderSize pixel border will be added around all tiles

    for (uint32 i = 0; i < RenderTargetData->NumLayers; i++)
    {
        RenderTargetData->LayerTypes[i] = PF_B8G8R8A8;
        RenderTargetData->LayerFallbackColors[i] = FLinearColor::Yellow;
        RenderTargetData->TileDataOffsetPerLayer.Add(0);
        RenderTargetData->BaseOffsetPerMip.Add(0);

        FOdysseyVirtualRenderTargetTileOffsetData tileOffsetData;
        tileOffsetData.Init(RenderTargetData->GetWidthInTiles(), RenderTargetData->GetHeightInTiles());

        uint32 numTiles = tileOffsetData.Width * tileOffsetData.Height;
        for (uint32 j = 0; j < numTiles; j++)
        {
            tileOffsetData.AddTile(j);
        }
        tileOffsetData.Finalize();
        RenderTargetData->TileOffsetData.Add(tileOffsetData);
    }

    // Equivalent to a Texture->UpdateResource()
    ENQUEUE_RENDER_COMMAND(InitRenderTargetResource)([this](FRHICommandListImmediate& RHICmdList)
    {
        RenderTargetResource.InitResource(RHICmdList);
    });

    /* mNearestNeighborTexture = MakeUnique<FTexture>();
    mNearestNeighborTexture->SamplerStateRHI = Odyssey::GetSamplerStateForAntiAliasing(EOdysseyAntiAliasing::NearestNeighbor);
    mBilinearTexture = MakeUnique<FTexture>();
    mBilinearTexture->SamplerStateRHI = Odyssey::GetSamplerStateForAntiAliasing(EOdysseyAntiAliasing::Bilinear); */
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Draw Viewport
void
FOdysseyPainterEditorTiledViewportClient::Draw( FViewport* iViewport, FCanvas* ioCanvas )
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

    bool bSingleVTPhysicalSpace = false;
    bool bIsVirtualTexture = true;
    float XPos = 0;
    float YPos = 0;
    float Width = RenderTargetResource.GetSizeX();
    float Height = RenderTargetResource.GetSizeY();
    FVector2D ViewportSize = FVector2D(iViewport->GetSizeXY());
    int32 MipLevel = 0;

    TRefCountPtr<FBatchedElementParameters> BatchedElementParameters = new FBatchedElementTexture2DPreviewParameters(MipLevel, 0, 0, false, false, false, true, false, false);

    FOdysseyVirtualRenderTargetResource* RenderTargetResourcePtr = &RenderTargetResource;
    //if ( Texture->GetResource() != nullptr && !CPUCopyTexture )
    {
        FCanvasTileItem TileItem( FVector2D( XPos, YPos ), RenderTargetResourcePtr, FVector2D( Width, Height ), FLinearColor::White );

        // Add the red, green, blue, alpha and desaturation flags to the enum to identify the chosen filters
        uint32 BlendMode = (uint32)SE_BLEND_RGBA_MASK_START;
        BlendMode += 1 << 0;
        BlendMode += 1 << 1;
        BlendMode += 1 << 2;
        BlendMode += 1 << 3;
        TileItem.BlendMode = (ESimpleElementBlendMode)BlendMode;

        TileItem.BatchedElementParameters = BatchedElementParameters;

        if (bIsVirtualTexture && RenderTargetResource.GetNumBlocks() > 1)
        {
            // Adjust UVs to display entire UDIM range, accounting for UE inverted V-axis
            const FIntPoint BlockSize = RenderTargetResource.GetSizeInBlocks();
            TileItem.UV0 = FVector2D(0.0f, 1.0f - (float)BlockSize.Y);
            TileItem.UV1 = FVector2D((float)BlockSize.X, 1.0f);
        }

        ioCanvas->DrawItem( TileItem );

        // if we are presenting a virtual texture, make the appropriate tiles resident
        if (bIsVirtualTexture)
        {
            const FVector2D ScreenSpaceSize(Width, Height);
            const FVector2D ViewportPositon(XPos, YPos);
            const FVector2D UV0 = TileItem.UV0;
            const FVector2D UV1 = TileItem.UV1;

            UE::RenderCommandPipe::FSyncScope SyncScope;

            const ERHIFeatureLevel::Type InFeatureLevel = GMaxRHIFeatureLevel;
            ENQUEUE_RENDER_COMMAND(MakeTilesResident)(
                [InFeatureLevel, RenderTargetResourcePtr, ScreenSpaceSize, ViewportPositon, ViewportSize, UV0, UV1, MipLevel](FRHICommandListImmediate& RHICmdList)
            {
                // AcquireAllocatedVT() must happen on render thread
                IAllocatedVirtualTexture* AllocatedVT = RenderTargetResourcePtr->AcquireAllocatedVT();

                IRendererModule& RenderModule = GetRendererModule();
                RenderModule.RequestVirtualTextureTiles(AllocatedVT, ScreenSpaceSize, ViewportPositon, ViewportSize, UV0, UV1, MipLevel);
                RenderModule.LoadPendingVirtualTextureTiles(RHICmdList, InFeatureLevel);
            });
        }
    }

    ioCanvas->PopTransform();

    //Draws the checkerBoard
    //After this line we can draw directly in the Canvas RenderTarget
    ioCanvas->Flush_GameThread();

    /* IOdysseyTextureRenderingAbility::FRenderFunction renderFunction;

    if (mTextureRenderer && mTextureRenderer->BuildRenderPipeline(FFrameNumber(0),EOdysseyRenderingType::Editor,renderFunction))
    {
        FIntRect srcRect = mTextureRenderer->GetDefaultRenderRect();

        ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
            [this, srcRect, renderFunction, canvasRenderTarget = ioCanvas->GetRenderTarget()](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder graphBuilder(RHICmdList);

                FRDGTextureRef canvasTexture = canvasRenderTarget->GetRenderTargetTexture( graphBuilder );
                FRDGTextureDesc renderTextureDesc = FRDGTextureDesc::Create2D(
                    //canvasTexture->Desc.Extent,
                    srcRect.Size(),
                    //canvasTexture->Desc.Format,
                    PF_B8G8R8A8,
                    FClearValueBinding::Transparent,
                    ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
                );
                FRDGTextureRef renderTexture = graphBuilder.CreateTexture( renderTextureDesc, TEXT( "FOdysseyPainterEditorTiledViewportClient::RenderTexture" ) );

                FIntRect canvasRect(0, 0, canvasTexture->Desc.Extent.X, canvasTexture->Desc.Extent.Y);

                renderFunction(
                    graphBuilder,
                    GMaxRHIFeatureLevel,
                    renderTexture,
                    srcRect,
                    srcRect,
                    FMatrix::Identity
                );

                FOdysseyBlendShader::BlendRect(
                    graphBuilder,
                    GMaxRHIFeatureLevel,
                    canvasTexture,
                    renderTexture,
                    canvasTexture,
                    canvasRect,
                    canvasRect,
                    mTransform,
                    EOdysseyBlendingMode::kNormal,
                    EOdysseyAlphaMode::kNormal,
                    1.f,
                    EOdysseyAntiAliasing::NearestNeighbor
                );

                graphBuilder.Execute();
            }
        );
    } */

    /* if (mTexture && mTexture->GetResource())
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
    } */


}

float
FOdysseyPainterEditorTiledViewportClient::GetZoom() const
{
    FVector dummy(1.f, 1.f, 1.f);
    return mTransform.TransformVector(dummy).X;
}

void
FOdysseyPainterEditorTiledViewportClient::SetZoom(FViewport* InViewport, float iZoom, const FVector2D& iZoomPosition )
{
    //Zooms in the center of the viewport
    float zoom = FMath::Max(iZoom, mMinZoom);
    float oldZoom = GetZoom();

    mTransform *= FTranslationMatrix(FVector(-iZoomPosition, 0));
    mTransform *= FScaleMatrix(zoom / oldZoom);
    mTransform *= FTranslationMatrix(FVector(iZoomPosition, 0));

    InViewport->Invalidate();
}

bool
FOdysseyPainterEditorTiledViewportClient::InputKey(const FInputKeyEventArgs& iEventArgs)
{
    if( iEventArgs.Event == EInputEvent::IE_Repeat )
        return false;

    if( iEventArgs.Event == EInputEvent::IE_Pressed )
    {
        mInitialTransform = mTransform;
        mInitialMousePosition = FVector2D(iEventArgs.Viewport->GetMouseX(), iEventArgs.Viewport->GetMouseY());
        if ( iEventArgs.Key == EKeys::RightMouseButton )
        {
            mIsPanning = true;
        }
        else if (iEventArgs.Key == EKeys::MouseScrollUp)
        {
            //ZoomIn
            float sliderPos = FMath::Loge(GetZoom());
            sliderPos += 0.1f;
            float newZoom = FMath::Exp(sliderPos);
            SetZoom(iEventArgs.Viewport, newZoom, mInitialMousePosition);
        }
        else if (iEventArgs.Key == EKeys::MouseScrollDown)
        {
            //ZoomOut
            float sliderPos = FMath::Loge(GetZoom());
            sliderPos -= 0.1f;
            float newZoom = FMath::Exp(sliderPos);
            SetZoom(iEventArgs.Viewport, newZoom, mInitialMousePosition);
        }
    }

    if( iEventArgs.Event == EInputEvent::IE_Released )
    {
        if ( iEventArgs.Key == EKeys::RightMouseButton )
        {
            mIsPanning = false;
        }
    }
    return true; //consume event
}

void
FOdysseyPainterEditorTiledViewportClient::CapturedMouseMove( FViewport* InViewport, int32 InMouseX, int32 InMouseY )
{
    FVector2D mousePosition(InMouseX, InMouseY);
    FVector2D delta = mousePosition - mInitialMousePosition;

    if (mIsPanning)
    {
        mTransform = mInitialTransform * FTranslationMatrix(FVector(delta, 0));
        InViewport->Invalidate();
    }
}

void
FOdysseyPainterEditorTiledViewportClient::SetTextureRenderer(class IOdysseyTextureRenderingAbility* InRenderer)
{
    mTextureRenderer = InRenderer;
}

void
FOdysseyPainterEditorTiledViewportClient::SetCanvasSize(int InWidth, int InHeight)
{
    mCanvasWidth = InWidth;
    mCanvasHeight = InHeight;

    mTranformInitialized = false;
}

void
FOdysseyPainterEditorTiledViewportClient::InitTransform(FViewport* InViewport)
{
    FIntRect canvasPadding(10.f, 10.f, -10.f, -10.f);
    const FIntRect& canvasRect = FIntRect(0, 0, InViewport->GetSizeXY().X, InViewport->GetSizeXY().Y) + canvasPadding;

    uint32 maxWidth = mCanvasWidth * 10;
    uint32 maxheight = mCanvasHeight * 10;

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

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------ FGCObject API

void
FOdysseyPainterEditorTiledViewportClient::AddReferencedObjects( FReferenceCollector& ioCollector )
{
    ioCollector.AddReferencedObject( mCheckerboardTexture );
}

FString FOdysseyPainterEditorTiledViewportClient::GetReferencerName() const
{
    return TEXT("FOdysseyPainterEditorTiledViewportClient");
}

#undef LOCTEXT_NAMESPACE
