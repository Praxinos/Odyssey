// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyBlendColorShader.h"

#include "RHICommandList.h"
#include "CanvasTypes.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphBuilder.h"
#include "TextureCompiler.h"
#include "SimpleElementShaders.h"
#include "ScreenPass.h"
#include "MeshPassProcessor.h"
#include "BatchedElements.h"

class FOdysseyBlendColorShaderPS : public FGlobalShader
{
public:
    DECLARE_SHADER_TYPE(FOdysseyBlendColorShaderPS, Global);
    SHADER_USE_PARAMETER_STRUCT(FOdysseyBlendColorShaderPS, FGlobalShader);

    using FParameters = FOdysseyBlendColorShaderParameters;

public:
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
    }
};

IMPLEMENT_SHADER_TYPE(, FOdysseyBlendColorShaderPS, TEXT("/OdysseyShaders/Private/OdysseyBlendColor.usf"), TEXT("MainPS"), SF_Pixel)

void FOdysseyBlendColorShader::BlendRect(
    FRDGBuilder& iGraphBuilder,
    ERHIFeatureLevel::Type iFeatureLevel,
    FRDGTextureRef iBackgroundTexture,
    FLinearColor iForegroundColor,
    FRDGTextureRef iDestinationTexture,
    const FIntRect& iDstRect,
    EOdysseyBlendingMode iBlendMode,
    EOdysseyAlphaMode iAlphaMode,
    float iOpacity
)
{
    FRDGTextureRef backgroundTexture = iBackgroundTexture;
    FRDGTextureRef destinationTexture = iDestinationTexture;

    if ( iBackgroundTexture == iDestinationTexture )
    {
        FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
            destinationTexture->Desc.Extent,
            destinationTexture->Desc.Format,
            FClearValueBinding::Transparent,
            ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
        );
        backgroundTexture = iGraphBuilder.CreateTexture(desc, TEXT("FOdysseyBlendColorShader::BackgroundTexture"));
        AddClearRenderTargetPass(iGraphBuilder, backgroundTexture, FLinearColor::Transparent, iDstRect);

        //Copy Destination Texture to Background Texture
        AddCopyTexturePass(
            iGraphBuilder,
            destinationTexture,
            backgroundTexture,
            iDstRect.Min,
            iDstRect.Min,
            iDstRect.Size()
        );
    }

    FSamplerStateRHIRef samplerStateRHI = Odyssey::GetSamplerStateForAntiAliasing(EOdysseyAntiAliasing::NearestNeighbor);

    //Alloc Shader Parameters
    FOdysseyBlendColorShaderParameters* shaderParameters = iGraphBuilder.AllocParameters<FOdysseyBlendColorShaderParameters>();
    shaderParameters->RenderTargets[0] = FRenderTargetBinding(destinationTexture, ERenderTargetLoadAction::ELoad);
    shaderParameters->Color = FVector4f(iForegroundColor.R, iForegroundColor.G, iForegroundColor.B, iForegroundColor.A);
    shaderParameters->DestinationTexture = backgroundTexture;
    shaderParameters->DestinationTextureSampler = samplerStateRHI;
    shaderParameters->Opacity = FMath::Clamp(iOpacity, 0.f, 1.f);
    shaderParameters->BlendMode = (uint32)iBlendMode;
    shaderParameters->AlphaMode = (uint32)iAlphaMode;

    //Create Shader
    TRefCountPtr< FOdysseyBlendColorShader > blendColorShader(new FOdysseyBlendColorShader(shaderParameters, iBlendMode));

    FIntPoint destinationTextureSize = destinationTexture->Desc.Extent;

    iGraphBuilder.AddPass(
        RDG_EVENT_NAME("OdysseyBlendColorShader"),
        shaderParameters,
        ERDGPassFlags::Raster,
        [iFeatureLevel, iDstRect, destinationTextureSize, blendColorShader](FRHICommandListImmediate& RHICmdList)
        {
            FBatchedElements blendBatchedElements;

            double x = iDstRect.Min.X;
            double y = iDstRect.Min.Y;
            double w = iDstRect.Width();
            double h = iDstRect.Height();
            float u0 = float(iDstRect.Min.X) / destinationTextureSize.X;
            float v0 = float(iDstRect.Min.Y) / destinationTextureSize.Y;
            float u1 = float(iDstRect.Max.X) / destinationTextureSize.X;
            float v1 = float(iDstRect.Max.Y) / destinationTextureSize.Y;

            int32 topLeftVertex = blendBatchedElements.AddVertex(FVector4(x, y, 0, 1), FVector2D(u0, v0), FLinearColor::White, FHitProxyId());
            int32 topRightVertex = blendBatchedElements.AddVertex(FVector4(x + w, y, 0, 1), FVector2D(u1, v0), FLinearColor::White, FHitProxyId());
            int32 bottomLeftVertex = blendBatchedElements.AddVertex(FVector4(x, y + h, 0, 1), FVector2D(u0, v1), FLinearColor::White, FHitProxyId());
            int32 bottomRightVertex = blendBatchedElements.AddVertex(FVector4(x + w, y + h, 0, 1), FVector2D(u1, v1), FLinearColor::White, FHitProxyId());

            blendBatchedElements.AddTriangle(topLeftVertex, topRightVertex, bottomRightVertex, blendColorShader.GetReference(), SE_BLEND_Opaque);
            blendBatchedElements.AddTriangle(topLeftVertex, bottomRightVertex, bottomLeftVertex, blendColorShader.GetReference(), SE_BLEND_Opaque);

            FMeshPassProcessorRenderState DrawRenderState;
            DrawRenderState.SetDepthStencilState(TStaticDepthStencilState<false, CF_Always>::GetRHI());

            // Guard against division by zero.
            uint32 ViewSizeX = FMath::Max<uint32>(destinationTextureSize.X, 1.f);
            uint32 ViewSizeY = FMath::Max<uint32>(destinationTextureSize.Y, 1.f);

            FMatrix transform = AdjustProjectionMatrixForRHI(
                FTranslationMatrix(FVector(0, 0, 0)) *
                FMatrix(
                FPlane(1.0f / (ViewSizeX / 2.0f), 0.0, 0.0f, 0.0f),
                FPlane(0.0f, -1.0f / (ViewSizeY / 2.0f), 0.0f, 0.0f),
                FPlane(0.0f, 0.0f, 1.0f, 0.0f),
                FPlane(-1.0f, 1.0f, 0.0f, 1.0f)
            )
            );

            FSceneView proxySceneView = FBatchedElements::CreateProxySceneView(transform, FIntRect(0, 0, destinationTextureSize.X, destinationTextureSize.Y));

            blendBatchedElements.Draw(
                RHICmdList,
                DrawRenderState,
                iFeatureLevel,
                proxySceneView,
                false
            );
        }
    );
}

