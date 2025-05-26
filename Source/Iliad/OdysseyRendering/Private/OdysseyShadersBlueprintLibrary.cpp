// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyShadersBlueprintLibrary.h"

#include "OdysseyBlendShader.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "RenderGraphBuilder.h"
#include "RHI.h"
#include "TextureResource.h"

void
UOdysseyShadersBlueprintLibrary::Blend(
    const UObject* WorldContextObject,
    UTexture* BackgroundTexture,
    UTexture* ForegroundTexture,
    UTextureRenderTarget2D* DestinationTexture,
    FIntRect SourceRect,
    FIntRect DestinationRect,
    FMatrix Transform,
    EOdysseyBlendingMode BlendMode,
    EOdysseyAlphaMode AlphaMode,
    float Opacity,
    EOdysseyAntiAliasing AntiAliasing
)
{
    if (!BackgroundTexture || !ForegroundTexture || !DestinationTexture)
        return;

    const ERHIFeatureLevel::Type featureLevel = WorldContextObject->GetWorld() ? WorldContextObject->GetWorld()->GetFeatureLevel() : GMaxRHIFeatureLevel;

    ENQUEUE_RENDER_COMMAND(UpdateOdysseyScreenPaintRTCommandSeams)(
        [=](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);

            //Register Textures in iGraphBuilder
            FRDGTextureRef backgroundTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(BackgroundTexture->GetResource()->TextureRHI, TEXT("Odyssey::Blend::BackgroundTexture")));
            FRDGTextureRef foregroundTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(ForegroundTexture->GetResource()->TextureRHI, TEXT("Odyssey::Blend::ForegroundTexture")));
            FRDGTextureRef destinationTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(DestinationTexture->GetRenderTargetResource()->GetRenderTargetTexture(), TEXT("Odyssey::Blend::DestinationTexture")));

            FIntRect sourceRect = SourceRect;
            sourceRect.Clip(FIntRect(0, 0, BackgroundTexture->GetResource()->GetSizeX(), BackgroundTexture->GetResource()->GetSizeY()));

            FOdysseyBlendShader::BlendRect(
                graphBuilder,
                featureLevel,
                backgroundTexture,
                foregroundTexture,
                destinationTexture,

                SourceRect,
                DestinationRect,

                Transform,

                BlendMode,
                AlphaMode,
                Opacity,
                AntiAliasing
            );

            graphBuilder.Execute();
        }
    );
}
