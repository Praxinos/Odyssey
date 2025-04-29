// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyShadersBlueprintLibrary.h"

#include "OdysseyBlendShader.h"

#include "Engine\TextureRenderTarget2D.h"
#include "RenderGraphBuilder.h"

void
UOdysseyShadersBlueprintLibrary::Blend(
    const UObject* WorldContextObject,
    UTexture* SourceTexture,
    UTextureRenderTarget2D* DestinationTexture,
    FVector2D SourcePosition,
    FVector2D SourceSize,
    FOdysseyImageAnchor SourceAnchor,
    FVector2D Position,
    FVector2D Scale,
    float RotationInDegrees,
    EOdysseyBlendingMode BlendMode,
    float Opacity,
    EOdysseyAntiAliasing AntiAliasing
)
{
    if (!SourceTexture || !DestinationTexture)
        return;

    const ERHIFeatureLevel::Type featureLevel = WorldContextObject->GetWorld()->GetFeatureLevel();

    ENQUEUE_RENDER_COMMAND(UpdateOdysseyScreenPaintRTCommandSeams)(
        [=](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);

            FOdysseyBlendShader::Execute(
                graphBuilder,
                featureLevel,
                SourceTexture->GetResource(),
                DestinationTexture->GetRenderTargetResource(),

                Position,
                SourcePosition,
                SourceSize,
                SourceAnchor,

                Scale,
                RotationInDegrees,

                BlendMode,
                Opacity,
                AntiAliasing
            );

            graphBuilder.Execute();
        }
    );
}