FOdysseyBlendColorShader::FOdysseyBlendColorShader(FOdysseyBlendColorShaderParameters* iPixelShaderParams, EOdysseyBlendingMode iBlendMode)
    : mPixelShaderParams(iPixelShaderParams)
    , mBlendMode(iBlendMode)
{
}

void
FOdysseyBlendColorShader::BindShaders(FRHICommandList& RHICmdList, FGraphicsPipelineStateInitializer& GraphicsPSOInit, ERHIFeatureLevel::Type InFeatureLevel, const FMatrix& InTransform, const float InGamma, const FMatrix& ColorWeights, const FTexture* Texture)
{
    static TGlobalResource< FSimpleElementVertexDeclaration > GBlendVertexDeclaration;
    TShaderMapRef< FSimpleElementVS > VertexShader(GetGlobalShaderMap(InFeatureLevel));
    TShaderMapRef< FOdysseyBlendColorShaderPS > PixelShader(GetGlobalShaderMap(InFeatureLevel));
    GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GBlendVertexDeclaration.VertexDeclarationRHI;
    GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
    GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
    GraphicsPSOInit.PrimitiveType = PT_TriangleList;
    GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
    RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
    SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0, EApplyRendertargetOption::CheckApply);
    SetShaderParametersLegacyVS(RHICmdList, VertexShader, InTransform );
    SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), *mPixelShaderParams);
}
