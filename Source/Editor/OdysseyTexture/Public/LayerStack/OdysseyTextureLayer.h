// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"

#include "UObject/OdysseyObjectPropertyTracker.h"
#include "Misc/OdysseyHandle.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyStaticImageRenderingAbility.h"
#include <ULIS>

#include "OdysseyTextureLayer.generated.h"

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayer
    : public UOdysseyLayer
    , public FOdysseyStaticImageRenderingAbility
{
    GENERATED_BODY()

public:
    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnRenderImageChanged, UOdysseyTextureLayer*, const TArray<::ULIS::FRectI>&, bool)

public:
    static FOnRenderImageChanged& OnRenderImageChanged(); //Delegate

public:
    UOdysseyTextureLayer();

protected:
    void IsActivatedChanged();
    //void ChildrenChanged();

public:
    virtual void PostInitProperties();
    virtual void PostLoad();

public:
    /**
     * @brief Renders an image over the given Block
     * Takes into account the size / format of the given block
     */
    virtual TArray<::ULIS::FEvent> RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>  ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);

    /**
     * @brief Copies an image in the given Block
     * Takes into account the size / format of the given block
     */
    virtual TArray<::ULIS::FEvent> CopyImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);

    /**
     * @brief Calls the RenderImageChanged delegate and informs the parent layers directly
     * If there's no parent, informs the layerstack
     * 
     * @param iRects 
     */
    virtual void RenderImageChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);
    virtual void RenderImageChanged(bool iIsInteractive);

    /**
     * @brief Called when one of the direct children layer render image changed
     * 
     * Called by the child layer RenderImageChanged() function
     */
    virtual void ChildRenderImageChanged(UOdysseyTextureLayer* iLayer, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);

    /**
     * @brief Preloads the layers and keeps them preloaded untile the hiven handles are destroyed
     * One handle corresponds to something being held in memory
     */
    virtual TSharedPtr<IOdysseyHandle> Preload();

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType) const override;
	virtual TSharedPtr<IOdysseyHandle> PreloadImageRendering(IOdysseyImageRenderer::eRenderType iRenderType) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;

private:
    void OnTrackerChildrenChanged(const TArray<UOdysseyLayer*>& iOldChildren);

private:
    FOdysseyObjectPropertyTracker mPropertyTracker;

    // 
    // OPTIMIZATIONS
    //
    TWeakPtr<IOdysseyHandle> mPreloadHandle;
};
