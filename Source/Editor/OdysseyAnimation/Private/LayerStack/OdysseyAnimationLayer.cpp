// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayer.h"

#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationLayer"


class FOdysseyAnimationLayerPreloadHandle : public IOdysseyHandle
{
public:
    ~FOdysseyAnimationLayerPreloadHandle()
    {
        mAnimationLayer->OnChildrenChanged().RemoveAll(this);
    }

    FOdysseyAnimationLayerPreloadHandle(UOdysseyAnimationLayer* iAnimationLayer, int iFrame)
        : mAnimationLayer(iAnimationLayer)
        , mFrame( iFrame )
    {
        UOdysseyLayer::OnChildrenChanged().AddRaw(this, &FOdysseyAnimationLayerPreloadHandle::OnChildrenChanged);
    }

    void OnChildrenChanged(UOdysseyLayer* iLayer)
    {
        if ( iLayer != mAnimationLayer )
            return;

        TArray<TSharedPtr<IOdysseyHandle>> handles;
        TArray<UOdysseyLayer*> layers = mAnimationLayer->GetChildren();
        for (UOdysseyLayer* layer : layers)
        {
            UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
            if (!animationLayer)
                continue;

            handles.Add(animationLayer->Preload(mFrame));
        }
        mChildrenHandles = handles; //Copy at the end to avoid unwanted handle destruction
    }

private:
    UOdysseyAnimationLayer* mAnimationLayer;
    TArray<TSharedPtr<IOdysseyHandle>> mChildrenHandles;
    int mFrame;
};

UOdysseyAnimationLayer::UOdysseyAnimationLayer()
    : mPropertyTracker(this)
{
}

void
UOdysseyAnimationLayer::PostInitProperties()
{
    Super::PostInitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    mPropertyTracker.Track(FName("Children"), IOdysseySinglePropertyTracker::TOnChanged<TArray<UOdysseyLayer*>>::CreateUObject(this, &UOdysseyAnimationLayer::OnTrackerChildrenChanged));
}

void
UOdysseyAnimationLayer::PostLoad()
{
    Super::PostLoad();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    //We just reset the tracker here, to avoid wrong values in OnTrackerParentChanged
    mPropertyTracker.Untrack("Children");
    mPropertyTracker.Track(FName("Children"), IOdysseySinglePropertyTracker::TOnChanged<TArray<UOdysseyLayer*>>::CreateUObject(this, &UOdysseyAnimationLayer::OnTrackerChildrenChanged));
}

UOdysseyAnimation*
UOdysseyAnimationLayer::GetAnimation()
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetLayerStack());
    if(!layerStack)
        return nullptr;

    return layerStack->GetAnimation();
}

TRange<int>
UOdysseyAnimationLayer::GetFrameRange() const
{
    return TRange<int>();
}

UOdysseyAnimationLayer::FOnRenderImageChanged&
UOdysseyAnimationLayer::OnRenderImageChanged()
{
    static FOnRenderImageChanged onRenderImageChanged;
    return onRenderImageChanged;
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayer::RenderLayersImage(TArray<UOdysseyLayer*> iLayers, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!ioBlock)
        return iWaitList;

    if (iLayers.Num() <= 0)
        return iWaitList;

    //Clear the block before blending on it
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( ioBlock->Format() );
    ::ULIS::FEvent eventClearBlock;
    ctx.Clear( *ioBlock, ::ULIS::FRectI::FromXYWH(iPos.x, iPos.y, iRect.w, iRect.h), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, iWaitList.Num(), iWaitList.GetData(), &eventClearBlock );

    TArray<::ULIS::FEvent> lastEvent = { eventClearBlock };
    for (int i = iLayers.Num() - 1; i >= 0 ; i--)
    {
        UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(iLayers[i]);
        if (!layer)
            continue;

        lastEvent = layer->RenderImage(ioBlock, iFrame, iRect, iPos, lastEvent);
    }
    
    ctx.Flush();

    return lastEvent;
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayer::RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return iWaitList;
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayer::CopyImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return iWaitList;
}

void
UOdysseyAnimationLayer::RenderImageChanged(bool iIsInteractive)
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return;

    RenderImageChanged(GetFrameRange(), { ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height()) }, iIsInteractive);
}

void
UOdysseyAnimationLayer::RenderImageChanged(const TRange<int>& iFrameRange, bool iIsInteractive)
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return;

    RenderImageChanged(iFrameRange, { ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height())}, iIsInteractive);
}

void
UOdysseyAnimationLayer::RenderImageChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return;

    RenderImageChanged(GetFrameRange(), iRects, iIsInteractive);
}

void
UOdysseyAnimationLayer::RenderImageChanged(const TRange<int>& iFrameRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    if ( !Parent )
        return;

    //PATCH BEGIN: because Unreal Undo does not make package dirty correctly after a save
    MarkPackageDirty();
    //PATCH END:

    UOdysseyAnimationLayer* parent = Cast<UOdysseyAnimationLayer>(Parent);
    if (parent)
        parent->ChildRenderImageChanged(this, iFrameRange, iRects, iIsInteractive);
    
    OnRenderImageChanged().Broadcast(this, iFrameRange, iRects, true); //always send at least 1 interactive event
    if (!iIsInteractive )
        OnRenderImageChanged().Broadcast(this, iFrameRange, iRects, false);
}

void
UOdysseyAnimationLayer::ChildRenderImageChanged(UOdysseyAnimationLayer* iLayer, const TRange<int>& iFrameRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    RenderImageChanged(iFrameRange, iRects, iIsInteractive);
}

void
UOdysseyAnimationLayer::IsActivatedChanged()
{
    Super::IsActivatedChanged();
    RenderImageChanged(false);
}

void
UOdysseyAnimationLayer::OnTrackerChildrenChanged(const TArray<UOdysseyLayer*>& iOldChildren)
{
    //TODO: Find a way to invalid only frame ranges that actually changed, instead of the while layer
    RenderImageChanged(false);
}

TSharedPtr<IOdysseyHandle>
UOdysseyAnimationLayer::Preload(int iFrame)
{
    TSharedPtr<IOdysseyHandle> handle = nullptr;
    TWeakPtr<IOdysseyHandle>* weakHandle = mPreloadHandles.Find(iFrame);
    if (!weakHandle)
    {
        handle = MakeShared<FOdysseyAnimationLayerPreloadHandle>(this, iFrame);
        mPreloadHandles.Add(iFrame, handle);
        return handle;
    }

    handle = weakHandle->Pin();
    if (handle)
        return handle;

    handle = MakeShared<FOdysseyAnimationLayerPreloadHandle>(this, iFrame);
    mPreloadHandles[iFrame] = handle;
    return handle;
}

#undef LOCTEXT_NAMESPACE
