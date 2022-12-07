// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyTextureLayer.h"

#include "LayerStack/OdysseyTextureLayerStack.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureLayer"


UOdysseyTextureLayer::FOnRenderImageChanged&
UOdysseyTextureLayer::OnRenderImageChanged()
{
    static FOnRenderImageChanged onRenderImageChanged;
    return onRenderImageChanged;
}

TArray<::ULIS::FEvent>
UOdysseyTextureLayer::RenderImage(::ULIS::FBlock* ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return iWaitList;
}

TArray<::ULIS::FEvent>
UOdysseyTextureLayer::CopyImage(::ULIS::FBlock* ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return iWaitList;
}

void
UOdysseyTextureLayer::RenderImageChanged(const TArray<::ULIS::FRectI>& iRects)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    if ( !Parent )
        return;

    //If parent is not a textureLayer, it is probably the RootLayer, so inform the layerStack directly
    UOdysseyTextureLayer* parent = Cast<UOdysseyTextureLayer>(Parent);
    if (parent)
        parent->ChildRenderImageChanged(this, iRects);
        
    OnRenderImageChanged().Broadcast(this, iRects);
}

void
UOdysseyTextureLayer::ChildRenderImageChanged(UOdysseyTextureLayer* iLayer, const TArray<::ULIS::FRectI>& iRects)
{
    RenderImageChanged(iRects);
}

void
UOdysseyTextureLayer::IsActivatedChanged()
{
    Super::IsActivatedChanged();

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    UTexture2D* texture = layerStack->GetTexture();
    if (!texture)
        return;

    RenderImageChanged({ ::ULIS::FRectI::FromXYWH(0, 0, texture->Source.GetSizeX(), texture->Source.GetSizeY() ) });
}

void
UOdysseyTextureLayer::ChildrenChanged()
{
    Super::ChildrenChanged();

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    UTexture2D* texture = layerStack->GetTexture();
    if (!texture)
        return;

    RenderImageChanged({ ::ULIS::FRectI::FromXYWH(0, 0, texture->Source.GetSizeX(), texture->Source.GetSizeY() ) });
}

#undef LOCTEXT_NAMESPACE
