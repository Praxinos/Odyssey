// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationCellThumbnailRenderer.h"

#include "Async/Async.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "HAL/RunnableThread.h"
#include "ImageUtils.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyAnimation.h"
#include "OdysseyPainterEditorSettings.h"
#include "RHITypes.h"
#include "TextureCompiler.h"
#include "TextureResource.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "OdysseyRasterBlockMutator.h"
#include "RenderGraphBuilder.h"

#define THUMBNAIL_RENDER_SIZE 64


void
UOdysseyAnimationCellThumbnailRenderer::PostInitProperties()
{
    Super::PostInitProperties();
    const UOdysseyPainterEditorSettings& settings = *GetDefault< UOdysseyPainterEditorSettings >();
    mCheckerboardTexture = FImageUtils::CreateCheckerboardTexture( settings.CheckerColorOne, settings.CheckerColorTwo, 16 );
}

void
UOdysseyAnimationCellThumbnailRenderer::GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const
{
    UOdysseyAnimationCell* cell = Cast<UOdysseyAnimationCell>(Object);
    if (!cell)
        return;

    UOdysseyAnimation*     animation = cell->GetAnimation();
    if (!animation)
        return;

    float ratio = (float)animation->GetWidth() / (float)animation->GetHeight();

    static const uint32 baseSize = THUMBNAIL_RENDER_SIZE;

    OutWidth = (uint32)(THUMBNAIL_RENDER_SIZE * Zoom * ratio);
    OutHeight =(uint32)(THUMBNAIL_RENDER_SIZE * Zoom);
}

void
UOdysseyAnimationCellThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily)
{
    FCanvasTileItem checkboardTileItem(
        FVector2D(X, Y),
        mCheckerboardTexture->GetResource(),
        FVector2D( Width, Height ),
        FVector2D( 0.f, 0.f ),
        FVector2D( Width / mCheckerboardTexture->GetSizeX(), Height / mCheckerboardTexture->GetSizeY() ),
        FLinearColor::White
    );
    checkboardTileItem.BlendMode = SE_BLEND_Opaque;
    Canvas->DrawItem( checkboardTileItem );
    Canvas->Flush_GameThread();

    UOdysseyAnimationCell* cell = Cast<UOdysseyAnimationCell>(Object);
    if (!cell)
        return;

    const ERHIFeatureLevel::Type featureLevel = Canvas->GetFeatureLevel();

    FOdysseyTextureRenderFunction childRenderFunction = cell->BuildRenderPipeline(FFrameNumber(0), EOdysseyRenderingType::Render);
    FIntRect srcRect = cell->GetDefaultRenderRect();
    ENQUEUE_RENDER_COMMAND(UOdysseyAnimationCellThumbnailRenderer_Draw)(
        [Viewport, srcRect, X, Y, Width, Height, featureLevel, childRenderFunction](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef destinationTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(Viewport->GetRenderTargetTexture(), TEXT("UOdysseyAnimationCellThumbnailRenderer::Draw")));

            childRenderFunction(
                graphBuilder,
                featureLevel,
                destinationTexture,
                srcRect,
                FIntRect(X, Y, Width, Height),
                FMatrix::Identity
            );

            graphBuilder.Execute();
        }
    );
}
