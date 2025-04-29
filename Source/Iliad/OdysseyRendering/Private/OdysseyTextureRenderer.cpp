// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyTextureRenderer.h"
#include "ScreenPass.h"
#include "CanvasItem.h"
#include "MeshPassProcessor.h"
#include "OdysseyBlendShader.h"

#define LOCTEXT_NAMESPACE "Imaging"

FOdysseyTextureRenderer::FOdysseyTextureRenderer()
{
    RootRenderPassId = FGuid::NewGuid();
    RenderPasses.Add(RootRenderPassId, {});
}

const FGuid&
FOdysseyTextureRenderer::GetRootPassId() const
{
    return RootRenderPassId;
}

const FOdysseyTextureRenderer::FRenderPass&
FOdysseyTextureRenderer::GetPass(const FGuid& iId) const
{
    check(RenderPasses.Contains(iId));
    return RenderPasses[iId];
}

FOdysseyTextureRenderer::FRenderPass&
FOdysseyTextureRenderer::GetPass(const FGuid& iId)
{
    check(RenderPasses.Contains(iId));
    return RenderPasses[iId];
}

void
FOdysseyTextureRenderer::Append(const FGuid& iParent, const FOdysseyTextureRenderer& iRenderer)
{
    check(RenderPasses.Contains(iParent));
    RenderPasses[iParent].Children.Append(iRenderer.GetPass(iRenderer.GetRootPassId()).Children);
    RenderPasses.Append(iRenderer.RenderPasses);
    RenderPasses.Remove(iRenderer.GetRootPassId());
}

FGuid
FOdysseyTextureRenderer::AddChild(const FGuid& iParent, EOdysseyBlendingMode iBlendingMode, float iOpacity, const FMatrix& iTransform, const FOnExecuteRenderPass& iOnExecuteRenderPass)
{
    check(RenderPasses.Contains(iParent));
    FGuid newId = FGuid::NewGuid();
    RenderPasses[iParent].Children.Add(newId);

    FRenderPass renderPass;
    renderPass.BlendingMode = iBlendingMode;
    renderPass.Opacity = iOpacity;
    renderPass.Transform = iTransform;
    renderPass.OnExecuteRenderPass = iOnExecuteRenderPass;

    RenderPasses.Add(newId, renderPass);
    return newId;
}

FGuid
FOdysseyTextureRenderer::InsertChildBefore(const FGuid& iParent, const FGuid& iReferenceChild, EOdysseyBlendingMode iBlendingMode, float iOpacity, const FMatrix& iTransform, const FOnExecuteRenderPass& iOnExecuteRenderPass)
{
    check(RenderPasses.Contains(iParent));
    check(RenderPasses.Contains(iReferenceChild));
    FGuid newId = FGuid::NewGuid();
    int32 index = RenderPasses[iParent].Children.IndexOfByKey(iReferenceChild);
    if (index != INDEX_NONE)
    {
        RenderPasses[iParent].Children.Insert(newId, index);
    }
    else
    {
        RenderPasses[iParent].Children.Insert(newId, 0);
    }
    FRenderPass renderPass;
    renderPass.BlendingMode = iBlendingMode;
    renderPass.Opacity = iOpacity;
    renderPass.Transform = iTransform;
    renderPass.OnExecuteRenderPass = iOnExecuteRenderPass;

    RenderPasses.Add(newId, renderPass);
    return newId;
}

FGuid
FOdysseyTextureRenderer::InsertChildAfter(const FGuid& iParent, const FGuid& iReferenceChild, EOdysseyBlendingMode iBlendingMode, float iOpacity, const FMatrix& iTransform, const FOnExecuteRenderPass& iOnExecuteRenderPass)
{
    check(RenderPasses.Contains(iParent));
    check(RenderPasses.Contains(iReferenceChild));
    FGuid newId = FGuid::NewGuid();
    int32 index = RenderPasses[iParent].Children.IndexOfByKey(iReferenceChild);
    if (index != INDEX_NONE)
    {
        RenderPasses[iParent].Children.Insert(newId, index + 1);
    }
    else
    {
        RenderPasses[iParent].Children.Add(newId);
    }
    FRenderPass renderPass;
    renderPass.BlendingMode = iBlendingMode;
    renderPass.Opacity = iOpacity;
    renderPass.Transform = iTransform;
    renderPass.OnExecuteRenderPass = iOnExecuteRenderPass;

    RenderPasses.Add(newId, renderPass);
    return newId;
}

void
FOdysseyTextureRenderer::RemoveChild(const FGuid& iParent, const FGuid& iChild)
{
    check(RenderPasses.Contains(iParent));
    check(RenderPasses.Contains(iChild));
    RenderPasses[iParent].Children.Remove(iChild);
    RenderPasses.Remove(iChild);
}

