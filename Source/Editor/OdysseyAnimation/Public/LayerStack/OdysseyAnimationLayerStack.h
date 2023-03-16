// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayerStack.h"
#include "Misc/OdysseyHandle.h"

#include <ULIS>

#include "OdysseyAnimationLayerStack.generated.h"

class UOdysseyAnimationLayer;

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayerStack
    : public UOdysseyLayerStack
{
    GENERATED_BODY()

public:
    UOdysseyAnimationLayerStack();

public:
    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     * @param UOdysseyAnimationLayerStack* LayerStack
     * @param const TRange<int>& FrameRange
     * @param const TArray<::ULIS::FRectI>& Rects
     * @param bool IsInteractive
     */
    DECLARE_MULTICAST_DELEGATE_FourParams(FOnRenderImageChanged, UOdysseyAnimationLayerStack*, const TRange<int>&, const TArray<::ULIS::FRectI>&, bool)
    static FOnRenderImageChanged& OnRenderImageChanged(); //Delegate

public:
    UFUNCTION(BlueprintPure, Category="LayerStack")
    UOdysseyAnimation* GetAnimation() const;

public:
    //Animation Specific
    /**
     * @brief Will return the actual frame range 
     * 
     * @return TRange<int> 
     */
    TRange<int> GetFrameRange() const;
    
    /**
     * @brief Returns a string identifying the frame composition (which layers)
     * 
     * @return FString 
     */
    virtual FString GetFrameId(int iFrameIndex) const;

public:
    /**
     * @brief Renders an image in the given Block
     * Takes into account the size / format of the given block
     * 
     */
    TArray<::ULIS::FEvent> RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);

public:
    /**
     * @brief Preloads the layers and keeps them preloaded untile the hiven handles are destroyed
     */
    TSharedPtr<IOdysseyHandle> Preload(int iFrame);

private:
    friend class UOdysseyAnimationLayerRoot;

    /**
     * @brief Called when one of the direct children layer render image changed
     * 
     * Called by the child layer RenderImageChanged() function
     */
    void OnRootLayerRenderImageChanged(UOdysseyAnimationLayer* iLayer, const TRange<int>& iFrameRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);
};