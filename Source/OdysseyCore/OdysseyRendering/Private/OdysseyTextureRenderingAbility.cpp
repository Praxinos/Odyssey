// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureRenderingAbility.h"

#include "CanvasTypes.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "GenerateMips.h"
#include "ScreenPass.h"
#include "TextureResource.h"

#include "OdysseyBlendShader.h"
#include "OdysseyColorSpaceShader.h"
#include "OdysseyRenderContext.h"

#define LOCTEXT_NAMESPACE "Imaging"

UTextureRenderTarget2D*
IOdysseyTextureRenderingAbility::CreateRenderTarget_Blueprint_Implementation()
{
    return CreateRenderingRenderTarget();
}

void
IOdysseyTextureRenderingAbility::Render_Implementation(UTextureRenderTarget2D* iRenderTarget) const
{
    FIntRect defaultRect = GetDefaultRenderRect();
    RenderRectAtRect_Implementation(iRenderTarget, defaultRect, defaultRect);
}

void
IOdysseyTextureRenderingAbility::RenderRect_Implementation(UTextureRenderTarget2D* iRenderTarget, const FIntRect& iSrcRect) const
{
    RenderRectAtRect_Implementation(iRenderTarget, iSrcRect, iSrcRect);
}

void
IOdysseyTextureRenderingAbility::RenderRectAtPosition_Implementation(UTextureRenderTarget2D* iRenderTarget, const FIntRect& iSrcRect, const FIntPoint& iPos) const
{
    RenderRectAtRect_Implementation(iRenderTarget, iSrcRect, FIntRect(iPos.X, iPos.Y, iPos.X + iSrcRect.Width(), iPos.Y + iSrcRect.Height()));
}

void
IOdysseyTextureRenderingAbility::RenderRectAtRect_Implementation(UTextureRenderTarget2D* iRenderTarget, const FIntRect& iSrcRect, const FIntRect& iDstRect) const
{
    Render_GameThread(iRenderTarget, EOdysseyRenderingType::Render, iSrcRect, iDstRect);
}

//---

void
IOdysseyTextureRenderingAbility::Render_GameThread(UTextureRenderTarget2D* iRenderTarget, uint64 iType, const FOdysseyRenderContext* RenderContext) const
{
    FIntRect defaultRect = GetDefaultRenderRect();
    Render_GameThread(iRenderTarget, iType, defaultRect, defaultRect, RenderContext);
}

void
IOdysseyTextureRenderingAbility::Render_GameThread(UTextureRenderTarget2D* iRenderTarget, uint64 iType, const FIntRect& iSrcRect, const FIntPoint& iPos, const FOdysseyRenderContext* RenderContext) const
{
    Render_GameThread(iRenderTarget, iType, iSrcRect, FIntRect(iPos.X, iPos.Y, iPos.X + iSrcRect.Width(), iPos.Y + iSrcRect.Height()), RenderContext);
}

void
IOdysseyTextureRenderingAbility::Render_GameThread(UTextureRenderTarget2D* iRenderTarget, uint64 iType, const FIntRect& iSrcRect, const FOdysseyRenderContext* RenderContext) const
{
    Render_GameThread(iRenderTarget, iType, iSrcRect, iSrcRect, RenderContext);
}

