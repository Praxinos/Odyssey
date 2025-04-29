// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyBlendingMode.h"
#include "OdysseyAntiAliasing.h"
#include "OdysseyImageAnchor.h"
#include "ShaderParameterMacros.h"

BEGIN_SHADER_PARAMETER_STRUCT(FOdysseyBlendShaderParameters, )
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SourceTexture)
    SHADER_PARAMETER_SAMPLER(SamplerState, SourceTextureSampler)

    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, DestinationTexture)
    SHADER_PARAMETER_SAMPLER(SamplerState, DestinationTextureSampler)

    SHADER_PARAMETER(FVector2f, SourcePosition)
    SHADER_PARAMETER(FVector2f, SourceSize)

    SHADER_PARAMETER(FVector2f, DestinationPosition)
    SHADER_PARAMETER(FVector2f, DestinationSize)

    SHADER_PARAMETER(float, Opacity)

    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class ODYSSEYRENDERING_API FOdysseyBlendShader : public FBatchedElementParameters
{
public:
    FOdysseyBlendShader(FOdysseyBlendShaderParameters* iPixelShaderParams, EOdysseyBlendingMode iBlendMode);

public:
    /** Binds vertex and pixel shaders for this element */
    virtual void BindShaders(FRHICommandList& RHICmdList, FGraphicsPipelineStateInitializer& GraphicsPSOInit, ERHIFeatureLevel::Type InFeatureLevel, const FMatrix& InTransform, const float InGamma, const FMatrix& ColorWeights, const FTexture* Texture) override;

public:
    static void Execute(
        FRDGBuilder& iGraphBuilder,
        ERHIFeatureLevel::Type iFeatureLevel,
        FRDGTextureRef iSourceTexture,
        FRDGTextureRef iDestinationTexture,

        FVector2D iPositionInDestination,
        FVector2D iPositionInSource,
        FVector2D iSizeInSource,
        FOdysseyImageAnchor iSourceHandlePosition,

        FVector2D iScale,
        float iRotationInDegrees,

        EOdysseyBlendingMode iBlendMode,
        float iOpacity,
        EOdysseyAntiAliasing iAntiAliasing
    );

public:
    /** Shader parameters */
    FOdysseyBlendShaderParameters* mPixelShaderParams;
    EOdysseyBlendingMode mBlendMode;
};
