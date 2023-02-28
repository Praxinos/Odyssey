// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayerStack.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "Misc/OdysseyHandle.h"

#include <ULIS>

#include "OdysseyTextureLayerStack.generated.h"

class UOdysseyTextureLayer;

UENUM()
enum class EOdysseyTextureLayerStackTextureUpdateMode
{
    Manually,
    OnTick,
    Instantaneous
};

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayerStack
    : public UOdysseyLayerStack
    , public FTickableGameObject
{
    GENERATED_BODY()

public:
    static UOdysseyTextureLayerStack* CreateEmptyFromTexture(UTexture2D* iTexture, UObject* iOuter);
    static UOdysseyTextureLayerStack* CreateFromTexture(UTexture2D* iTexture, UObject* iOuter);
    UOdysseyTextureLayerStack();

public:
    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnRenderImageChanged, UOdysseyTextureLayerStack*, const TArray<::ULIS::FRectI>&, bool)
    static FOnRenderImageChanged& OnRenderImageChanged(); //Delegate

public:
    UFUNCTION(BlueprintPure, Category="LayerStack")
    UTexture2D* GetTexture() const;

public:
    /**
     * @brief Renders an image in the given Block
     * Takes into account the size / format of the given block
     * 
     */
    TArray<::ULIS::FEvent> RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);

    /**
     * @brief Called when one of the direct children layer render image changed
     * 
     * Called by the child layer RenderImageChanged() function
     */
    void OnRootLayerRenderImageChanged(UOdysseyTextureLayer* iLayer, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);

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
    TArray<::ULIS::FEvent> RenderLayersImage(TArray<UOdysseyLayer*> iLayers, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);

public:
    /**
     * @brief Preloads the layers and keeps them preloaded untile the hiven handles are destroyed
     * One handle corresponds to something being held in memory
     */
    TSharedPtr<IOdysseyHandle> Preload();

    /**
     * @brief Returns the surface used for fast update (see ActivateTextureFastUpdate)
     * 
     * @return TSharedPtr<FOdysseySurfaceTexture2DEditable> 
     */
    TSharedPtr<FOdysseySurfaceTexture2DEditable> GetSurface() const;

    /**
     * @brief Uncompresses the texture to allow realtime update
     */
    void ActivateTextureFastUpdate();

    /**
     * @brief Uncompresses the texture to disallow realtime editing
     */
    void InactivateTextureFastUpdate();

    /**
     * @brief Defines when the texture should be updated, and how frequently
     * Manually = does not update the texture automatically
     * OnTick = refresh the texture at each engine tick if needed
     * Instantaneous = refresh the texture as soon as anything has changed in the layerstack
     * 
     * Default is OnTick
     * 
     * This is a transient setter, it goes back to OnTick at each new Unreal session
     */
    void SetTextureUpdateMode(EOdysseyTextureLayerStackTextureUpdateMode iMode);

    /**
     * @brief Set the Texture Update Mode
     * Manually = does not update the text automatically
     * OnTick = refreshed the texture at each engine tick if needed
     * Instantaneous = refreshes the texture as soon as anything has changed in the layerstack
     * 
     * Default is OnTick
     */
    EOdysseyTextureLayerStackTextureUpdateMode GetTextureUpdateMode();

    /**
     * @brief Set the Texture Update Mode
     * Forces the texture to be updated now
     */
    void UpdateTexture();

private:
    virtual bool IsTickableInEditor() const override { return true; }
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT( UOdysseyTextureLayerStack, STATGROUP_Tickables); }
    virtual void Tick(float DeltaTime) override;

private:
    void CompressTexture();
    void UncompressTexture();
    void FastUpdateTexture(const TArray<::ULIS::FRectI>& iRects);
    void UpdateTextureSource();

    //React to Texture property changes, updating texture source if we are in fast update
    void OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain);
    void OnPackagePreSave(UPackage* iPackage, FObjectPreSaveContext ObjectSaveContext);
    void OnPackageSaved(const FString& iPackageFilename, UPackage* iPackage, FObjectPostSaveContext ObjectSaveContext);

private:
    TSharedPtr<FOdysseySurfaceTexture2DEditable> mTextureFastUpdateSurface;
    int mTextureCompressionNone;

    EOdysseyTextureLayerStackTextureUpdateMode mTextureUpdateMode = EOdysseyTextureLayerStackTextureUpdateMode::OnTick;
    TArray<::ULIS::FRectI> mInvalidRects;
};