void
FOdysseyTextureRenderer::Clear()
{
    RootRenderPassId = FGuid::NewGuid();
    RenderPasses.Empty();
    RenderPasses.Add(RootRenderPassId, {});
}

void
FOdysseyTextureRenderer::Render(
    FRDGBuilder& GraphBuilder,
    FRDGTextureRef DestinationTexture,
    ERHIFeatureLevel::Type FeatureLevel,
    FIntRect SrcRect,
    FIntRect DstRect
) const
{
    check(RenderPasses.Contains(RootRenderPassId));
    const FRenderPass& rootPass = RenderPasses[RootRenderPassId];
    for (const FGuid& childId : rootPass.Children)
    {
        const FRenderPass& childPass = RenderPasses[childId];
        RenderPass_Recursive(
            childPass,
            GraphBuilder,
            DestinationTexture,
            FeatureLevel,
            SrcRect,
            DstRect
        );
    }
}

void
FOdysseyTextureRenderer::RenderPass_Recursive(
    const FRenderPass& iRenderPass,
    FRDGBuilder& GraphBuilder,
    FRDGTextureRef DestinationTexture,
    ERHIFeatureLevel::Type FeatureLevel,
    FIntRect SrcRect,
    FIntRect DstRect
) const
{
    FRDGTextureRef childrenTexture = nullptr;
    if (!iRenderPass.Children.IsEmpty())
    {
        //TODO: Create smaller texture using iSrcRect and/or iDstRect
        FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
            DestinationTexture->Desc.Extent,
            DestinationTexture->Desc.Format,
            FClearValueBinding::Transparent,
            ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
        );

        childrenTexture = GraphBuilder.CreateTexture(desc, TEXT("FOdysseyTextureRenderer::childrenTexture"));
        AddClearRenderTargetPass(GraphBuilder, childrenTexture, FLinearColor::Transparent, DstRect );

        for (const FGuid& childId : iRenderPass.Children)
        {
            const FRenderPass& childPass = RenderPasses[childId];
            RenderPass_Recursive(childPass, GraphBuilder, childrenTexture, FeatureLevel, SrcRect, DstRect);
        }
    }

    iRenderPass.Render(
        GraphBuilder,
        DestinationTexture,
        childrenTexture,
        FeatureLevel,
        SrcRect,
        DstRect
    );
}

