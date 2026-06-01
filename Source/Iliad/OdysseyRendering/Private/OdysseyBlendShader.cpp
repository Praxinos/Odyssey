// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyBlendShader.h"

#include "RHICommandList.h"
#include "CanvasTypes.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphBuilder.h"
#include "TextureCompiler.h"
#include "SimpleElementShaders.h"
#include "SceneView.h"
#include "ScreenPass.h"
#include "MeshPassProcessor.h"

class FOdysseyBlendShaderPS : public FGlobalShader
{
public:
    DECLARE_SHADER_TYPE(FOdysseyBlendShaderPS, Global);
    SHADER_USE_PARAMETER_STRUCT(FOdysseyBlendShaderPS, FGlobalShader);

    using FParameters = FOdysseyBlendShaderParameters;

public:
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
    }
};

IMPLEMENT_SHADER_TYPE(, FOdysseyBlendShaderPS, TEXT("/OdysseyShaders/Private/OdysseyBlend.usf"), TEXT("MainPS"), SF_Pixel)

void FOdysseyBlendShader::BlendRect(
    FRDGBuilder& iGraphBuilder,
    ERHIFeatureLevel::Type iFeatureLevel,
    FRDGTextureRef iBackgroundTexture,
    FRDGTextureRef iForegroundTexture,
    FRDGTextureRef iDestinationTexture,

    const FIntRect& iSrcRect,
    const FIntRect& iDstRect,

    const FMatrix& iTransform,

    EOdysseyBlendingMode iBlendMode,
    EOdysseyAlphaMode iAlphaMode,
    float iOpacity,
    EOdysseyAntiAliasing iAntiAliasing
)
{
    FRDGTextureRef backgroundTexture = iBackgroundTexture;
    FRDGTextureRef foregroundTexture = iForegroundTexture;
    FRDGTextureRef destinationTexture = iDestinationTexture;

    if ( iBackgroundTexture == iDestinationTexture )
    {
        ETextureCreateFlags textureFlags = ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable;
        textureFlags |= iBackgroundTexture->Desc.Flags & ETextureCreateFlags::SRGB;
        FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
            destinationTexture->Desc.Extent,
            destinationTexture->Desc.Format,
            FClearValueBinding::Transparent,
            textureFlags
        );
        backgroundTexture = iGraphBuilder.CreateTexture(desc, TEXT("FOdysseyBlendShader::BackgroundTexture"));
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

    FSamplerStateRHIRef samplerStateRHI = Odyssey::GetSamplerStateForAntiAliasing(iAntiAliasing);

    //Alloc Shader Parameters
    FOdysseyBlendShaderParameters* shaderParameters = iGraphBuilder.AllocParameters<FOdysseyBlendShaderParameters>();
    shaderParameters->RenderTargets[0] = FRenderTargetBinding(destinationTexture, ERenderTargetLoadAction::ELoad);
    shaderParameters->SourceTexture = foregroundTexture;
    shaderParameters->SourceTextureSampler = samplerStateRHI;
    shaderParameters->DestinationTexture = backgroundTexture;
    shaderParameters->DestinationTextureSampler = samplerStateRHI;
    shaderParameters->Opacity = FMath::Clamp(iOpacity, 0.f, 1.f);
    shaderParameters->BlendMode = (uint32)iBlendMode;
    shaderParameters->AlphaMode = (uint32)iAlphaMode;

    //Create Shader
    TRefCountPtr< FOdysseyBlendShader > blendShader(new FOdysseyBlendShader(shaderParameters, iBlendMode));

    FIntPoint foregroundTextureSize = foregroundTexture->Desc.Extent;
    FIntPoint destinationTextureSize = destinationTexture->Desc.Extent;

    iGraphBuilder.AddPass(
        RDG_EVENT_NAME("OdysseyBlendShader"),
        shaderParameters,
        ERDGPassFlags::Raster,
        [iFeatureLevel, iDstRect, iSrcRect, foregroundTextureSize, destinationTextureSize, blendShader, iTransform](FRHICommandListImmediate& RHICmdList)
        {
            FBatchedElements blendBatchedElements;

            FVector srcTopLeft(float(iSrcRect.Min.X), float(iSrcRect.Min.Y), 0.f);
            FVector srcTopRight(float(iSrcRect.Max.X), float(iSrcRect.Min.Y), 0.f);
            FVector srcBottomLeft(float(iSrcRect.Min.X), float(iSrcRect.Max.Y), 0.f);
            FVector srcBottomRight(float(iSrcRect.Max.X), float(iSrcRect.Max.Y), 0.f);

            FVector dstTopLeft(float(iDstRect.Min.X), float(iDstRect.Min.Y), 0.f);
            FVector dstTopRight(float(iDstRect.Max.X), float(iDstRect.Min.Y), 0.f);
            FVector dstBottomLeft(float(iDstRect.Min.X), float(iDstRect.Max.Y), 0.f);
            FVector dstBottomRight(float(iDstRect.Max.X), float(iDstRect.Max.Y), 0.f);

            //dstTopLeft = iTransform.TransformPosition(dstTopLeft);
            //dstTopRight = iTransform.TransformPosition(dstTopRight);
            //dstBottomLeft = iTransform.TransformPosition(dstBottomLeft);
            //dstBottomRight = iTransform.TransformPosition(dstBottomRight);

            srcTopLeft = iTransform.Inverse().TransformPosition(srcTopLeft);
            srcTopRight = iTransform.Inverse().TransformPosition(srcTopRight);
            srcBottomLeft = iTransform.Inverse().TransformPosition(srcBottomLeft);
            srcBottomRight = iTransform.Inverse().TransformPosition(srcBottomRight);

            srcTopLeft /= FVector(foregroundTextureSize.X, foregroundTextureSize.Y, 1.f);
            srcTopRight /= FVector(foregroundTextureSize.X, foregroundTextureSize.Y, 1.f);
            srcBottomLeft /= FVector(foregroundTextureSize.X, foregroundTextureSize.Y, 1.f);
            srcBottomRight /= FVector(foregroundTextureSize.X, foregroundTextureSize.Y, 1.f);

            int32 topLeftVertex = blendBatchedElements.AddVertex(FVector4(dstTopLeft.X, dstTopLeft.Y, 0, 1), FVector2D(srcTopLeft.X, srcTopLeft.Y), FLinearColor::White, FHitProxyId());
            int32 topRightVertex = blendBatchedElements.AddVertex(FVector4(dstTopRight.X, dstTopRight.Y, 0, 1), FVector2D(srcTopRight.X, srcTopRight.Y), FLinearColor::White, FHitProxyId());
            int32 bottomLeftVertex = blendBatchedElements.AddVertex(FVector4(dstBottomLeft.X, dstBottomLeft.Y, 0, 1), FVector2D(srcBottomLeft.X, srcBottomLeft.Y), FLinearColor::White, FHitProxyId());
            int32 bottomRightVertex = blendBatchedElements.AddVertex(FVector4(dstBottomRight.X, dstBottomRight.Y, 0, 1), FVector2D(srcBottomRight.X, srcBottomRight.Y), FLinearColor::White, FHitProxyId());


            blendBatchedElements.AddTriangle(topLeftVertex, topRightVertex, bottomRightVertex, blendShader.GetReference(), SE_BLEND_Opaque);
            blendBatchedElements.AddTriangle(topLeftVertex, bottomRightVertex, bottomLeftVertex, blendShader.GetReference(), SE_BLEND_Opaque);

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

FOdysseyBlendShader::FOdysseyBlendShader(FOdysseyBlendShaderParameters* iPixelShaderParams, EOdysseyBlendingMode iBlendMode)
    : mPixelShaderParams(iPixelShaderParams)
    , mBlendMode(iBlendMode)
{
}

void
FOdysseyBlendShader::BindShaders(FRHICommandList& RHICmdList, FGraphicsPipelineStateInitializer& GraphicsPSOInit, ERHIFeatureLevel::Type InFeatureLevel, const FMatrix& InTransform, const float InGamma, const FMatrix& ColorWeights, const FTexture* Texture)
{
    static TGlobalResource< FSimpleElementVertexDeclaration > GBlendVertexDeclaration;
    TShaderMapRef< FSimpleElementVS > VertexShader(GetGlobalShaderMap(InFeatureLevel));
    TShaderMapRef< FOdysseyBlendShaderPS > PixelShader(GetGlobalShaderMap(InFeatureLevel));
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
