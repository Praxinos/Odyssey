// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyScanCleanerShader.h"

#include "RHICommandList.h"
#include "CanvasTypes.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphBuilder.h"
#include "TextureCompiler.h"
#include "SimpleElementShaders.h"
#include "ScreenPass.h"
#include "MeshPassProcessor.h"
#include "PixelShaderUtils.h"
#include "Engine/TextureRenderTarget2D.h"
#include "TextureResource.h"

class FOdysseyScanCleanerShaderPS : public FGlobalShader
{
public:
    DECLARE_SHADER_TYPE(FOdysseyScanCleanerShaderPS, Global);
    SHADER_USE_PARAMETER_STRUCT(FOdysseyScanCleanerShaderPS, FGlobalShader);

    using FParameters = FOdysseyScanCleanerShaderParameters;

public:
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
    }
};

IMPLEMENT_SHADER_TYPE(, FOdysseyScanCleanerShaderPS, TEXT("/OdysseyShaders/Private/OdysseyScanCleaner.usf"), TEXT("MainPS"), SF_Pixel)

void FOdysseyScanCleanerShader::ScanCleaner(
    FRDGBuilder& iGraphBuilder,
    ERHIFeatureLevel::Type iFeatureLevel,
    FRDGTextureRef iSourceTexture,
    FRDGTextureRef iAdjustCurveTexture,
    FRDGTextureRef iDestinationTexture,
    float iColorSaturation,
    float iColorValue
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
        sourceTexture = iGraphBuilder.CreateTexture(desc, TEXT("FOdysseyScanCleanerShader::SourceTexture"));
        AddClearRenderTargetPass(iGraphBuilder, sourceTexture, FLinearColor::Transparent);

        //Copy Destination Texture to Background Texture
        AddCopyTexturePass(
            iGraphBuilder,
            destinationTexture,
            sourceTexture
        );
    }

    //Alloc Shader Parameters
    FOdysseyScanCleanerShaderParameters* shaderParameters = iGraphBuilder.AllocParameters<FOdysseyScanCleanerShaderParameters>();
    shaderParameters->RenderTargets[0] = FRenderTargetBinding(destinationTexture, ERenderTargetLoadAction::ELoad);
    shaderParameters->SourceTexture = sourceTexture;
    shaderParameters->SourceTextureSampler = Odyssey::GetSamplerStateForAntiAliasing(EOdysseyAntiAliasing::NearestNeighbor);
    shaderParameters->AdjustCurveTexture = adjustCurveTexture;
    shaderParameters->AdjustCurveTextureSampler = Odyssey::GetSamplerStateForAntiAliasing(EOdysseyAntiAliasing::Bilinear);
    shaderParameters->ColorSaturation = iColorSaturation;
    shaderParameters->ColorValue = iColorValue;

    //Create Shader
    FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(iFeatureLevel);
    TShaderMapRef< FOdysseyScanCleanerShaderPS > PixelShader(GlobalShaderMap);

    FPixelShaderUtils::AddFullscreenPass(
        iGraphBuilder,
        GlobalShaderMap,
        RDG_EVENT_NAME("OdysseyScanCleanerShader"),
        PixelShader,
        shaderParameters,
        FIntRect(0, 0, destinationTexture->Desc.Extent.X, destinationTexture->Desc.Extent.Y)
    );
}

void
FOdysseyScanCleanerShader::InitTextureFromCurves(
    UTextureRenderTarget2D* iRenderTarget,
    const FRealCurve& iCurve
)
{
    iRenderTarget->RenderTargetFormat = RTF_R32f;
    iRenderTarget->ClearColor = FLinearColor::Transparent;
    iRenderTarget->bForceLinearGamma = true;
    iRenderTarget->bAutoGenerateMips = false;
    iRenderTarget->InitAutoFormat(1024,1);
    iRenderTarget->UpdateResourceImmediate(true);


    FTextureRenderTargetResource* RTResource = iRenderTarget->GameThread_GetRenderTargetResource();

    TArray<float> PixelData;
    PixelData.AddUninitialized(1024);
    for (int i = 0; i < 1024; i++)
    {
        float pos = i/1024.f;

        float& destPixel = PixelData[i];
        destPixel = iCurve.Eval(pos);
    }

    FUpdateTextureRegion2D Region(0, 0, 0, 0, 1024, 1);

    ENQUEUE_RENDER_COMMAND(UpdateRTCommand)(
        [RTResource, PixelData, Region](FRHICommandListImmediate& RHICmdList)
        {
            RHIUpdateTexture2D(
                RTResource->GetRenderTargetTexture(),
                0,
                Region,
                Region.Width * sizeof(FLinearColor),
                (uint8*)PixelData.GetData()
            );
        }
    );
}
