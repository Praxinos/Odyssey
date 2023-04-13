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
        UpdateChildrenHandles();
        UOdysseyLayer::OnRenderImageIdChanged().AddRaw(this, &FOdysseyAnimationLayerPreloadHandle::OnRenderImageIdChanged);
    }

    void OnRenderImageIdChanged(UOdysseyLayer* iLayer)
    {
        if ( iLayer != mAnimationLayer )
            return;

        FOdysseyAnimationRenderImageId frameId = iLayer->GetFrameId(iFrame);
        if (mFrameId == frameId)
            return;

        UpdateChildrenHandles();
    }

    void UpdateChildrenHandles()
    {
        TArray<TSharedPtr<IOdysseyHandle>> handles;
        TArray<UOdysseyLayer*> layers = mAnimationLayer->GetChildren();
        for ( UOdysseyLayer* layer : layers )
        {
            UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
            if ( !animationLayer )
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

//===========================

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

FInt32Range
UOdysseyAnimationLayer::GetFrameRange() const
{
    TArray<FInt32Range> ranges;
    const TArray<UOdysseyLayer*> layers = Children;
    for (UOdysseyLayer* layer : layers)
    {
        UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
        if (!animationLayer)
            continue;

        ranges.Add(animationLayer->GetFrameRange());
    }

    return FInt32Range::Hull(ranges);
}

FString
UOdysseyAnimationLayer::GetFrameId(int iFrameIndex) const
{
    FString id = "";
    const TArray<UOdysseyLayer*> layers = Children;
    for (UOdysseyLayer* layer : layers)
    {
        UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
        if (!animationLayer)
            continue;

        id += animationLayer->GetFrameId(iFrameIndex);
    }

    return id;
}

UOdysseyAnimationLayer::FOnRenderImageDataChanged&
UOdysseyAnimationLayer::OnRenderImageDataChanged()
{
    static FOnRenderImageDataChanged onRenderImageDataChanged;
    return onRenderImageDataChanged;
}

UOdysseyAnimationLayer::FOnRenderImageIdChanged&
UOdysseyAnimationLayer::OnRenderImageIdChanged()
{
    static FOnRenderImageIdChanged onRenderImageIdChanged;
    return onRenderImageIdChanged;
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
UOdysseyAnimationLayer::RenderImageDataChanged(const FOdysseyAnimationRenderImageId& iFrameId)
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return;

    RenderImageChanged(iFrameId, { ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height()) });
}

void
UOdysseyAnimationLayer::RenderImageChanged(const FOdysseyAnimationRenderImageId& iFrameId, const TArray<::ULIS::FRectI>& iRects)
{
    if ( !Parent )
        return;

    //PATCH BEGIN: because Unreal Undo does not make package dirty correctly after a save
    MarkPackageDirty();
    //PATCH END:

    UOdysseyAnimationLayer* parent = Cast<UOdysseyAnimationLayer>(Parent);
    if (parent)
        parent->ChildRenderImageChanged(this, iFrameId, iRects);
    
    OnRenderImageDataChanged().Broadcast(this, iFrameId, iRects); //always send at least 1 interactive event
    if (!iIsInteractive )
        OnRenderImageDataChanged().Broadcast(this, iFrameId, iRects);
}

void
UOdysseyAnimationLayer::ChildRenderImageDataChanged(UOdysseyAnimationLayer* iLayer, const FOdysseyAnimationRenderImageId& iFrameId, const TArray<::ULIS::FRectI>& iRects)
{
    RenderImageDataChanged(iFrameId, iRects);
}

void
UOdysseyAnimationLayer::RenderImageIdChanged()
{
    OnRenderImageIdChanged().Broadcast(this);
}

void
UOdysseyAnimationLayer::IsActivatedChanged()
{
    Super::IsActivatedChanged();
    RenderImageChanged(mRenderImageState->GetFrameIds());
}

void
UOdysseyAnimationLayer::OnTrackerChildrenChanged(const TArray<UOdysseyLayer*>& iOldChildren)
{

    //TODO: Find a way to invalid only frame ranges that actually changed, instead of the while layer
    RenderImageChanged(mRenderImageState->GetFrameIds());
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
