// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

class ODYSSEYRENDERING_API FOdysseyTextureRenderer
{
public:
    FOdysseyTextureRenderer();

public:
    struct FRenderPassParameters
    {
        FRDGBuilder& GraphBuilder;
        FRDGTextureRef DestinationTexture;
        ERHIFeatureLevel::Type FeatureLevel;
        FFrameNumber Frame;
        FMatrix SrcTransform;
        FIntRect SrcRect;
        FIntRect DstRect;
    };

    DECLARE_DELEGATE_OneParam(FOnExecuteRenderPass, const FRenderPassParameters&);
    struct FRenderPass
    {
        FOnExecuteRenderPass OnExecuteRenderPass;
        TArray<FGuid> mChildren;
    };

    const FGuid& GetRootPassId();
    const FRenderPass& GetPass(const FGuid& iId) const;

    FGuid AddChild(const FGuid& iParent, const FOnExecuteRenderPass& iOnExecuteRenderPass);
    FGuid InsertChildBefore(const FGuid& iParent, const FGuid& iReferenceChild, const FOnExecuteRenderPass& iOnExecuteRenderPass);
    FGuid InsertChildAfter(const FGuid& iParent, const FGuid& iReferenceChild, const FOnExecuteRenderPass& iOnExecuteRenderPass);
    void RemoveChild(const FGuid& iParent, const FGuid& iChild);

    void Clear();

    void Render(const FRenderPassParameters& iParameters) const;

private:

    FGuid RootRenderPassId;
    TMap<FGuid, FRenderPass> RenderPasses;
};
