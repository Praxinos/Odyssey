// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyBlendShader.h"

#include "RHICommandList.h"
#include "CanvasTypes.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphBuilder.h"
#include "OdysseyCanvasUtils.h"
#include "TextureCompiler.h"
#include "SimpleElementShaders.h"

/* class FOdysseyBlendShaderVS : public FGlobalShader
{
    DECLARE_SHADER_TYPE(FOdysseyBlendShaderVS, Global);
public:

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
    }

    FOdysseyBlendShaderVS() {}

    FOdysseyBlendShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
        : FGlobalShader(Initializer)
    {
    }

    void SetParameters(FRHIBatchedShaderParameters& BatchedParameters)
    {
    }
}; */


template<EOdysseyBlendingMode tBlendMode>
class TOdysseyBlendShaderPS : public FGlobalShader
{
public:
    DECLARE_SHADER_TYPE(TOdysseyBlendShaderPS, Global);
    SHADER_USE_PARAMETER_STRUCT(TOdysseyBlendShaderPS, FGlobalShader);

    using FParameters = FOdysseyBlendShaderParameters;

public:
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
    }
};

//IMPLEMENT_SHADER_TYPE(, FOdysseyBlendShaderVS, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("MainVS"), SF_Vertex)

IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kNormal>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendNormalPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kTop>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendTopPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kBack>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendBackPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kBehind>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendBehindPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kDarken>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendDarkenPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kMultiply>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendMultiplyPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kColorBurn>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendColorBurnPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kLighten>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendLightenPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kAverage>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendAveragePS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kNegation>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendNegationPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kScreen>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendScreenPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kColorDodge>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendColorDodgePS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kAdd>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendAddPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kLinearDodge>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendLinearDodgePS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kSoftLight>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendSoftLightPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kLinearBurn>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendLinearBurnPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kHardLight>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendHardLightPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kOverlay>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendOverlayPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kVividLight>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendVividLightPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kLinearLight>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendLinearLightPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kPinLight>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendPinLightPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kHardMix>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendHardMixPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kDifference>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendDifferencePS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kExclusion>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendExclusionPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kSubstract>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendSubstractPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kDivide>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendDividePS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kPhoenix>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendPhoenixPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kReflect>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendReflectPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kGlow>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendGlowPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kHue>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendHuePS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kSaturation>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendSaturationPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kColor>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendColorPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, TOdysseyBlendShaderPS<EOdysseyBlendingMode::kLuminosity>, TEXT("/Plugins/Odyssey/Private/OdysseyBlend.usf"), TEXT("BlendLuminosityPS"), SF_Pixel)

TGlobalResource< FSimpleElementVertexDeclaration > GBlendVertexDeclaration;

void FOdysseyBlendShader::Execute(
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
)
{
    //Retrieve Anti Aliasing Sampler State
    FSamplerStateRHIRef samplerStateRHI = Odyssey::GetSamplerStateForAntiAliasing(iAntiAliasing);

    FVector2D scaledSourceSize = iSizeInSource * iScale;

    //Alloc Shader Parameters
    FOdysseyBlendShaderParameters* shaderParameters = iGraphBuilder.AllocParameters<FOdysseyBlendShaderParameters>();

    shaderParameters->SourceTexture = iSourceTexture;
    shaderParameters->SourceTextureSampler = samplerStateRHI;
    shaderParameters->DestinationTexture = iDestinationTexture;
    shaderParameters->DestinationTextureSampler = samplerStateRHI;

    shaderParameters->SourcePosition = FVector2f(iPositionInSource.X, iPositionInSource.Y);
    shaderParameters->SourceSize = FVector2f(iSizeInSource.X, iSizeInSource.Y);
    shaderParameters->DestinationPosition = FVector2f(iPositionInDestination.X, iPositionInDestination.Y);
    shaderParameters->DestinationSize = FVector2f(scaledSourceSize.X, scaledSourceSize.Y);

    shaderParameters->Opacity = FMath::Clamp(iOpacity, 0.f, 1.f);

    //Create Shader
    TRefCountPtr< FOdysseyBlendShader > blendShader(new FOdysseyBlendShader(shaderParameters, iBlendMode));

    //Create a Canvas to draw with the shader
    FCanvas* canvas = FCanvas::Create(iGraphBuilder, iDestinationTexture, nullptr, FGameTime(), iFeatureLevel);

    //Draw a Quad
    /*Odyssey::CanvasUtils::DrawTransformedQuad(
        canvas,
        blendShader,
        iPositionInDestination,
        iSizeInSource,
        iSourceHandlePosition,
        iScale,
        iRotationInDegrees,
        FVector2D(iDestinationTexture->Desc.GetSize().X, iDestinationTexture->Desc.GetSize().Y)
    );

    //Ask the canvas to initiate rendering of the quad
    canvas->Flush_RenderThread(iGraphBuilder, true); */
}

