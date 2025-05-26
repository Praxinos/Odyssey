// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyBlendingMode.h"
#include "OdysseyAntiAliasing.h"
#include "OdysseyImageAnchor.h"
#include "BatchedElements.h"
#include "RHI.h"
#include "RenderGraphFwd.h"
#include "ShaderParameterMacros.h"

class FRDGBuilder;
class FTexture;

BEGIN_SHADER_PARAMETER_STRUCT(FOdysseyBlendColorShaderParameters, )
    SHADER_PARAMETER(FVector4f, Color)

    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, DestinationTexture)
    SHADER_PARAMETER_SAMPLER(SamplerState, DestinationTextureSampler)

    SHADER_PARAMETER(float, Opacity)
    SHADER_PARAMETER(uint32, BlendMode)
    SHADER_PARAMETER(uint32, AlphaMode)

    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class ODYSSEYRENDERING_API FOdysseyBlendColorShader : public FBatchedElementParameters
{
public:
    FOdysseyBlendColorShader(FOdysseyBlendColorShaderParameters* iPixelShaderParams, EOdysseyBlendingMode iBlendMode);

public:
    /** Binds vertex and pixel shaders for this element */
    virtual void BindShaders(FRHICommandList& RHICmdList, FGraphicsPipelineStateInitializer& GraphicsPSOInit, ERHIFeatureLevel::Type InFeatureLevel, const FMatrix& InTransform, const float InGamma, const FMatrix& ColorWeights, const FTexture* Texture) override;

public:
    static void BlendRect(
        FRDGBuilder& iGraphBuilder,
        ERHIFeatureLevel::Type iFeatureLevel,
        FRDGTextureRef iBackgroundTexture,
        FLinearColor iForegroundColor,
        FRDGTextureRef iDestinationTexture,
        const FIntRect& iDstRect,
        EOdysseyBlendingMode iBlendMode,
        EOdysseyAlphaMode iAlphaMode,
        float iOpacity
    );

public:
    /** Shader parameters */
    FOdysseyBlendColorShaderParameters* mPixelShaderParams;
    EOdysseyBlendingMode mBlendMode;
};
