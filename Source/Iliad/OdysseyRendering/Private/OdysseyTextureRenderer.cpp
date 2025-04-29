// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyTextureRenderer.h"

#define LOCTEXT_NAMESPACE "Imaging"

FOdysseyTextureRenderer::FOdysseyTextureRenderer()
{
    RootRenderPassId = FGuid::NewGuid();
    RenderPasses.Add(RootRenderPassId, {});
}

const FGuid&
FOdysseyTextureRenderer::GetRootPassId()
{
    return RootRenderPassId;
}

const FOdysseyTextureRenderer::FRenderPass&
FOdysseyTextureRenderer::GetPass(const FGuid& iId) const
{
    check(RenderPasses.Contains(iId));
    return RenderPasses[iId];
}

FGuid
FOdysseyTextureRenderer::AddChild(const FGuid& iParent, const FOdysseyTextureRenderer::FOnExecuteRenderPass& iOnExecuteRenderPass)
{
    check(RenderPasses.Contains(iParent));
    FGuid newId = FGuid::NewGuid();
    RenderPasses[iParent].mChildren.Add(newId);
    RenderPasses.Add(newId, { iOnExecuteRenderPass });
    return newId;
}

FGuid
FOdysseyTextureRenderer::InsertChildBefore(const FGuid& iParent, const FGuid& iReferenceChild, const FOdysseyTextureRenderer::FOnExecuteRenderPass& iOnExecuteRenderPass)
{
    check(RenderPasses.Contains(iParent));
    check(RenderPasses.Contains(iReferenceChild));
    FGuid newId = FGuid::NewGuid();
    int32 index = RenderPasses[iParent].mChildren.IndexOfByKey(iReferenceChild);
    if (index != INDEX_NONE)
    {
        RenderPasses[iParent].mChildren.Insert(newId, index);
    }
    else
    {
        RenderPasses[iParent].mChildren.Insert(newId, 0);
    }
    RenderPasses.Add(newId, { iOnExecuteRenderPass });
    return newId;
}

FGuid
FOdysseyTextureRenderer::InsertChildAfter(const FGuid& iParent, const FGuid& iReferenceChild, const FOdysseyTextureRenderer::FOnExecuteRenderPass& iOnExecuteRenderPass)
{
    check(RenderPasses.Contains(iParent));
    check(RenderPasses.Contains(iReferenceChild));
    FGuid newId = FGuid::NewGuid();
    int32 index = RenderPasses[iParent].mChildren.IndexOfByKey(iReferenceChild);
    if (index != INDEX_NONE)
    {
        RenderPasses[iParent].mChildren.Insert(newId, index + 1);
    }
    else
    {
        RenderPasses[iParent].mChildren.Add(newId);
    }
    RenderPasses.Add(newId, { iOnExecuteRenderPass });
    return newId;
}

void
FOdysseyTextureRenderer::RemoveChild(const FGuid& iParent, const FGuid& iChild)
{
    check(RenderPasses.Contains(iParent));
    check(RenderPasses.Contains(iChild));
    RenderPasses[iParent].mChildren.Remove(iChild);
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
FOdysseyTextureRenderer::Render(const FRenderPassParameters& iParameters) const
{
    check(RenderPasses.Contains(RootRenderPassId));
    const FRenderPass& rootPass = RenderPasses[RootRenderPassId];
    for (const FGuid& childId : rootPass.mChildren)
    {
        FRenderPass childPass = RenderPasses[childId];
        if (childPass.OnExecuteRenderPass.IsBound())
        {
            childPass.OnExecuteRenderPass.ExecuteIfBound(iParameters);
        }
    }
}

#undef LOCTEXT_NAMESPACE