void
FOdysseyTextureRenderer::FRenderPass::Render(
    FRDGBuilder& GraphBuilder,
    FRDGTextureRef DestinationTexture,
    FRDGTextureRef ChildrenTexture,
    ERHIFeatureLevel::Type FeatureLevel,
    FIntRect SrcRect,
    FIntRect DstRect
) const
{
    //Render the foregroundTexture
    FRDGTextureRef foregroundTexture = ChildrenTexture;
    if(OnExecuteRenderPass.IsBound())
    {
        FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
            DestinationTexture->Desc.Extent,
            DestinationTexture->Desc.Format,
            FClearValueBinding::Transparent,
            ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
        );

        foregroundTexture = GraphBuilder.CreateTexture(desc, TEXT("FOdysseyTextureRenderer::IntermediateTexture"));
        AddClearRenderTargetPass(GraphBuilder, foregroundTexture, FLinearColor::Transparent, DstRect );

        FRenderPassParameters renderPassParameters;
        renderPassParameters.DestinationTexture = foregroundTexture;
        renderPassParameters.ChildrenTexture = ChildrenTexture;
        renderPassParameters.FeatureLevel = FeatureLevel;
        renderPassParameters.SrcRect = SrcRect;
        renderPassParameters.DstRect = DstRect;

        OnExecuteRenderPass.ExecuteIfBound(GraphBuilder, renderPassParameters);
    }

    if (!foregroundTexture)
        return;

    //Create the background texture
    //TODO: Create smaller texture using iSrcRect and/or iDstRect
    FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
        DestinationTexture->Desc.Extent,
        DestinationTexture->Desc.Format,
        FClearValueBinding::Transparent,
        ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
    );
    FRDGTextureRef backgroundTexture = GraphBuilder.CreateTexture(desc, TEXT("FOdysseyTextureRenderer::IntermediateTexture"));
    AddClearRenderTargetPass(GraphBuilder, backgroundTexture, FLinearColor::Transparent, DstRect );

    //Copy Destination Texture to Background Texture
    AddCopyTexturePass(
        GraphBuilder,
        DestinationTexture,
        backgroundTexture,
        DstRect.Min,
        DstRect.Min,
        DstRect.Size()
    );

    FSamplerStateRHIRef samplerStateRHI = Odyssey::GetSamplerStateForAntiAliasing(AntiAliasing);

    //Alloc Shader Parameters
    FOdysseyBlendShaderParameters* shaderParameters = GraphBuilder.AllocParameters<FOdysseyBlendShaderParameters>();
    shaderParameters->RenderTargets[0] = FRenderTargetBinding(backgroundTexture, ERenderTargetLoadAction::ELoad);
    shaderParameters->SourceTexture = foregroundTexture;
    shaderParameters->SourceTextureSampler = samplerStateRHI;
    shaderParameters->DestinationTexture = backgroundTexture;
    shaderParameters->DestinationTextureSampler = samplerStateRHI;
    shaderParameters->Opacity = FMath::Clamp(Opacity, 0.f, 1.f);

    //Create Shader
    TRefCountPtr< FOdysseyBlendShader > blendShader(new FOdysseyBlendShader(shaderParameters, BlendingMode));

    //FCanvas* canvas = FCanvas::Create(GraphBuilder, backgroundTexture, nullptr, FGameTime(), FeatureLevel);

    FIntPoint foregroundTextureSize = foregroundTexture->Desc.Extent;
    FIntPoint backgroundTextureSize = backgroundTexture->Desc.Extent;

    GraphBuilder.AddPass(
        RDG_EVENT_NAME("OdysseyBlendShader"),
        shaderParameters,
        ERDGPassFlags::Raster,
        [FeatureLevel, DstRect, SrcRect, foregroundTextureSize, backgroundTextureSize, blendShader](FRHICommandListImmediate& RHICmdList)
        {
            FBatchedElements blendBatchedElements;

            double x = DstRect.Min.X;
            double y = DstRect.Min.Y;
            double w = DstRect.Width();
            double h = DstRect.Height();
            float u0 = float(SrcRect.Min.X) / foregroundTextureSize.X;
            float v0 = float(SrcRect.Min.Y) / foregroundTextureSize.Y;
            float u1 = float(SrcRect.Max.X) / foregroundTextureSize.X;
            float v1 = float(SrcRect.Max.Y) / foregroundTextureSize.Y;

            //FCanvasTileItem TileItem(FVector2D(x, y), GWhiteTexture, FVector2D(w, h), FVector2D(u, v), FVector2D(sizeU, sizeV), FLinearColor::White);

            int32 topLeftVertex = blendBatchedElements.AddVertex(FVector4(x, y, 0, 1), FVector2D(u0, v0), FLinearColor::White, FHitProxyId());
            int32 topRightVertex = blendBatchedElements.AddVertex(FVector4(x + w, y, 0, 1), FVector2D(u1, v0), FLinearColor::White, FHitProxyId());
            int32 bottomLeftVertex = blendBatchedElements.AddVertex(FVector4(x, y + h, 0, 1), FVector2D(u0, v1), FLinearColor::White, FHitProxyId());
            int32 bottomRightVertex = blendBatchedElements.AddVertex(FVector4(x + w, y + h, 0, 1), FVector2D(u1, v1), FLinearColor::White, FHitProxyId());

            blendBatchedElements.AddTriangle(topLeftVertex, topRightVertex, bottomRightVertex, blendShader.GetReference(), SE_BLEND_Opaque);
            blendBatchedElements.AddTriangle(topLeftVertex, bottomRightVertex, bottomLeftVertex, blendShader.GetReference(), SE_BLEND_Opaque);

            FMeshPassProcessorRenderState DrawRenderState;
            DrawRenderState.SetDepthStencilState(TStaticDepthStencilState<false, CF_Always>::GetRHI());

            // Guard against division by zero.
            uint32 ViewSizeX = FMath::Max<uint32>(backgroundTextureSize.X, 1.f);
            uint32 ViewSizeY = FMath::Max<uint32>(backgroundTextureSize.Y, 1.f);

            FMatrix transform =  AdjustProjectionMatrixForRHI(
                FTranslationMatrix(FVector(0, 0, 0)) *
                FMatrix(
                    FPlane(1.0f / (ViewSizeX / 2.0f), 0.0, 0.0f, 0.0f ),
                    FPlane(0.0f, -1.0f / (ViewSizeY / 2.0f), 0.0f, 0.0f ),
                    FPlane(0.0f, 0.0f, 1.0f, 0.0f ),
                    FPlane(-1.0f, 1.0f, 0.0f, 1.0f )
                    )
                );

            FSceneView proxySceneView = FBatchedElements::CreateProxySceneView(transform, FIntRect(0, 0, backgroundTextureSize.X, backgroundTextureSize.Y));

            blendBatchedElements.Draw(
                RHICmdList,
                DrawRenderState,
                FeatureLevel,
                proxySceneView,
                false
            );
        }
    );

    AddCopyTexturePass(
        GraphBuilder,
        backgroundTexture,
        DestinationTexture,
        DstRect.Min,
        DstRect.Min,
        DstRect.Size()
    );
}

#undef LOCTEXT_NAMESPACE
