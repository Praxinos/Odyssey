// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayerRoot.h"
#include "OdysseyAnimationLayerFolder.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyRectUtils.h"

UOdysseyAnimationLayerStack::FOnRenderImageChanged&
UOdysseyAnimationLayerStack::OnRenderImageChanged()
{
    static FOnRenderImageChanged onRenderImageChanged;
    return onRenderImageChanged;
}

UOdysseyAnimationLayerStack::UOdysseyAnimationLayerStack()
{
    CompatibleLayers.Add(UOdysseyAnimationLayerFolder::StaticClass());
    CompatibleLayers.Add(UOdysseyAnimationLayerImageRaster::StaticClass());

    LayerRootClass = UOdysseyAnimationLayerRoot::StaticClass();
}

UOdysseyAnimation*
UOdysseyAnimationLayerStack::GetAnimation() const
{
    UObject* outer = GetOuter();
    while(outer)
    {
        if (outer->GetClass() == UOdysseyAnimation::StaticClass())
            return Cast<UOdysseyAnimation>(outer);

		outer = outer->GetOuter();
    }

    return nullptr;    
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayerStack::RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    //TODO: Move to LayerRoot

    if (!ioBlock)
        return iWaitList;

    TArray<UOdysseyLayer*> children = GetRootLayers();
    if (children.Num() <= 0)
        return iWaitList;

    //ensure we use a sourceBlock with an alpha channel
    ::ULIS::eFormat format = static_cast< ::ULIS::eFormat >( ioBlock->Format() | ULIS_W_ALPHA( 1 ) );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    //Convert the destination if needed and Blend the folderBlock
    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, format, iRect, iPos, iWaitList,
        [this, &children, &iFrame](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            return RenderLayersImage(children, ioDest, iFrame, iRect, iPos, iWaitList);
        }
    );
    ctx.Flush();

    return eventConvertAndExecute;
}

void
UOdysseyAnimationLayerStack::OnRootLayerRenderImageChanged(UOdysseyAnimationLayer* iLayer, const TRange<int>& iFrameRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    OnRenderImageChanged().Broadcast(this, iFrameRange, iRects, iIsInteractive);
}

void
UOdysseyAnimationLayerStack::Preload(int iFrame, TArray<TSharedPtr<IOdysseyHandle>>& oHandles)
{
    const TArray<UOdysseyLayer*>& layers = GetLayers();
    for (UOdysseyLayer* layer : layers)
    {
        UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
        if (!animationLayer)
            continue;

        animationLayer->Preload(iFrame, oHandles);
    }
}
