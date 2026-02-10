// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "BatchedElements.h"
#include "RenderGraphFwd.h"
#include "OdysseyBlendingMode.h"
#include "OdysseyAntiAliasing.h"
#include "ShaderParameterMacros.h"

class FRDGBuilder;

BEGIN_SHADER_PARAMETER_STRUCT(FOdysseyScanCleanerShaderParameters, )
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SourceTexture)
    SHADER_PARAMETER_SAMPLER(SamplerState, SourceTextureSampler)

    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, AdjustCurveTexture)
    SHADER_PARAMETER_SAMPLER(SamplerState, AdjustCurveTextureSampler)

    SHADER_PARAMETER(float, ColorSaturation)
    SHADER_PARAMETER(float, ColorValue)

    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class ODYSSEYRENDERING_API FOdysseyScanCleanerShader
{
public:
    static void ScanCleaner(
        FRDGBuilder& iGraphBuilder,
        ERHIFeatureLevel::Type iFeatureLevel,
        FRDGTextureRef iSourceTexture,
        FRDGTextureRef iAdjustCurveTexture,
        FRDGTextureRef iDestinationTexture,
        float iColorSaturation,
        float iColorValue
    );

    static void InitTextureFromCurves(
        UTexture2D* iTexture,
        const FRealCurve& iCurve
    );
};