FOdysseyBlendShader::FOdysseyBlendShader(FOdysseyBlendShaderParameters* iPixelShaderParams, EOdysseyBlendingMode iBlendMode)
    : mPixelShaderParams(iPixelShaderParams)
    , mBlendMode(iBlendMode)
{
}

void
FOdysseyBlendShader::BindShaders(FRHICommandList& RHICmdList, FGraphicsPipelineStateInitializer& GraphicsPSOInit, ERHIFeatureLevel::Type InFeatureLevel, const FMatrix& InTransform, const float InGamma, const FMatrix& ColorWeights, const FTexture* Texture)
{
#define BINDSHADERS_BLENDMODE(BLENDMODE)                                                                                    \
    TShaderMapRef< FSimpleElementVS > VertexShader(GetGlobalShaderMap(InFeatureLevel));                                     \
    TShaderMapRef< TOdysseyBlendShaderPS< BLENDMODE > > PixelShader(GetGlobalShaderMap(InFeatureLevel));                    \
    GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GBlendVertexDeclaration.VertexDeclarationRHI;                   \
    GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();                                      \
    GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();                                         \
    GraphicsPSOInit.PrimitiveType = PT_TriangleList;                                                                        \
    GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();                                                             \
    SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0, EApplyRendertargetOption::CheckApply);                         \
    SetShaderParametersLegacyVS(RHICmdList, VertexShader, InTransform );                                                    \
    SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), *mPixelShaderParams);

    switch (mBlendMode)
    {
        case EOdysseyBlendingMode::kNormal:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kNormal)
        }
        break;
        case EOdysseyBlendingMode::kTop:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kTop)
        }
        break;
        case EOdysseyBlendingMode::kBack:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kBack)
        }
        break;
        case EOdysseyBlendingMode::kBehind:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kBehind)
        }
        break;
        case EOdysseyBlendingMode::kDarken:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kDarken)
        }
        break;
        case EOdysseyBlendingMode::kMultiply:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kMultiply)
        }
        break;
        case EOdysseyBlendingMode::kColorBurn:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kColorBurn)
        }
        break;
        case EOdysseyBlendingMode::kLighten:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kLighten)
        }
        break;
        case EOdysseyBlendingMode::kAverage:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kAverage)
        }
        break;
        case EOdysseyBlendingMode::kNegation:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kNegation)
        }
        break;
        case EOdysseyBlendingMode::kScreen:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kScreen)
        }
        break;
        case EOdysseyBlendingMode::kColorDodge:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kColorDodge)
        }
        break;
        case EOdysseyBlendingMode::kAdd:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kAdd)
        }
        break;
        case EOdysseyBlendingMode::kLinearDodge:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kLinearDodge)
        }
        break;
        case EOdysseyBlendingMode::kSoftLight:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kSoftLight)
        }
        break;
        case EOdysseyBlendingMode::kLinearBurn:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kLinearBurn)
        }
        break;
        case EOdysseyBlendingMode::kHardLight:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kHardLight)
        }
        break;
        case EOdysseyBlendingMode::kOverlay:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kOverlay)
        }
        break;
        case EOdysseyBlendingMode::kVividLight:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kVividLight)
        }
        break;
        case EOdysseyBlendingMode::kLinearLight:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kLinearLight)
        }
        break;
        case EOdysseyBlendingMode::kPinLight:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kPinLight)
        }
        break;
        case EOdysseyBlendingMode::kHardMix:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kHardMix)
        }
        break;
        case EOdysseyBlendingMode::kDifference:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kDifference)
        }
        break;
        case EOdysseyBlendingMode::kExclusion:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kExclusion)
        }
        break;
        case EOdysseyBlendingMode::kSubstract:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kSubstract)
        }
        break;
        case EOdysseyBlendingMode::kDivide:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kDivide)
        }
        break;
        case EOdysseyBlendingMode::kPhoenix:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kPhoenix)
        }
        break;
        case EOdysseyBlendingMode::kReflect:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kReflect)
        }
        break;
        case EOdysseyBlendingMode::kGlow:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kGlow)
        }
        break;
        case EOdysseyBlendingMode::kHue:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kHue)
        }
        break;
        case EOdysseyBlendingMode::kSaturation:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kSaturation)
        }
        break;
        case EOdysseyBlendingMode::kColor:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kColor)
        }
        break;
        case EOdysseyBlendingMode::kLuminosity:
        {
            BINDSHADERS_BLENDMODE(EOdysseyBlendingMode::kLuminosity)
        }
        break;
    }
}
