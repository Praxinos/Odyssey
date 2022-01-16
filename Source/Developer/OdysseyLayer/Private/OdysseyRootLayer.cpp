// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyRootLayer.h"
#include "ULISLoaderModule.h"

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

TArray<::ULIS::FEvent>
FOdysseyRootLayer::RenderImage( ::ULIS::FBlock** ioBlocks, const ::ULIS::FRectI* iRects, const ::ULIS::FVec2I* iPositions, const uint32 iNum)
{
    TArray< TSharedPtr< IOdysseyLayer > > children = GetNodes();
    TArray<::ULIS::FEvent> eventRender;
    if( !IsVisible() || iNum == 0 || children.Num() == 0)
    {
        for (uint32 i = 0; i < iNum; i++)
            eventRender.Add(::ULIS::FEvent::NoOP());
        return eventRender;
    }

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( ioBlocks[0]->Format() );
    TArray<::ULIS::FEvent> clearEvents;
    clearEvents.SetNum(iNum);

    for( uint32 i = 0; i < iNum; ++i ) {
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromPositionAndSize( iPositions[i], iRects[i].Size() );
        ctx.Clear( *ioBlocks[i], rect, ::ULIS::FSchedulePolicy::MonoScanlines, 0, nullptr, &clearEvents[i] );
        ctx.Flush();
    }
    //ctx.Finish();

    eventRender = clearEvents;
    for( int i = children.Num() - 1; i >= 0; --i ) {
        TSharedPtr< IOdysseyLayer > child = children[i];
        if( !child->ImplementsCapability(IOdysseyLayerImageBlendingCapability::GetGuid() ) )
            continue;

        IOdysseyLayerImageBlendingCapability* layerBlendable = child->GetCapability< IOdysseyLayerImageBlendingCapability >();
        if( !layerBlendable )
            continue;

        eventRender = layerBlendable->Blend( ioBlocks, iRects, iPositions, iNum, eventRender.GetData() );
    }

    return eventRender;
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
        layerBlendable->ImageResultChangedDelegate().AddRaw( this, &FOdysseyRootLayer::OnChildImageResultChanged, iLayer );
        if( iLayer->IsVisible() )
        {
            mImageResultChangedDelegate.Broadcast( nullptr, 0 );
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
        mImageResultChangedDelegate.Broadcast( nullptr, 0 );
    }
}

void
FOdysseyRootLayer::OnChildImageResultChanged( const ::ULIS::FRectI* iRects, const uint32 iNumRects, TSharedPtr< IOdysseyLayer > iLayer )
{
    bool isBlendable = iLayer->ImplementsCapability( IOdysseyLayerImageBlendingCapability::GetGuid() );
    if( isBlendable )
    {
        mImageResultChangedDelegate.Broadcast( iRects, iNumRects );
    }
}

void
FOdysseyRootLayer::Serialize(FArchive &Ar)
{
    //does nothing
}

#undef LOCTEXT_NAMESPACE
