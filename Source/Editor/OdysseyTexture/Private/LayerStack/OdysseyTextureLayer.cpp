// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyTextureLayer.h"

#include "LayerStack/OdysseyTextureLayerStack.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureLayer"

class FOdysseyTextureLayerPreloadHandle : public IOdysseyHandle
{
public:
    ~FOdysseyTextureLayerPreloadHandle()
    {
        mTextureLayer->OnChildrenChanged().RemoveAll(this);
    }

    FOdysseyTextureLayerPreloadHandle(UOdysseyTextureLayer* iTextureLayer)
        : mTextureLayer(iTextureLayer)
    {
        UOdysseyLayer::OnChildrenChanged().AddRaw(this, &FOdysseyTextureLayerPreloadHandle::OnChildrenChanged);
        OnChildrenChanged(iTextureLayer);
    }

    void OnChildrenChanged(UOdysseyLayer* iLayer)
    {
        if ( iLayer != mTextureLayer )
            return;

        TArray<TSharedPtr<IOdysseyHandle>> handles;
        TArray<UOdysseyLayer*> layers = mTextureLayer->GetChildren();
        for (UOdysseyLayer* layer : layers)
        {
            UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(layer);
            if (!textureLayer)
                continue;

            handles.Add(textureLayer->Preload());
        }
        mChildrenHandles = handles; //Copy at the end to avoid unwanted handle destruction
    }

private:
    UOdysseyTextureLayer* mTextureLayer;
    TArray<TSharedPtr<IOdysseyHandle>> mChildrenHandles;
};

UOdysseyTextureLayer::UOdysseyTextureLayer()
    : mPropertyTracker(this)
{
}

UOdysseyTextureLayer::FOnRenderImageChanged&
UOdysseyTextureLayer::OnRenderImageChanged()
{
    static FOnRenderImageChanged onRenderImageChanged;
    return onRenderImageChanged;
}

void
UOdysseyTextureLayer::PostInitProperties()
{
    Super::PostInitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    mPropertyTracker.Track(FName("Children"), IOdysseySinglePropertyTracker::TOnChanged<TArray<UOdysseyLayer*>>::CreateUObject(this, &UOdysseyTextureLayer::OnTrackerChildrenChanged));
}

void
UOdysseyTextureLayer::PostLoad()
{
    Super::PostLoad();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    //We just reset the tracker here, to avoid wrong values in OnTrackerParentChanged
    mPropertyTracker.Untrack("Children");
    mPropertyTracker.Track(FName("Children"), IOdysseySinglePropertyTracker::TOnChanged<TArray<UOdysseyLayer*>>::CreateUObject(this, &UOdysseyTextureLayer::OnTrackerChildrenChanged));
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

TSharedPtr<IOdysseyHandle>
UOdysseyTextureLayer::Preload()
{
    TSharedPtr<IOdysseyHandle> handle = mPreloadHandle.Pin();
    if (handle)
        return handle;

    handle = MakeShared<FOdysseyTextureLayerPreloadHandle>(this);
    mPreloadHandle = handle;
    return handle;
}

void
UOdysseyTextureLayer::OnTrackerChildrenChanged(const TArray<UOdysseyLayer*>& iOldChildren)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if ( !layerStack )
        return;

    UTexture2D* texture = layerStack->GetTexture();
    if ( !texture )
        return;

    RenderImageChanged({ ::ULIS::FRectI::FromXYWH(0, 0, texture->Source.GetSizeX(), texture->Source.GetSizeY() ) }, false);
}

#undef LOCTEXT_NAMESPACE
