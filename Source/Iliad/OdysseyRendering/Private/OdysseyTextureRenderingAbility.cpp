// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyTextureRenderingAbility.h"
#include "CanvasTypes.h"
#include "ScreenPass.h"
#include "Engine/TextureRenderTarget2D.h"

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

    TSharedPtr<FOdysseyTextureRenderer> renderer = BuildTextureRenderer(iFrame);
    if (!renderer)
        return;

    ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
        [this, renderer, iRenderTarget, iSrcRect, iDstRect, featureLevel](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);

            FRDGTextureRef destinationTexture = iRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
            FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
                destinationTexture->Desc.Extent,
                PF_FloatRGBA,
                FClearValueBinding::Transparent,
                ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
            );

            FRDGTextureRef outputTexture = graphBuilder.CreateTexture(desc, TEXT("IOdysseyTextureRenderingAbility::outputTexture"));

            AddClearRenderTargetPass(graphBuilder, outputTexture, FLinearColor::Transparent, iDstRect );

            renderer->Render(
                graphBuilder,
                outputTexture,
                featureLevel,
                iSrcRect,
                iDstRect
            );

            const FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
            FRDGDrawTextureInfo infos;
            infos.SourcePosition = iSrcRect.Min;
            infos.DestPosition = iDstRect.Min;
            infos.Size = iDstRect.Size();

            //AddDrawTexturePass ensures format conversions
            AddDrawTexturePass(graphBuilder, GlobalShaderMap, outputTexture, destinationTexture, infos);

            AddDrawTexturePass(
                graphBuilder,
                FScreenPassViewInfo(),
                outputTexture,
                destinationTexture,
                iSrcRect.Min,
                iSrcRect.Size(),
                iDstRect.Min,
                iDstRect.Size()
            );

            graphBuilder.Execute();
        }
    );
}

#undef LOCTEXT_NAMESPACE
