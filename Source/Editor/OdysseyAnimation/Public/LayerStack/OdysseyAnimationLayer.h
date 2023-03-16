// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"

#include "UObject/OdysseyObjectPropertyTracker.h"
#include "Misc/OdysseyHandle.h"
#include <ULIS>

#include "OdysseyAnimationLayer.generated.h"

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayer
    : public UOdysseyLayer
{
    GENERATED_BODY()

public:
    //Delegates

    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     * @param UOdysseyAnimationLayer* Layer
     * @param const TRange<int>& FrameRange
     * @param const TArray<::ULIS::FRectI>& Rects
     * @param bool IsInteractive
     */
    DECLARE_MULTICAST_DELEGATE_FourParams(FOnRenderImageChanged, UOdysseyAnimationLayer*, const TRange<int>&, const TArray<::ULIS::FRectI>&, bool)
    static FOnRenderImageChanged& OnRenderImageChanged();

public:
    UOdysseyAnimationLayer();

    virtual void PostInitProperties();

    virtual void PostLoad();

public:
    //Getters
    UOdysseyAnimation* GetAnimation();
    virtual TRange<int> GetFrameRange() const;

    /**
     * @brief Returns a string identifying the frame composition (which layers)
     * 
     * @return FString 
     */
    virtual FString GetFrameId(int iFrameIndex) const;

public:
    //Frame Range System
    /*
     * Each Layer has a range of frames on which it can render an image or sound
     * 
     * The range can be closed or open
     * 
     */
    //TRange<int> GetRenderImageFrameRange() const;

    //TRange<float> GetRenderSoundFrameRange() const;

public:
    //Image Rendering

    /**
     * @brief Reders the given layer's image
     * 
     * @param iLayers 
     * @param ioBlock 
     * @param iRect 
     * @param iPos 
     * @param iWaitList 
     * @return TArray<::ULIS::FEvent> 
     */
    static TArray<::ULIS::FEvent> RenderLayersImage(TArray<UOdysseyLayer*> iLayers, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);

    /**
     * @brief Renders an image over the given Block
     * Takes into account the size / format of the given block
     */
    virtual TArray<::ULIS::FEvent> RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>  ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);

    /**
     * @brief Copies an image in the given Block
     * Takes into account the size / format of the given block
     */
    virtual TArray<::ULIS::FEvent> CopyImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);

    /**
     * @brief Call RenderImageChanged when the image rendering changed for this layer
     * 
     * If no range is provided, the full range of the layer is used
     * If no rects is provided, the full rect of the layer is used
     */
    void RenderImageChanged(bool iIsInteractive);
    void RenderImageChanged(const TRange<int>& iFrameRange, bool iIsInteractive);
    void RenderImageChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);
    virtual void RenderImageChanged(const TRange<int>& iFrameRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);

    /**
     * @brief Called when one of the direct children layer render image changed
     * 
     * Called by the child layer RenderImageChanged() function
     */
    virtual void ChildRenderImageChanged(UOdysseyAnimationLayer* iLayer, const TRange<int>& iFrameRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);

    /**
     * @brief Preloads the layers and keeps them preloaded untile the hiven handles are destroyed
     */
    virtual TSharedPtr<IOdysseyHandle> Preload(int iFrame);

protected:
    //Property changes
    virtual void IsActivatedChanged() override;
    virtual void OnTrackerChildrenChanged(const TArray<UOdysseyLayer*>& iOldChildren);

private:
    FOdysseyObjectPropertyTracker mPropertyTracker;

    // 
    // OPTIMIZATIONS
    //
    TMap<int, TWeakPtr<IOdysseyHandle>> mPreloadHandles;

};
