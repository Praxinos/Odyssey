// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorTiledViewportClient.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "CanvasRender.h"
#include "Engine/Texture2D.h"
#include "Engine.h"
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

    //We only need 1 layer containing base color
    RenderTargetData->NumLayers = 1;
    RenderTargetData->WidthInBlocks = 1;
    RenderTargetData->HeightInBlocks = 1;
    RenderTargetData->TileSize = 256;
    RenderTargetData->TileBorderSize = 0; // A BorderSize pixel border will be added around all tiles
    RenderTargetData->Width = 65536; //~2 million pixels in width => 2^21
    RenderTargetData->Height = 65536; //~ 2 million pixels in height => 2^21
    RenderTargetData->NumMips = FMath::CeilLogTwo(FMath::Max(RenderTargetData->Width, RenderTargetData->Height));

    check(RenderTargetData->Width / RenderTargetData->TileSize <= VIRTUALTEXTURE_MAX_PAGETABLE_SIZE)
    check(RenderTargetData->Height / RenderTargetData->TileSize <= VIRTUALTEXTURE_MAX_PAGETABLE_SIZE)

    for (uint32 i = 0; i < RenderTargetData->NumLayers; i++)
    {
        RenderTargetData->LayerTypes[i] = PF_B8G8R8A8;
        RenderTargetData->LayerFallbackColors[i] = FLinearColor::Yellow;
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
FOdysseyPainterEditorTiledViewportClient::DrawCheckboard( FViewport* iViewport, FCanvas* ioCanvas )
{
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
}

void
FOdysseyPainterEditorTiledViewportClient::DrawVirtualTexture( FViewport* iViewport, FCanvas* ioCanvas )
{
    const FVector scaleVector = mTransform.GetScaleVector();
    int32 MipLevel = FMath::CeilToInt(FMath::Max(1.0f, -FMath::Log2(scaleVector.X))) - 1.0f;

    UE_LOG(LogTemp, Warning, TEXT("===================="));
    UE_LOG(LogTemp, Warning, TEXT("MipLevel=%d"), MipLevel);
    UE_LOG(LogTemp, Warning, TEXT("Zoom=%.2f %% "), scaleVector.X * 100.f);

    FOdysseyVirtualRenderTargetResource* RenderTargetResourcePtr = &RenderTargetResource;
    FVector2D TextureSize(RenderTargetResource.GetSizeX(), RenderTargetResource.GetSizeY());
    FVector2D ScaledTextureSize = TextureSize * scaleVector.X;
    FVector2D ViewportSize(iViewport->GetSizeXY());

    TRefCountPtr<FBatchedElementParameters> BatchedElementParameters = new FBatchedElementTexture2DPreviewParameters(MipLevel, 0, 0, false, false, false, true, false, false);

    FMatrix inverseTransform = mTransform.Inverse();

    FVector ViewportTopLeft = inverseTransform.TransformPosition(FVector(0.0f, 0.0f, 0.f));
    FVector ViewportTopRight = inverseTransform.TransformPosition(FVector(ViewportSize.X, 0.f, 0.f));
    FVector ViewportBottomLeft = inverseTransform.TransformPosition(FVector(0.f, ViewportSize.Y, 0.f));
    FVector ViewportBottomRight = inverseTransform.TransformPosition(FVector(ViewportSize.X, ViewportSize.Y, 0.f));

    FBox2D TransformedViewportBoundingBox(
        FVector2D(
            FMath::Min(ViewportTopLeft.X, ViewportTopRight.X, ViewportBottomLeft.X, ViewportBottomRight.X) * scaleVector.X,
            FMath::Min(ViewportTopLeft.Y, ViewportTopRight.Y, ViewportBottomLeft.Y, ViewportBottomRight.Y) * scaleVector.Y
        ),
        FVector2D(
            FMath::Max(ViewportTopLeft.X, ViewportTopRight.X, ViewportBottomLeft.X, ViewportBottomRight.X) * scaleVector.X,
            FMath::Max(ViewportTopLeft.Y, ViewportTopRight.Y, ViewportBottomLeft.Y, ViewportBottomRight.Y) * scaleVector.Y
        )
    );

    FVector2D TransformedViewportBoundingBoxSize = TransformedViewportBoundingBox.Max - TransformedViewportBoundingBox.Min;
    /*FBox2D ScaledTextureBoundingBox( FVector2D::Zero(), ScaledTextureSize);
    FBox2D BoundingBoxToLoad = TransformedViewportBoundingBox.Overlap( ScaledTextureBoundingBox );
    FVector2D UV0 = FVector2D(BoundingBoxToLoad.Min.X / ScaledTextureSize.X, BoundingBoxToLoad.Min.Y / ScaledTextureSize.Y);
    FVector2D UV1 = FVector2D(BoundingBoxToLoad.Max.X / ScaledTextureSize.X, BoundingBoxToLoad.Max.Y / ScaledTextureSize.Y);

    if (RenderTargetResource.GetNumBlocks() > 1)
    {
        // Adjust UVs to display entire UDIM range, accounting for UE inverted V-axis
        const FIntPoint BlockSize = RenderTargetResource.GetSizeInBlocks();
        UV0 = FVector2D(-1.0f, -1.0f) + UV0 * BlockSize;
        UV1 = FVector2D(-1.0f, -1.0f) + UV1 * BlockSize;
    } */

    const FVector2D TexturePositionInTransformedViewport = -TransformedViewportBoundingBox.Min - ScaledTextureSize / 2.f;

    FVector2D UV0(1.0f - RenderTargetResource.GetSizeInBlocks().X, 1.0f - RenderTargetResource.GetSizeInBlocks().Y);
    FVector2D UV1(RenderTargetResource.GetSizeInBlocks().X, RenderTargetResource.GetSizeInBlocks().Y);

    UE::RenderCommandPipe::FSyncScope SyncScope;

    const ERHIFeatureLevel::Type InFeatureLevel = GMaxRHIFeatureLevel;
    ENQUEUE_RENDER_COMMAND(MakeTilesResident)(
        [InFeatureLevel, RenderTargetResourcePtr, ScaledTextureSize, TexturePositionInTransformedViewport, TransformedViewportBoundingBoxSize, UV0, UV1, MipLevel](FRHICommandListImmediate& RHICmdList)
    {
        // AcquireAllocatedVT() must happen on render thread
        IAllocatedVirtualTexture* AllocatedVT = RenderTargetResourcePtr->AcquireAllocatedVT();

        IRendererModule& RenderModule = GetRendererModule();
        RenderModule.RequestVirtualTextureTiles(
            AllocatedVT,
            ScaledTextureSize,
            TexturePositionInTransformedViewport,
            TransformedViewportBoundingBoxSize,
            UV0,
            UV1,
            MipLevel
        );
        RenderModule.LoadPendingVirtualTextureTiles(RHICmdList, InFeatureLevel);
    });

    FCanvasTileItem TileItem( -TextureSize/2, RenderTargetResourcePtr, TextureSize, UV0, UV1, FLinearColor::White );

    // Add the red, green, blue, alpha and desaturation flags to the enum to identify the chosen filters
    uint32 BlendMode = (uint32)SE_BLEND_RGBA_MASK_START;
    BlendMode += 1 << 0;
    BlendMode += 1 << 1;
    BlendMode += 1 << 2;
    BlendMode += 1 << 3;
    TileItem.BlendMode = (ESimpleElementBlendMode)BlendMode;

    TileItem.BatchedElementParameters = BatchedElementParameters;

    /* if (RenderTargetResource.GetNumBlocks() > 1)
    {
        // Adjust UVs to display entire UDIM range, accounting for UE inverted V-axis
        const FIntPoint BlockSize = RenderTargetResource.GetSizeInBlocks();
        TileItem.UV0 = FVector2D(0.0f, 1.0f - (float)BlockSize.Y);
        TileItem.UV1 = FVector2D((float)BlockSize.X, 1.0f);
    } */

    ioCanvas->DrawItem( TileItem );
}

void
FOdysseyPainterEditorTiledViewportClient::DrawVirtualTextureWarning( FViewport* iViewport, FCanvas* ioCanvas )
{
    UFont* ReportingFont = GEngine->GetLargeFont();
    constexpr double ReportingLineSpacing = 2;
    double ReportingLineX = 8;
    double ReportingLineY = 8;
    float FontScale = 1.f;
    FSlateFontInfo FontInfo(ReportingFont, ReportingFont->LegacyFontSize);

    auto DrawText = [&FontInfo, &ioCanvas, &FontScale](const double LineX, const double LineY, const FText& InText, const FLinearColor& InColor)->FVector2D
    {
        const FVector2D InPosition(LineX, LineY);
        FCanvasTextItem TextItem(InPosition, InText, FontInfo, InColor);
        TextItem.Scale = FVector2D(FontScale);
        ioCanvas->DrawItem(TextItem);
        return TextItem.DrawnSize;
    };

    checkf(ReportingFont, TEXT("Texture Editor : Engine large font must be set"));

    // If we are requesting an explicit mip level of a VT asset, test to see if we can even display it properly and warn about it
    const FVector scaleVector = mTransform.GetScaleVector();
    int32 MipLevel = FMath::CeilLogTwo(1/scaleVector.X);
    MipLevel = FMath::Max(0, MipLevel);
    if (MipLevel >= 0.f)
    {
        const uint32 Mip = (uint32)MipLevel;
        const FIntPoint SizeOnMip = { (int32)RenderTargetResource.GetSizeX() >> Mip, (int32)RenderTargetResource.GetSizeY() >> Mip };
        const uint64 NumPixels = static_cast<uint64>(SizeOnMip.X) * SizeOnMip.Y;

        const FIntPoint PhysicalTextureSize = RenderTargetResource.GetPhysicalTextureSize(0u);
        const uint64 NumPhysicalPixels = static_cast<uint64>(PhysicalTextureSize.X) * PhysicalTextureSize.Y;

        if (NumPixels >= NumPhysicalPixels)
        {
            const FText Message = NSLOCTEXT("OdysseyTextureEditor", "InvalidVirtualTextureMipDisplay", "Displaying a virtual texture on a mip level that is larger than the physical cache. Rendering will probably be invalid!");
            DrawText(ReportingLineX, ReportingLineY, Message, FLinearColor::Red);
            //ReportingLineY += ReportingLineHeight;
        }
    }
}

void
FOdysseyPainterEditorTiledViewportClient::Draw( FViewport* iViewport, FCanvas* ioCanvas )
{
    if (!mTranformInitialized)
        InitTransform(iViewport);

    const UOdysseyPainterEditorSettings& settings = *GetDefault<UOdysseyPainterEditorSettings>();
    ioCanvas->Clear(settings.GetBackgroundColor());
    ioCanvas->PushRelativeTransform(mTransform); //

    DrawCheckboard(iViewport, ioCanvas);

    bool bIsVirtualTexture = true;
    if (bIsVirtualTexture)
        DrawVirtualTexture(iViewport, ioCanvas);

    ioCanvas->PopTransform();

    DrawVirtualTextureWarning(iViewport, ioCanvas);

    ioCanvas->Flush_GameThread();
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
