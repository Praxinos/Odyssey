// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/OdysseyAnimationLayerRoot.h"
#include "LayerStack/OdysseyAnimationLayerFolder.h"
#include "LayerStack/OdysseyAnimationLayerImageRaster.h"
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

TRange<int>
UOdysseyAnimationLayerStack::GetFrameRange() const
{
    return Cast<UOdysseyAnimationLayerRoot>(LayerRoot)->GetFrameRange();
}

FString
UOdysseyAnimationLayerStack::GetFrameId(int iFrameIndex) const
{
    return Cast<UOdysseyAnimationLayerRoot>(LayerRoot)->GetFrameId(iFrameIndex);
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
            return UOdysseyAnimationLayer::RenderLayersImage(children, ioDest, iFrame, iRect, iPos, iWaitList);
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

TSharedPtr<IOdysseyHandle>
UOdysseyAnimationLayerStack::Preload(int iFrame)
{
    return Cast<UOdysseyAnimationLayerRoot>(LayerRoot)->Preload(iFrame);
}
