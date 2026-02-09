// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAdjustRGBAShader.h"

#include "RHICommandList.h"
#include "CanvasTypes.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphBuilder.h"
#include "TextureCompiler.h"
#include "SimpleElementShaders.h"
#include "ScreenPass.h"
#include "MeshPassProcessor.h"
#include "PixelShaderUtils.h"

class FOdysseyAdjustRGBAShaderPS : public FGlobalShader
{
public:
    DECLARE_SHADER_TYPE(FOdysseyAdjustRGBAShaderPS, Global);
    SHADER_USE_PARAMETER_STRUCT(FOdysseyAdjustRGBAShaderPS, FGlobalShader);

    using FParameters = FOdysseyAdjustRGBAShaderParameters;

public:
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
    }
};

IMPLEMENT_SHADER_TYPE(, FOdysseyAdjustRGBAShaderPS, TEXT("/OdysseyShaders/Private/OdysseyAdjustRGBA.usf"), TEXT("MainPS"), SF_Pixel)

void FOdysseyAdjustRGBAShader::AdjustRGBA(
    FRDGBuilder& iGraphBuilder,
    ERHIFeatureLevel::Type iFeatureLevel,
    FRDGTextureRef iSourceTexture,
    FRDGTextureRef iAdjustCurveTexture,
    FRDGTextureRef iDestinationTexture
)
{
    FRDGTextureRef sourceTexture = iSourceTexture;
    FRDGTextureRef adjustCurveTexture = iAdjustCurveTexture;
    FRDGTextureRef destinationTexture = iDestinationTexture;

    if ( iSourceTexture == iDestinationTexture )
    {
        ETextureCreateFlags textureFlags = ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable;
        textureFlags |= iSourceTexture->Desc.Flags & ETextureCreateFlags::SRGB;
        FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
            destinationTexture->Desc.Extent,
            destinationTexture->Desc.Format,
            FClearValueBinding::Transparent,
            textureFlags
        );
        sourceTexture = iGraphBuilder.CreateTexture(desc, TEXT("FOdysseyAdjustRGBAShader::SourceTexture"));
        AddClearRenderTargetPass(iGraphBuilder, sourceTexture, FLinearColor::Transparent);

        //Copy Destination Texture to Background Texture
        AddCopyTexturePass(
            iGraphBuilder,
            destinationTexture,
            sourceTexture
        );
    }

    //Alloc Shader Parameters
    FOdysseyAdjustRGBAShaderParameters* shaderParameters = iGraphBuilder.AllocParameters<FOdysseyAdjustRGBAShaderParameters>();
    shaderParameters->RenderTargets[0] = FRenderTargetBinding(destinationTexture, ERenderTargetLoadAction::ELoad);
    shaderParameters->SourceTexture = sourceTexture;
    shaderParameters->SourceTextureSampler = Odyssey::GetSamplerStateForAntiAliasing(EOdysseyAntiAliasing::NearestNeighbor);
    shaderParameters->AdjustCurveTexture = adjustCurveTexture;
    shaderParameters->AdjustCurveTextureSampler = Odyssey::GetSamplerStateForAntiAliasing(EOdysseyAntiAliasing::Bilinear);

    //Create Shader
    FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(iFeatureLevel);
    TShaderMapRef< FOdysseyAdjustRGBAShaderPS > PixelShader(GlobalShaderMap);

    FPixelShaderUtils::AddFullscreenPass(
        iGraphBuilder,
        GlobalShaderMap,
        RDG_EVENT_NAME("OdysseyAdjustRGBAShader"),
        PixelShader,
        shaderParameters,
        FIntRect(0, 0, destinationTexture->Desc.Extent.X, destinationTexture->Desc.Extent.Y)
    );
}

void
FOdysseyAdjustRGBAShader::InitTextureFromCurves(
    UTexture2D* iTexture,
    const FRealCurve& iCurveR,
    const FRealCurve& iCurveG,
    const FRealCurve& iCurveB,
    const FRealCurve& iCurveA
)
{
    iTexture->SRGB = false;
    iTexture->Source.Init(1024, 1, 1, 1, TSF_RGBA32F, nullptr);
    FLinearColor* MipData = reinterpret_cast<FLinearColor*>(iTexture->Source.LockMip(0));

    //R Curve
    for (int i = 0; i < 1024; i++)
    {
        float pos = i/1024.f;

        FLinearColor& destPixel = MipData[i];
        destPixel.R = iCurveR.Eval(pos);
        destPixel.G = iCurveG.Eval(pos);
        destPixel.B = iCurveB.Eval(pos);
        destPixel.A = iCurveA.Eval(pos);
    }

    iTexture->Source.UnlockMip(0);
    iTexture->UpdateResource();
}
