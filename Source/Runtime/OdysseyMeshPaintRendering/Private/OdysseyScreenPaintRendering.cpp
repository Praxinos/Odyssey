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

namespace OdysseyScreenPaintRendering
{

    /** Mesh paint vertex shader */
    class TOdysseyScreenPaintVertexShader : public FGlobalShader
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

        void SetParameters(FRHICommandList& RHICmdList, const FMatrix& InTransform)
        {
            SetShaderValue(RHICmdList, RHICmdList.GetBoundVertexShader(), TransformParameter, (FMatrix44f)InTransform);
        }

    private:
        LAYOUT_FIELD(FShaderParameter, TransformParameter);
    };


    IMPLEMENT_SHADER_TYPE(, TOdysseyScreenPaintVertexShader, TEXT("/Plugins/Iliad/Private/OdysseyScreenPaintShader.usf"), TEXT("MainVS"), SF_Vertex);


    /** Mesh paint pixel shader */
    class TOdysseyScreenPaintPixelShader : public FGlobalShader
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

        void SetParameters(FRHICommandList& RHICmdList, const float InGamma, const FOdysseyScreenPaintShaderParameters& InShaderParams)
        {
            FRHIPixelShader* ShaderRHI = RHICmdList.GetBoundPixelShader();

            SetTextureParameter(
                RHICmdList,
                ShaderRHI,
                Stroke2DParameter,
                TextureParameterSampler,
                TStaticSamplerState< SF_Trilinear, AM_Clamp, AM_Clamp, AM_Clamp >::GetRHI(),
                InShaderParams.Stroke2D->GetResource()->GetTextureRHI());
            //InShaderParams.Stroke2D->GetRenderTargetResource()->TextureRHI);

            SetShaderValue(RHICmdList, ShaderRHI, WorldToBrushMatrixParameter, (FMatrix44f)InShaderParams.WorldToBrushMatrix);

            SetShaderValue(RHICmdList, ShaderRHI, TextureHitPointParameter, (FVector2f)InShaderParams.TextureHitPoint);

            SetShaderValue(RHICmdList, ShaderRHI, StampQualityParameter, InShaderParams.StampQuality);

            SetShaderValue(RHICmdList, ShaderRHI, xScreenAxisParameter, (FVector3f)InShaderParams.xScreenAxis);

            SetShaderValue(RHICmdList, ShaderRHI, yScreenAxisParameter, (FVector3f)InShaderParams.yScreenAxis);
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

    IMPLEMENT_SHADER_TYPE(, TOdysseyScreenPaintPixelShader, TEXT("/Plugins/Iliad/Private/OdysseyScreenPaintShader.usf"), TEXT("MainPS"), SF_Pixel);

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

        SetGraphicsPipelineState(iRHICmdList, iGraphicsPSOInit, EApplyRendertargetOption::ForceApply);

        // Set vertex shader parameters
        VertexShader->SetParameters(iRHICmdList, iTransform);

        // Set pixel shader parameters
        PixelShader->SetParameters(iRHICmdList, iGamma, iShaderParams);
    }
}

