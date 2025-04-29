// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyBlendingMode.h"
#include "OdysseyAntiAliasing.h"

class ODYSSEYRENDERING_API FOdysseyTextureRenderer
{
public:
    FOdysseyTextureRenderer();

public:
    struct FRenderPassParameters
    {
        FRDGTextureRef DestinationTexture;
        FRDGTextureRef ChildrenTexture;
        ERHIFeatureLevel::Type FeatureLevel;
        FMatrix SrcTransform;
        FIntRect SrcRect;
        FIntRect DstRect;
    };

    DECLARE_DELEGATE_TwoParams(FOnExecuteRenderPass, FRDGBuilder&, const FRenderPassParameters&);
    class FRenderPass
    {
        friend class FOdysseyTextureRenderer;

        public:
            const TArray<FGuid>& GetChildren() const { return Children; }
            FOnExecuteRenderPass GetOnExecuteRenderPass() const { return OnExecuteRenderPass; }

            void Render(
                FRDGBuilder& GraphBuilder,
                FRDGTextureRef DestinationTexture,
                FRDGTextureRef ChildrenTexture,
                ERHIFeatureLevel::Type FeatureLevel,
                FIntRect SrcRect,
                FIntRect DstRect
            ) const;

        public:
            EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal;
            float Opacity = 1.0f;
            FMatrix Transform;
            EOdysseyAntiAliasing AntiAliasing = EOdysseyAntiAliasing::AnisotropicLinear;

        private:
            TArray<FGuid> Children;
            FOnExecuteRenderPass OnExecuteRenderPass;
    };

    const FGuid& GetRootPassId() const;
    const FRenderPass& GetPass(const FGuid& iId) const;
    FRenderPass& GetPass(const FGuid& iId);

    void Append(const FGuid& iParent, const FOdysseyTextureRenderer& iRenderer);
    FGuid AddChild(const FGuid& iParent, EOdysseyBlendingMode iBlendingMode, float iOpacity, const FMatrix& iTransform, const FOnExecuteRenderPass& iOnExecuteRenderPass = FOnExecuteRenderPass());
    FGuid InsertChildBefore(const FGuid& iParent, const FGuid& iReferenceChild, EOdysseyBlendingMode iBlendingMode, float iOpacity, const FMatrix& iTransform, const FOnExecuteRenderPass& iOnExecuteRenderPass = FOnExecuteRenderPass());
    FGuid InsertChildAfter(const FGuid& iParent, const FGuid& iReferenceChild, EOdysseyBlendingMode iBlendingMode, float iOpacity, const FMatrix& iTransform, const FOnExecuteRenderPass& iOnExecuteRenderPass = FOnExecuteRenderPass());
    void RemoveChild(const FGuid& iParent, const FGuid& iChild);

    void Clear();

    void Render(
        FRDGBuilder& GraphBuilder,
        FRDGTextureRef DestinationTexture,
        ERHIFeatureLevel::Type FeatureLevel,
        FIntRect SrcRect,
        FIntRect DstRect
    ) const;

private:
    void RenderPass_Recursive(
        const FRenderPass& iRenderPass,
        FRDGBuilder& GraphBuilder,
        FRDGTextureRef DestinationTexture,
        ERHIFeatureLevel::Type FeatureLevel,
        FIntRect SrcRect,
        FIntRect DstRect
    ) const;

private:
    FGuid RootRenderPassId;
    TMap<FGuid, FRenderPass> RenderPasses;
};
