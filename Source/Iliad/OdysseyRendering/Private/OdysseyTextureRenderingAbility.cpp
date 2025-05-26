// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureRenderingAbility.h"

#include "CanvasTypes.h"
#include "ScreenPass.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "TextureResource.h"

#include "OdysseyBlendShader.h"

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

//---

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

//---

void
IOdysseyTextureRenderingAbility::RenderAndResize_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, FCanRenderFunction iCanRenderFunction) const
{
    FIntRect srcDefaultRect = GetDefaultRenderRect();
    FIntRect dstDefaultRect( 0, 0, iRenderTarget->SizeX, iRenderTarget->SizeY );
    RenderAndResize_GameThread(iRenderTarget, iFrame, iType, srcDefaultRect, dstDefaultRect, iCanRenderFunction);
}

void
IOdysseyTextureRenderingAbility::RenderAndResize_GameThread(UTextureRenderTarget2D* iDstRenderTarget, FFrameNumber iFrame, uint64 iType, const FIntRect& iSrcRect, const FIntRect& iDstRect, FCanRenderFunction iCanRenderFunction) const
{
    const ERHIFeatureLevel::Type featureLevel = iDstRenderTarget->GetWorld() ? iDstRenderTarget->GetWorld()->GetFeatureLevel() : GMaxRHIFeatureLevel;

    IOdysseyTextureRenderingAbility::FRenderFunction renderFunction;
    if (!BuildRenderPipeline(iFrame, iType, renderFunction, iCanRenderFunction, {}))
    {
        ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
            [this, iDstRenderTarget, iDstRect](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder graphBuilder(RHICmdList);

                FRDGTextureRef destinationTexture = iDstRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                AddClearRenderTargetPass(graphBuilder, destinationTexture, FLinearColor::Transparent, iDstRect);

                graphBuilder.Execute();
            }
        );
        return;
    }

    ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
        [this, renderFunction, iDstRenderTarget, featureLevel, iSrcRect, iDstRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder( RHICmdList );

            FRDGTextureRef destinationTexture = graphBuilder.RegisterExternalTexture( CreateRenderTarget( iDstRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture(), TEXT( "UOdysseyAnimationCellThumbnailRenderer::destinationTexture" ) ) );

            AddClearRenderTargetPass( graphBuilder, destinationTexture, FLinearColor::Transparent, iDstRect );

            FRDGTextureDesc renderTextureDesc = FRDGTextureDesc::Create2D(
                iSrcRect.Size(),
                destinationTexture->Desc.Format,
                FClearValueBinding::Transparent,
                ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
            );
            FRDGTextureRef renderTexture = graphBuilder.CreateTexture( renderTextureDesc, TEXT( "UOdysseyLayer::renderTexture" ) );

            renderFunction(
                graphBuilder,
                featureLevel,
                renderTexture,
                iSrcRect,
                iSrcRect,
                FMatrix::Identity
            );

            FOdysseyBlendShader::BlendRect(
                graphBuilder,
                featureLevel,
                destinationTexture,
                renderTexture,
                destinationTexture,
                iSrcRect,
                iDstRect,
                FMatrix::Identity,
                EOdysseyBlendingMode::kNormal,
                EOdysseyAlphaMode::kNormal,
                1.0f,
                EOdysseyAntiAliasing::Anisotropic
            );

            graphBuilder.Execute();
        }
    );
}

#undef LOCTEXT_NAMESPACE
