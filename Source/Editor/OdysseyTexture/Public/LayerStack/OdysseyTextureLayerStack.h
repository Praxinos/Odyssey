// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayerStackNew.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyTextureLayerStack.generated.h"

class UOdysseyTextureLayer;

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayerStack
    : public UOdysseyLayerStack
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

public:
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

public:
    TArray<::ULIS::FEvent> RenderLayersImage(TArray<UOdysseyLayer*> iLayers, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);

public:
    TSharedPtr<FOdysseySurfaceTexture2DEditable> GetSurface() const;

public:
    //Texture Update / Edition
    virtual void ApplyPerformanceMode(eOdysseyPerformanceMode iPerformanceMode) override;

private:
    void ActivateTextureFastUpdate();
    void InactivateTextureFastUpdate();
    void FastUpdateTexture(const TArray<::ULIS::FRectI>& iRects);
    void UpdateTextureSource();

    //React to Texture property changes, updating texture source if we are in fast update
    void OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain);
    void OnPackagePreSave(UPackage* iPackage, FObjectPreSaveContext ObjectSaveContext);
    void OnPackageSaved(const FString& iPackageFilename, UPackage* iPackage, FObjectPostSaveContext ObjectSaveContext);

private:
    TSharedPtr<FOdysseySurfaceTexture2DEditable> mTextureFastUpdateSurface;
    int mTextureCompressionNone;
};