void
IOdysseyTextureRenderingAbility::Render_GameThread(UTextureRenderTarget2D* iRenderTarget, uint64 iType, const FIntRect& iSrcRect, const FIntRect& iDstRect, const FOdysseyRenderContext* RenderContext) const
{
    const ERHIFeatureLevel::Type featureLevel = iRenderTarget->GetWorld() ? iRenderTarget->GetWorld()->GetFeatureLevel() : GMaxRHIFeatureLevel;

    IOdysseyTextureRenderingAbility::FRenderFunction renderFunction;
    FOdysseyRenderContext Request = RenderContext ? *RenderContext : FOdysseyRenderContext();
    Request.Type = iType;
    if (!BuildRenderPipeline(renderFunction, {}, &Request))
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

    checkf(iRenderTarget, TEXT( "Render_GameThread::iRenderTarget is Null" ))
    checkf(iRenderTarget->GameThread_GetRenderTargetResource(), TEXT( "Render_GameThread::GetRenderTargetResource is Null" ))

    ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
        [this, renderFunction, iRenderTarget, featureLevel, iSrcRect, iDstRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);

            FRDGTextureRef destinationTexture = iRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

            //ensure the render target is cleared
            //this ensures that, if there is nothing to render the result will be a fully transparent texture
            AddClearRenderTargetPass(graphBuilder, destinationTexture, FLinearColor::Transparent, iDstRect);

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
IOdysseyTextureRenderingAbility::RenderAndResize_GameThread(UTextureRenderTarget2D* iRenderTarget, uint64 iType, const FOdysseyRenderContext* RenderContext) const
{
    FIntRect srcDefaultRect = GetDefaultRenderRect();
    FIntRect dstDefaultRect( 0, 0, iRenderTarget->SizeX, iRenderTarget->SizeY );
    RenderAndResize_GameThread(iRenderTarget, iType, srcDefaultRect, dstDefaultRect, RenderContext);
}

void
IOdysseyTextureRenderingAbility::RenderAndResize_GameThread(UTextureRenderTarget2D* iDstRenderTarget, uint64 iType, const FIntRect& iSrcRect, const FIntRect& iDstRect, const FOdysseyRenderContext* RenderContext) const
{
    const ERHIFeatureLevel::Type featureLevel = iDstRenderTarget->GetWorld() ? iDstRenderTarget->GetWorld()->GetFeatureLevel() : GMaxRHIFeatureLevel;

    IOdysseyTextureRenderingAbility::FRenderFunction renderFunction;
    FOdysseyRenderContext Request = RenderContext ? *RenderContext : FOdysseyRenderContext();
    Request.Type = iType;
    if (!BuildRenderPipeline(renderFunction, {}, &Request))
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

            FRDGTextureRef destinationTexture = graphBuilder.RegisterExternalTexture(
                CreateRenderTarget(
                    iDstRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture(),
                    TEXT( "UOdysseyAnimationCellThumbnailRenderer::destinationTexture" )
                )
            );

            AddClearRenderTargetPass( graphBuilder, destinationTexture, FLinearColor::Transparent, iDstRect );

            FRDGTextureDesc renderTextureDesc = FRDGTextureDesc::Create2D(
                iSrcRect.Size(),
                destinationTexture->Desc.Format,
                FClearValueBinding::Transparent,
                ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable,
                FMath::CeilLogTwo(FMath::Max(iSrcRect.Width(), iSrcRect.Height())) //NumMips
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

            FGenerateMips::Execute(graphBuilder, featureLevel, renderTexture);

            FOdysseyBlendShader::BlendRect(
                graphBuilder,
                featureLevel,
                destinationTexture,
                renderTexture,
                destinationTexture,
                iSrcRect,
                iDstRect,
                FMatrix::Identity,
                EOdysseyColorBlendMode::Normal,
                EOdysseyAlphaBlendMode::Normal,
                1.0f,
                EOdysseyAntiAliasing::Anisotropic
            );

            graphBuilder.Execute();
        }
    );
}

#undef LOCTEXT_NAMESPACE

