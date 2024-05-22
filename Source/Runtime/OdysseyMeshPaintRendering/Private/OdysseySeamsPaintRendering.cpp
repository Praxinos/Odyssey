#include "OdysseySeamsPaintRendering.h"
#include "ShaderParameters.h"
#include "RenderResource.h"
#include "Shader.h"
#include "StaticBoundShaderState.h"
#include "RHIStaticStates.h"
#include "BatchedElements.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "TextureResource.h"
#include "Engine/TextureRenderTarget2D.h"
#include "PipelineStateCache.h"
#include "DataDrivenShaderPlatformInfo.h"

namespace OdysseySeamsPaintRendering
{

    /** Seams paint vertex shader */
    class TOdysseySeamsPaintVertexShader : public FGlobalShader
    {
        DECLARE_SHADER_TYPE(TOdysseySeamsPaintVertexShader, Global);

    public:

        static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
        {
            return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
        }

        /** Default constructor. */
        TOdysseySeamsPaintVertexShader() {}

        /** Initialization constructor. */
        TOdysseySeamsPaintVertexShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
            : FGlobalShader(Initializer)
        {
            TransformParameter.Bind(Initializer.ParameterMap, TEXT("c_Transform"));
        }

        void SetParameters(FRHIBatchedShaderParameters& BatchedParameters, const FMatrix44f& InTransform)
        {
            SetShaderValue(BatchedParameters, TransformParameter, InTransform);
        }

    private:
        LAYOUT_FIELD(FShaderParameter, TransformParameter);
    };


    IMPLEMENT_SHADER_TYPE(, TOdysseySeamsPaintVertexShader, TEXT("/Plugins/Iliad/Private/OdysseySeamsPaintShader.usf"), TEXT("MainVS"), SF_Vertex);


    /** Mesh paint pixel shader */
    class TOdysseySeamsPaintPixelShader : public FGlobalShader
    {
        DECLARE_SHADER_TYPE(TOdysseySeamsPaintPixelShader, Global);
    public:

        static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
        {
            return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
        }

        /** Default constructor. */
        TOdysseySeamsPaintPixelShader() {}

        /** Initialization constructor. */
        TOdysseySeamsPaintPixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
            : FGlobalShader(Initializer)
        {
            Stroke2DParameter.Bind(Initializer.ParameterMap, TEXT("s_Stroke2D"));
            SeamMaskParameter.Bind(Initializer.ParameterMap, TEXT("s_SeamMask"));
            Stroke2DParameterSampler.Bind(Initializer.ParameterMap, TEXT("s_Stroke2DSampler"));
            SeamMaskParameterSampler.Bind(Initializer.ParameterMap, TEXT("s_SeamMaskSampler"));
            WidthPixelOffsetParameter.Bind(Initializer.ParameterMap, TEXT("c_WidthPixelOffset"));
            HeightPixelOffsetParameter.Bind(Initializer.ParameterMap, TEXT("c_HeightPixelOffset"));
        }

        void SetParameters(FRHIBatchedShaderParameters& BatchedParameters, const float InGamma, const FOdysseySeamsPaintShaderParameters& InShaderParams)
        {
            SetTextureParameter(
                BatchedParameters,
                Stroke2DParameter,
                Stroke2DParameterSampler,
                TStaticSamplerState< SF_Trilinear, AM_Clamp, AM_Clamp, AM_Clamp >::GetRHI(),
                //InShaderParams.Stroke2D->Resource->TextureRHI);
                InShaderParams.Stroke2D->GetRenderTargetResource()->TextureRHI);

            SetTextureParameter(
                BatchedParameters,
                SeamMaskParameter,
                SeamMaskParameterSampler,
                TStaticSamplerState< SF_Trilinear, AM_Clamp, AM_Clamp, AM_Clamp >::GetRHI(),
                InShaderParams.SeamMaskRenderTarget->GetRenderTargetResource()->TextureRHI);

            SetShaderValue(BatchedParameters, WidthPixelOffsetParameter, InShaderParams.WidthPixelOffset);

            SetShaderValue(BatchedParameters, HeightPixelOffsetParameter, InShaderParams.HeightPixelOffset);
        }

    private:
        LAYOUT_FIELD(FShaderResourceParameter, Stroke2DParameterSampler);

        LAYOUT_FIELD(FShaderResourceParameter, SeamMaskParameterSampler);

        LAYOUT_FIELD(FShaderResourceParameter, Stroke2DParameter);

        LAYOUT_FIELD(FShaderResourceParameter, SeamMaskParameter);

        LAYOUT_FIELD(FShaderParameter, WidthPixelOffsetParameter);

        LAYOUT_FIELD(FShaderParameter, HeightPixelOffsetParameter);
    };

    IMPLEMENT_SHADER_TYPE(, TOdysseySeamsPaintPixelShader, TEXT("/Plugins/Iliad/Private/OdysseySeamsPaintShader.usf"), TEXT("MainPS"), SF_Pixel);

    typedef FSimpleElementVertexDeclaration FMeshPaintDilateVertexDeclaration;
    TGlobalResource< FMeshPaintDilateVertexDeclaration > GMeshPaintDilateVertexDeclaration;

    /** Binds the mesh paint vertex and pixel shaders to the graphics device */
    void SetSeamsPaintShaders(FRHICommandList& iRHICmdList, FGraphicsPipelineStateInitializer& iGraphicsPSOInit,
        ERHIFeatureLevel::Type iFeatureLevel,
        const FMatrix& iTransform,
        const float iGamma,
        const FOdysseySeamsPaintShaderParameters& iShaderParams)
    {
        TShaderMapRef< TOdysseySeamsPaintVertexShader > VertexShader(GetGlobalShaderMap(iFeatureLevel));
        TShaderMapRef< TOdysseySeamsPaintPixelShader > PixelShader(GetGlobalShaderMap(iFeatureLevel));

        iGraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GMeshPaintDilateVertexDeclaration.VertexDeclarationRHI;
        iGraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
        iGraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
        iGraphicsPSOInit.PrimitiveType = PT_TriangleList;

        SetGraphicsPipelineState(iRHICmdList, iGraphicsPSOInit, 0, EApplyRendertargetOption::ForceApply);

        SetShaderParametersLegacyVS(iRHICmdList, VertexShader, FMatrix44f(iTransform));
        SetShaderParametersLegacyPS(iRHICmdList, PixelShader, iGamma, iShaderParams);
    }
}

