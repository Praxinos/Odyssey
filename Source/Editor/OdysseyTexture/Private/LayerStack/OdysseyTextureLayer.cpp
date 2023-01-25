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
UOdysseyTextureLayer::RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return iWaitList;
}

TArray<::ULIS::FEvent>
UOdysseyTextureLayer::CopyImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return iWaitList;
}

void
UOdysseyTextureLayer::RenderImageChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    if ( !Parent )
        return;

    //If parent is not a textureLayer, it is probably the RootLayer, so inform the layerStack directly
    UOdysseyTextureLayer* parent = Cast<UOdysseyTextureLayer>(Parent);
    if (parent)
        parent->ChildRenderImageChanged(this, iRects, iIsInteractive);
    
    OnRenderImageChanged().Broadcast(this, iRects, true); //always send at least 1 interactive event
    if (!iIsInteractive )
        OnRenderImageChanged().Broadcast(this, iRects, false);
}

void
UOdysseyTextureLayer::ChildRenderImageChanged(UOdysseyTextureLayer* iLayer, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    RenderImageChanged(iRects, iIsInteractive);
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

    RenderImageChanged({ ::ULIS::FRectI::FromXYWH(0, 0, texture->Source.GetSizeX(), texture->Source.GetSizeY() ) }, false);
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

    RenderImageChanged({ ::ULIS::FRectI::FromXYWH(0, 0, texture->Source.GetSizeX(), texture->Source.GetSizeY() ) }, false);
}

void
UOdysseyTextureLayer::Preload(TArray<TSharedPtr<IOdysseyHandle>>& oHandles)
{
    const TArray<UOdysseyLayer*>& layers = Children;
    for (UOdysseyLayer* layer : layers)
    {
        UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(layer);
        if (!textureLayer)
            continue;

        textureLayer->Preload(oHandles);
    }
}

#undef LOCTEXT_NAMESPACE
