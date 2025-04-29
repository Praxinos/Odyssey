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
    Render_GameThread(iRenderTarget, iFrame, EOdysseyRenderingType::Render, iSrcRect, iDstRect);
}

void
IOdysseyTextureRenderingAbility::Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, FCanRenderFunction iCanRenderFunction) const
{
    FIntRect defaultRect = GetDefaultRenderRect();
    Render_GameThread(iRenderTarget, iFrame, iType, defaultRect, defaultRect, iCanRenderFunction);
}

void
IOdysseyTextureRenderingAbility::Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, const FIntRect& iSrcRect, const FIntPoint& iPos, FCanRenderFunction iCanRenderFunction) const
{
    Render_GameThread(iRenderTarget, iFrame, iType, iSrcRect, FIntRect(iPos.X, iPos.Y, iPos.X + iSrcRect.Width(), iPos.Y + iSrcRect.Height()), iCanRenderFunction);
}

void
IOdysseyTextureRenderingAbility::Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, const FIntRect& iSrcRect, FCanRenderFunction iCanRenderFunction) const
{
    Render_GameThread(iRenderTarget, iFrame, iType, iSrcRect, iSrcRect, iCanRenderFunction);
}

void
IOdysseyTextureRenderingAbility::Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, const FIntRect& iSrcRect, const FIntRect& iDstRect, FCanRenderFunction iCanRenderFunction) const
{
    const ERHIFeatureLevel::Type featureLevel = iRenderTarget->GetWorld() ? iRenderTarget->GetWorld()->GetFeatureLevel() : GMaxRHIFeatureLevel;

    IOdysseyTextureRenderingAbility::FRenderFunction renderFunction;
    if (!BuildRenderPipeline(iFrame, iType, renderFunction, iCanRenderFunction, {}))
    {
        ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
            [this, iRenderTarget, iDstRect](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder graphBuilder(RHICmdList);

                FRDGTextureRef destinationTexture = iRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                AddClearRenderTargetPass(graphBuilder, destinationTexture, FLinearColor::Transparent, iDstRect);

                graphBuilder.Execute();
            }
        );
        return;
    }

    ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
        [this, renderFunction, iRenderTarget, featureLevel, iSrcRect, iDstRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);

            FRDGTextureRef destinationTexture = iRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

            renderFunction(
                graphBuilder,
                featureLevel,
                destinationTexture,
                iSrcRect,
                iDstRect,
                FMatrix::Identity
            );

            graphBuilder.Execute();
        }
    );
}

#undef LOCTEXT_NAMESPACE
