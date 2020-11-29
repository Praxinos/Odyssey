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

namespace OdysseyMeshPaintRendering
{

    /** Mesh paint vertex shader */
    class TOdysseyMeshPaintVertexShader: public FGlobalShader
    {
        DECLARE_SHADER_TYPE(TOdysseyMeshPaintVertexShader,Global);

    public:

        static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
        {
            return IsFeatureLevelSupported(Parameters.Platform,ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
        }

        /** Default constructor. */
        TOdysseyMeshPaintVertexShader() {}

        /** Initialization constructor. */
        TOdysseyMeshPaintVertexShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
            : FGlobalShader(Initializer)
        {
            TransformParameter.Bind(Initializer.ParameterMap,TEXT("c_Transform"));
        }

        void SetParameters(FRHICommandList& RHICmdList,const FMatrix& InTransform)
        {
            SetShaderValue(RHICmdList,RHICmdList.GetBoundVertexShader(),TransformParameter,InTransform);
        }

    private:
        LAYOUT_FIELD(FShaderParameter,TransformParameter);
    };


    IMPLEMENT_SHADER_TYPE(,TOdysseyMeshPaintVertexShader,TEXT("/Plugins/Iliad/Private/OdysseyMeshPaintShader.usf"),TEXT("MainVS"),SF_Vertex);


	/** Mesh paint pixel shader */
	class TOdysseyMeshPaintPixelShader : public FGlobalShader
	{
		DECLARE_SHADER_TYPE( TOdysseyMeshPaintPixelShader, Global );
	public:

		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
		{
			return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
		}

		/** Default constructor. */
		TOdysseyMeshPaintPixelShader() {}

		/** Initialization constructor. */
		TOdysseyMeshPaintPixelShader( const ShaderMetaType::CompiledShaderInitializerType& Initializer )
			: FGlobalShader( Initializer )
		{
			WorldToBrushMatrixParameter.Bind( Initializer.ParameterMap, TEXT( "c_WorldToBrushMatrix" ) );
            Stroke2DParameter.Bind(Initializer.ParameterMap,TEXT( "s_Stroke2D" ));
            TextureHitPointParameter.Bind(Initializer.ParameterMap,TEXT("c_TextureHitPoint"));
		}

		void SetParameters(FRHICommandList& RHICmdList, const float InGamma, const FOdysseyMeshPaintShaderParameters& InShaderParams )
		{
			FRHIPixelShader* ShaderRHI = RHICmdList.GetBoundPixelShader();

            SetTextureParameter(
                RHICmdList,
                ShaderRHI,
                Stroke2DParameter,
                TextureParameterSampler,
                TStaticSamplerState< SF_Point,AM_Clamp,AM_Clamp,AM_Clamp >::GetRHI(),
                InShaderParams.Stroke2D->Resource->TextureRHI);

			SetShaderValue(RHICmdList, ShaderRHI, WorldToBrushMatrixParameter, InShaderParams.WorldToBrushMatrix );

            SetShaderValue(RHICmdList,ShaderRHI,TextureHitPointParameter,InShaderParams.TextureHitPoint);
		}

	private:
		/** Sampler Texture that is a clone of the destination render target before we start drawing */
        LAYOUT_FIELD(FShaderResourceParameter,TextureParameterSampler);

        /** Reference colors for the application in 3D */
        LAYOUT_FIELD(FShaderResourceParameter,Stroke2DParameter);

		/** Brush -> World matrix */
		LAYOUT_FIELD(FShaderParameter, WorldToBrushMatrixParameter);

        LAYOUT_FIELD(FShaderParameter,TextureHitPointParameter);
	};

    IMPLEMENT_SHADER_TYPE( ,TOdysseyMeshPaintPixelShader,TEXT("/Plugins/Iliad/Private/OdysseyMeshPaintShader.usf"),TEXT("MainPS"),SF_Pixel);

    typedef FSimpleElementVertexDeclaration FMeshPaintDilateVertexDeclaration;
    TGlobalResource< FMeshPaintDilateVertexDeclaration > GMeshPaintDilateVertexDeclaration;

	/** Binds the mesh paint vertex and pixel shaders to the graphics device */
	void SetMeshPaintShaders( FRHICommandList& iRHICmdList, FGraphicsPipelineStateInitializer& iGraphicsPSOInit, 
                              ERHIFeatureLevel::Type iFeatureLevel, 
                              const FMatrix& iTransform,
                              const float iGamma,
							  const FOdysseyMeshPaintShaderParameters& iShaderParams )
	{
        TShaderMapRef< TOdysseyMeshPaintVertexShader > VertexShader(GetGlobalShaderMap(iFeatureLevel));
		TShaderMapRef< TOdysseyMeshPaintPixelShader > PixelShader(GetGlobalShaderMap(iFeatureLevel));

        iGraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GMeshPaintDilateVertexDeclaration.VertexDeclarationRHI;
        iGraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
		iGraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
		iGraphicsPSOInit.PrimitiveType = PT_TriangleList;

		SetGraphicsPipelineState(iRHICmdList, iGraphicsPSOInit, EApplyRendertargetOption::ForceApply);
		
        // Set vertex shader parameters
        VertexShader->SetParameters(iRHICmdList,iTransform);

		// Set pixel shader parameters
		PixelShader->SetParameters(iRHICmdList, iGamma, iShaderParams );
	}
}

