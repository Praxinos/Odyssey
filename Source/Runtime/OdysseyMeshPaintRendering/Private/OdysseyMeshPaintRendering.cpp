#include "OdysseyMeshPaintRendering.h"
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

namespace OdysseyMeshPaintRendering
{

    /** Mesh paint vertex shader */
    class TOdysseyMeshPaintVertexShader : public FGlobalShader
    {
        DECLARE_SHADER_TYPE(TOdysseyMeshPaintVertexShader, Global);

    public:

        static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
        {
            return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
        }

        /** Default constructor. */
        TOdysseyMeshPaintVertexShader() {}

        /** Initialization constructor. */
        TOdysseyMeshPaintVertexShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
            : FGlobalShader(Initializer)
        {
            TransformParameter.Bind(Initializer.ParameterMap, TEXT("c_Transform"));
        }

        void SetParameters(FRHIBatchedShaderParameters& BatchedParameters, const FMatrix44f& InTransform)
        {
            SetShaderValue(BatchedParameters, TransformParameter, (FMatrix44f)InTransform);
        }

    private:
        LAYOUT_FIELD(FShaderParameter, TransformParameter);
    };


    IMPLEMENT_SHADER_TYPE(, TOdysseyMeshPaintVertexShader, TEXT("/Plugins/Iliad/Private/OdysseyMeshPaintShader.usf"), TEXT("MainVS"), SF_Vertex);


    /** Mesh paint pixel shader */
    class TOdysseyMeshPaintPixelShader : public FGlobalShader
    {
        DECLARE_SHADER_TYPE(TOdysseyMeshPaintPixelShader, Global);
    public:

        static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
        {
            return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
        }

        /** Default constructor. */
        TOdysseyMeshPaintPixelShader() {}

        /** Initialization constructor. */
        TOdysseyMeshPaintPixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
            : FGlobalShader(Initializer)
        {
            WorldToBrushMatrixParameter.Bind(Initializer.ParameterMap, TEXT("c_WorldToBrushMatrix"));
            Stroke2DParameter.Bind(Initializer.ParameterMap, TEXT("s_Stroke2D"));
            TextureHitPointParameter.Bind(Initializer.ParameterMap, TEXT("c_TextureHitPoint"));
            StampQualityParameter.Bind(Initializer.ParameterMap, TEXT("c_StampQuality"));
        }

        void SetParameters(FRHIBatchedShaderParameters& BatchedParameters, const float InGamma, const FOdysseyMeshPaintShaderParameters& InShaderParams)
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
    };

    IMPLEMENT_SHADER_TYPE(, TOdysseyMeshPaintPixelShader, TEXT("/Plugins/Iliad/Private/OdysseyMeshPaintShader.usf"), TEXT("MainPS"), SF_Pixel);

    typedef FSimpleElementVertexDeclaration FMeshPaintDilateVertexDeclaration;
    TGlobalResource< FMeshPaintDilateVertexDeclaration > GMeshPaintDilateVertexDeclaration;

    /** Binds the mesh paint vertex and pixel shaders to the graphics device */
    void SetMeshPaintShaders(FRHICommandList& iRHICmdList, FGraphicsPipelineStateInitializer& iGraphicsPSOInit,
        ERHIFeatureLevel::Type iFeatureLevel,
        const FMatrix& iTransform,
        const float iGamma,
        const FOdysseyMeshPaintShaderParameters& iShaderParams)
    {
        TShaderMapRef< TOdysseyMeshPaintVertexShader > VertexShader(GetGlobalShaderMap(iFeatureLevel));
        TShaderMapRef< TOdysseyMeshPaintPixelShader > PixelShader(GetGlobalShaderMap(iFeatureLevel));

        iGraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GMeshPaintDilateVertexDeclaration.VertexDeclarationRHI;
        iGraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
        iGraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
        iGraphicsPSOInit.PrimitiveType = PT_TriangleList;

        SetGraphicsPipelineState(iRHICmdList, iGraphicsPSOInit, 0, EApplyRendertargetOption::ForceApply);

        SetShaderParametersLegacyVS(iRHICmdList, VertexShader, FMatrix44f(iTransform));
        SetShaderParametersLegacyPS(iRHICmdList, PixelShader, iGamma, iShaderParams);    
    }
}

