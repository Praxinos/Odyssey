// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyTextureRenderingAbility.h"
#include "CanvasTypes.h"
#include "ScreenPass.h"

#define LOCTEXT_NAMESPACE "Imaging"

void
IOdysseyTextureRenderingAbility::Render_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame) const
{
    FIntRect defaultRect = GetDefaultRenderRect();
    RenderRectAtRect_Implementation(iRenderTarget, iFrame, defaultRect, defaultRect);
}

void
IOdysseyTextureRenderingAbility::RenderRect_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const FIntRect& iSrcRect) const
{
    RenderRectAtRect_Implementation(iRenderTarget, iFrame, iSrcRect, iSrcRect);
}

void
IOdysseyTextureRenderingAbility::RenderRectAtPosition_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const FIntRect& iSrcRect, const FIntPoint& iPos) const
{
    RenderRectAtRect_Implementation(iRenderTarget, iFrame, iSrcRect, FIntRect(iPos.X, iPos.Y, iPos.X + iSrcRect.Width(), iPos.Y + iSrcRect.Height()));
}

void
IOdysseyTextureRenderingAbility::RenderRectAtRect_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const FIntRect& iSrcRect, const FIntRect& iDstRect) const
{
    const ERHIFeatureLevel::Type featureLevel = iRenderTarget->GetWorld() ? iRenderTarget->GetWorld()->GetFeatureLevel() : GMaxRHIFeatureLevel;

    mPixelFence.BeginFence();
    mPixelFence.Wait();

    ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
        [this, iRenderTarget, iFrame, iSrcRect, iDstRect, featureLevel](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef destinationTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(iRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture(), TEXT("Odyssey::Blend::DestinationTexture")));

            /*FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
                destinationTexture->Desc.Extent,
                PF_FloatRGBA,
                FClearValueBinding::Transparent,
                ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
            );
            FRDGTextureRef backdropTexture = graphBuilder.CreateTexture(desc, TEXT("UOdysseyAnimationCellImageRaster::RenderTarget"));

            AddDrawTexturePass(
                graphBuilder,
                GetGlobalShaderMap(GMaxRHIFeatureLevel),
                destinationTexture,
                backdropTexture,
                FRDGDrawTextureInfo()
            ); */

            RenderToTexture_RenderThread(graphBuilder, destinationTexture, featureLevel, iFrame, iSrcRect, iDstRect);

            /* AddDrawTexturePass(
                graphBuilder,
                GetGlobalShaderMap(GMaxRHIFeatureLevel),
                backdropTexture,
                destinationTexture,
                FRDGDrawTextureInfo()
            ); */

            graphBuilder.Execute();
        }
    );

    mPixelFence.BeginFence();
    mPixelFence.Wait();
}

#undef LOCTEXT_NAMESPACE