bool IOdysseyTextureRenderingAbility::BuildRenderPipeline(FRenderFunction& OutRenderFunction,
    TArray<const IOdysseyTextureRenderingAbility*> Parents, const FOdysseyRenderContext* RenderContext) const
{
    OutRenderFunction = FRenderFunction();
    if (RenderContext && !RenderContext->ShouldRender(*this, Parents))
    {
        return false;
    }
    Parents.Add(this);
    const FOdysseyRenderOverride* Override = RenderContext ? RenderContext->Find(*this) : nullptr;
    if (Override && Override->RenderReplacement)
    {
        OutRenderFunction = Override->RenderReplacement;
    }
    else
    {
        BuildRenderPipelineInternal(OutRenderFunction, Parents, RenderContext);
    }
    if (Override && !Override->RenderReplacement && !BuildsCompositeContent())
    {
        if (Override->ContentReplacement)
        {
            const FMatrix ContentTransform = GetContentTransform(RenderContext);
            if (ContentTransform.Equals(FMatrix::Identity))
            {
                OutRenderFunction = Override->ContentReplacement;
            }
            else
            {
                OutRenderFunction = [Replacement = Override->ContentReplacement, ContentTransform](FRDGBuilder& GraphBuilder,
                    ERHIFeatureLevel::Type FeatureLevel, FRDGTextureRef Destination, const FIntRect& SourceRect,
                    const FIntRect& DestinationRect, const FMatrix& Transform)
                {
                    Replacement(GraphBuilder, FeatureLevel, Destination, SourceRect, DestinationRect, ContentTransform * Transform);
                };
            }
        }
        if (Override->ContentDecorator)
        {
            OutRenderFunction = Override->ContentDecorator(MoveTemp(OutRenderFunction));
        }
    }
    if (!BuildsCompositeContent() && !(Override && Override->RenderReplacement))
    {
        OutRenderFunction = BuildColorConvertRenderFunction(MoveTemp(OutRenderFunction), GetContentColorConversion(RenderContext));
    }
    if (Override && Override->RenderDecorator)
    {
        OutRenderFunction = Override->RenderDecorator(MoveTemp(OutRenderFunction));
    }
    return bool(OutRenderFunction);
}

IOdysseyTextureRenderingAbility::FRenderFunction
IOdysseyTextureRenderingAbility::BuildColorConvertRenderFunction(FRenderFunction RenderFunction, EOdysseyColorConversion Conversion)
{
    if (!RenderFunction)
    {
        return RenderFunction;
    }
    FOdysseyColorConversionPass Pass = FOdysseyColorSpaceShader::Prepare(Conversion);
    if (!Pass.IsValid())
    {
        return RenderFunction;
    }

    return [RenderFunction = MoveTemp(RenderFunction), Pass = MoveTemp(Pass)](FRDGBuilder& GraphBuilder, ERHIFeatureLevel::Type FeatureLevel,
        FRDGTextureRef Destination, const FIntRect& SourceRect, const FIntRect& DestinationRect, const FMatrix& Transform)
    {
        // Only the visible part of the destination area is converted.
        FIntRect Visible = DestinationRect;
        Visible.Clip(FIntRect(FIntPoint::ZeroValue, Destination->Desc.Extent));
        if (Visible.Width() <= 0 || Visible.Height() <= 0)
        {
            return;
        }

        // The intermediate covers the destination area only, not the whole destination: a node rendered into a small
        // area of a large canvas does not allocate a canvas-sized texture. The function renders into it at (0, 0).
        // Full float precision, so that the conversion is not altered by an intermediate quantization.
        const FIntRect IntermediateRect(FIntPoint::ZeroValue, DestinationRect.Size());
        FRDGTextureRef Intermediate = GraphBuilder.CreateTexture(FRDGTextureDesc::Create2D(DestinationRect.Size(),
            PF_A32B32G32R32F, FClearValueBinding::Transparent, TexCreate_ShaderResource | TexCreate_RenderTargetable),
            TEXT("Odyssey.ColorSpace.Intermediate"));
        AddClearRenderTargetPass(GraphBuilder, Intermediate, FLinearColor::Transparent);
        RenderFunction(GraphBuilder, FeatureLevel, Intermediate, SourceRect, IntermediateRect, Transform);
        Pass.Add(GraphBuilder, FeatureLevel, Intermediate, Visible - DestinationRect.Min, Destination, Visible);
    };
}
