// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyScreenPaintRendering.h"
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
#include "Engine/Texture2D.h"

namespace OdysseyScreenPaintRendering
{

    /** Mesh paint vertex shader */
    class ODYSSEYRENDERING_API TOdysseyScreenPaintVertexShader : public FGlobalShader
    {
        DECLARE_SHADER_TYPE(TOdysseyScreenPaintVertexShader, Global);

    public:

        static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
        {
            return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
        }

        /** Default constructor. */
        TOdysseyScreenPaintVertexShader() {}

        /** Initialization constructor. */
        TOdysseyScreenPaintVertexShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
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


    IMPLEMENT_SHADER_TYPE(, TOdysseyScreenPaintVertexShader, TEXT("/OdysseyShaders/Private/OdysseyScreenPaintShader.usf"), TEXT("MainVS"), SF_Vertex);


    /** Mesh paint pixel shader */
    class ODYSSEYRENDERING_API TOdysseyScreenPaintPixelShader : public FGlobalShader
    {
        DECLARE_SHADER_TYPE(TOdysseyScreenPaintPixelShader, Global);
    public:

        static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
        {
            return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
        }

        /** Default constructor. */
        TOdysseyScreenPaintPixelShader() {}

        /** Initialization constructor. */
        TOdysseyScreenPaintPixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
            : FGlobalShader(Initializer)
        {
            WorldToBrushMatrixParameter.Bind(Initializer.ParameterMap, TEXT("c_WorldToBrushMatrix"));
            Stroke2DParameter.Bind(Initializer.ParameterMap, TEXT("s_Stroke2D"));
            TextureHitPointParameter.Bind(Initializer.ParameterMap, TEXT("c_TextureHitPoint"));
            StampQualityParameter.Bind(Initializer.ParameterMap, TEXT("c_StampQuality"));
            xScreenAxisParameter.Bind(Initializer.ParameterMap, TEXT("c_xScreenAxis"));
            yScreenAxisParameter.Bind(Initializer.ParameterMap, TEXT("c_yScreenAxis"));
        }

        void SetParameters(FRHIBatchedShaderParameters& BatchedParameters, const float InGamma, const FOdysseyScreenPaintShaderParameters& InShaderParams)
        {
            SetTextureParameter(
                BatchedParameters,
                Stroke2DParameter,
                TextureParameterSampler,
                TStaticSamplerState< SF_Trilinear, AM_Clamp, AM_Clamp, AM_Clamp >::GetRHI(),
                InShaderParams.Stroke2D->GetResource()->GetTextureRHI());
            //InShaderParams.Stroke2D->GetRenderTargetResource()->TextureRHI);

            SetShaderValue(BatchedParameters, WorldToBrushMatrixParameter, (FMatrix44f)InShaderParams.WorldToBrushMatrix);

            SetShaderValue(BatchedParameters, TextureHitPointParameter, (FVector2f)InShaderParams.TextureHitPoint);

            SetShaderValue(BatchedParameters, StampQualityParameter, InShaderParams.StampQuality);

            SetShaderValue(BatchedParameters, xScreenAxisParameter, (FVector3f)InShaderParams.xScreenAxis);

            SetShaderValue(BatchedParameters, yScreenAxisParameter, (FVector3f)InShaderParams.yScreenAxis);
        }

    private:
        /** Sampler Texture that is a clone of the destination render target before we start drawing */
        LAYOUT_FIELD(FShaderResourceParameter, TextureParameterSampler);

        /** Reference colors for the application in 3D */
        LAYOUT_FIELD(FShaderResourceParameter, Stroke2DParameter);

        /** Brush -> World matrix */
        LAYOUT_FIELD(FShaderParameter, WorldToBrushMatrixParameter);

        LAYOUT_FIELD(FShaderParameter, TextureHitPointParameter);

        LAYOUT_FIELD(FShaderParameter, StampQualityParameter);

        LAYOUT_FIELD(FShaderParameter, xScreenAxisParameter);

        LAYOUT_FIELD(FShaderParameter, yScreenAxisParameter);
    };

    IMPLEMENT_SHADER_TYPE(, TOdysseyScreenPaintPixelShader, TEXT("/OdysseyShaders/Private/OdysseyScreenPaintShader.usf"), TEXT("MainPS"), SF_Pixel);

    typedef FSimpleElementVertexDeclaration FMeshPaintDilateVertexDeclaration;
    TGlobalResource< FMeshPaintDilateVertexDeclaration > GMeshPaintDilateVertexDeclaration;

    /** Binds the mesh paint vertex and pixel shaders to the graphics device */
    void SetMeshPaintShaders(FRHICommandList& iRHICmdList, FGraphicsPipelineStateInitializer& iGraphicsPSOInit,
        ERHIFeatureLevel::Type iFeatureLevel,
        const FMatrix& iTransform,
        const float iGamma,
        const FOdysseyScreenPaintShaderParameters& iShaderParams)
    {
        TShaderMapRef< TOdysseyScreenPaintVertexShader > VertexShader(GetGlobalShaderMap(iFeatureLevel));
        TShaderMapRef< TOdysseyScreenPaintPixelShader > PixelShader(GetGlobalShaderMap(iFeatureLevel));

        iGraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GMeshPaintDilateVertexDeclaration.VertexDeclarationRHI;
        iGraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
        iGraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
        iGraphicsPSOInit.PrimitiveType = PT_TriangleList;

        SetGraphicsPipelineState(iRHICmdList, iGraphicsPSOInit, 0, EApplyRendertargetOption::ForceApply);

        SetShaderParametersLegacyVS(iRHICmdList, VertexShader, FMatrix44f(iTransform));
        SetShaderParametersLegacyPS(iRHICmdList, PixelShader, iGamma, iShaderParams);
    }
}
