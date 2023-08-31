// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyTextureLayer.h"

#include "LayerStack/OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerImageRenderer.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureLayer"

void
UOdysseyTextureLayer::IsActivatedChanged()
{
    Super::IsActivatedChanged();

    UOdysseyTextureLayer* parentLayer = Cast<UOdysseyTextureLayer>(GetParent());
    if (!parentLayer)
        return;

    parentLayer->ImageRenderingCompositionChanged();
}

void
UOdysseyTextureLayer::ChildrenChanged()
{
    Super::ChildrenChanged();

    ImageRenderingCompositionChanged();
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyTextureLayer::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType) const
{
    return MakeShared<FOdysseyTextureLayerImageRenderer>(this, iRenderType, GetImageRenderingRects());
}

TArray<FGuid>
UOdysseyTextureLayer::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };

    const TArray<UOdysseyLayer*>& children = GetChildren();
    for (UOdysseyLayer* child : children)
    {
        if (!child->IsActivated)
            continue;

        UOdysseyTextureLayer* textureChild = Cast<UOdysseyTextureLayer>(child);
        if (!textureChild)
            continue;

        idComposition.Append(textureChild->GetImageRenderingComposition(iRenderType));
    }

    return idComposition;
}

TArray<::ULIS::FRectI>
UOdysseyTextureLayer::GetImageRenderingRects() const
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if(!layerStack)
        return {};

    return layerStack->GetImageRenderingRects();
}

#undef LOCTEXT_NAMESPACE
