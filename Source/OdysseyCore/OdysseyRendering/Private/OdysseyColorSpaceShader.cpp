// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyColorSpaceShader.h"

#include "GlobalShader.h"
#include "PixelShaderUtils.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ShaderParameterStruct.h"

namespace
{
BEGIN_SHADER_PARAMETER_STRUCT(FOdysseyColorSpaceShaderParameters, )
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SourceTexture)
    SHADER_PARAMETER(uint32, Conversion)
    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()
}

class FOdysseyColorSpaceShaderPS : public FGlobalShader
{
public:
    DECLARE_SHADER_TYPE(FOdysseyColorSpaceShaderPS, Global);
    SHADER_USE_PARAMETER_STRUCT(FOdysseyColorSpaceShaderPS, FGlobalShader);
    using FParameters = FOdysseyColorSpaceShaderParameters;

public:
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
    }
};

IMPLEMENT_SHADER_TYPE(, FOdysseyColorSpaceShaderPS, TEXT("/OdysseyShaders/Private/OdysseyColorSpace.usf"), TEXT("MainPS"), SF_Pixel)

void
FOdysseyColorSpaceShader::Convert(FRDGBuilder& GraphBuilder, ERHIFeatureLevel::Type FeatureLevel, FRDGTextureRef Source,
    FRDGTextureRef Destination, const FIntRect& Rect, EOdysseyColorConversion Conversion)
{
    check(Source != Destination);
    FIntRect Viewport = Rect;
    Viewport.Clip(FIntRect(FIntPoint::ZeroValue, Destination->Desc.Extent));
    if (Conversion == EOdysseyColorConversion::None || Viewport.Width() <= 0 || Viewport.Height() <= 0)
    {
        return;
    }

    FOdysseyColorSpaceShaderParameters* Parameters = GraphBuilder.AllocParameters<FOdysseyColorSpaceShaderParameters>();
    Parameters->SourceTexture = Source;
    Parameters->Conversion = Conversion == EOdysseyColorConversion::Encode ? 0 : 1;
    Parameters->RenderTargets[0] = FRenderTargetBinding(Destination, ERenderTargetLoadAction::ELoad);

    FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
    TShaderMapRef<FOdysseyColorSpaceShaderPS> PixelShader(GlobalShaderMap);
    FPixelShaderUtils::AddFullscreenPass(GraphBuilder, GlobalShaderMap, RDG_EVENT_NAME("OdysseyColorSpace"),
        PixelShader, Parameters, Viewport);
}

IOdysseyTextureRenderingAbility::FRenderFunction
FOdysseyColorSpaceShader::WrapRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction RenderFunction, EOdysseyColorConversion Conversion)
{
    if (!RenderFunction || Conversion == EOdysseyColorConversion::None)
    {
        return RenderFunction;
    }

    return [RenderFunction = MoveTemp(RenderFunction), Conversion](FRDGBuilder& GraphBuilder, ERHIFeatureLevel::Type FeatureLevel,
        FRDGTextureRef Destination, const FIntRect& SourceRect, const FIntRect& DestinationRect, const FMatrix& Transform)
    {
        // Full float precision, so that the conversion is not altered by an intermediate quantization.
        FRDGTextureRef Intermediate = GraphBuilder.CreateTexture(FRDGTextureDesc::Create2D(Destination->Desc.Extent,
            PF_A32B32G32R32F, FClearValueBinding::Transparent, TexCreate_ShaderResource | TexCreate_RenderTargetable),
            TEXT("Odyssey.ColorSpace.Intermediate"));
        AddClearRenderTargetPass(GraphBuilder, Intermediate, FLinearColor::Transparent, DestinationRect);
        RenderFunction(GraphBuilder, FeatureLevel, Intermediate, SourceRect, DestinationRect, Transform);
        Convert(GraphBuilder, FeatureLevel, Intermediate, Destination, DestinationRect, Conversion);
    };
}
