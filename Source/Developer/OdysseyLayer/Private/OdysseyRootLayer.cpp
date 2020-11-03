// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyRootLayer.h"

#include "IOdysseyLayerImageBlendingCapability.h"

#define LOCTEXT_NAMESPACE "OdysseyRootLayer"

//---

FOdysseyRootLayer::~FOdysseyRootLayer()
{
}

FOdysseyRootLayer::FOdysseyRootLayer( const FOdysseyRootLayer& iLayer)
    : IOdysseyLayer(iLayer)
    , IOdysseyLayerImageRenderingCapability(iLayer)
{
}

FOdysseyRootLayer::FOdysseyRootLayer()
    : IOdysseyLayer( "", IOdysseyLayer::eType::kRoot )
    , IOdysseyLayerImageRenderingCapability()
{
}

FOdysseyRootLayer*
FOdysseyRootLayer::Clone() const
{
    return new FOdysseyRootLayer(*this);
}

//---

void
FOdysseyRootLayer::RenderImage(::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect, ::ul3::FVec2F iPos)
{
    if (!IsVisible())
        return;

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent =  ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::FRect rect = ::ul3::FRect::FromXYWH(iPos.x, iPos.y, iRect.w, iRect.h);
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, ioBlock, rect );

    TArray<TSharedPtr<IOdysseyLayer>> children = GetNodes();
    for (int i = children.Num() - 1; i >= 0; i--)
    {
        TSharedPtr<IOdysseyLayer> child = children[i];
        if (!child->ImplementsCapability(IOdysseyLayerImageBlendingCapability::GetGuid()))
            continue;

        IOdysseyLayerImageBlendingCapability* layerBlendable = child->GetCapability<IOdysseyLayerImageBlendingCapability>();
        if (!layerBlendable)
            continue;

        layerBlendable->Blend(ioBlock, iRect, iPos);
    }
}

bool
FOdysseyRootLayer::ImplementsCapability(FGuid iGuid) const
{
    return IOdysseyLayerImageRenderingCapability::GetGuids().Contains(iGuid);
}

void*
FOdysseyRootLayer::GetCapabilityPtrFromGuid(FGuid iGuid)
{
    if (IOdysseyLayerImageRenderingCapability::GetGuids().Contains(iGuid))
        return IOdysseyLayerImageRenderingCapability::GetCapabilityPtrFromGuid(this, iGuid);
        
    return nullptr;
}

void
FOdysseyRootLayer::AddNode(TSharedPtr<IOdysseyLayer> iLayer, int iIndex)
{
    IOdysseyLayer::AddNode(iLayer, iIndex);
    bool isBlendable = iLayer->ImplementsCapability(IOdysseyLayerImageBlendingCapability::GetGuid());
    if (isBlendable)
    {
        IOdysseyLayerImageBlendingCapability* layerBlendable = iLayer->GetCapability<IOdysseyLayerImageBlendingCapability>();
		layerBlendable->ImageResultChangedDelegate().AddRaw(this, &FOdysseyRootLayer::OnChildImageResultChanged, iLayer);
        if (iLayer->IsVisible())
        {
            mImageResultChangedDelegate.Broadcast();
        }
    }
}

void
FOdysseyRootLayer::DeleteNode(int iIndex)
{
    TSharedPtr<IOdysseyLayer> layer = GetNode(iIndex);
    bool isBlendable = layer->ImplementsCapability(IOdysseyLayerImageBlendingCapability::GetGuid());
    if (isBlendable)
    {
        IOdysseyLayerImageBlendingCapability* layerBlendable = layer->GetCapability<IOdysseyLayerImageBlendingCapability>();
		layerBlendable->ImageResultChangedDelegate().RemoveAll(this);
    }

    IOdysseyLayer::DeleteNode(iIndex);

    if (isBlendable && layer->IsVisible())
    {
        mImageResultChangedDelegate.Broadcast();
    }
}

void
FOdysseyRootLayer::OnChildImageResultChanged(TSharedPtr<IOdysseyLayer> iLayer)
{
    bool isBlendable = iLayer->ImplementsCapability(IOdysseyLayerImageBlendingCapability::GetGuid());
    if (isBlendable)
    {
        mImageResultChangedDelegate.Broadcast();
    }
}

void
FOdysseyRootLayer::Serialize(FArchive &Ar)
{
    //does nothing
}

#undef LOCTEXT_NAMESPACE
