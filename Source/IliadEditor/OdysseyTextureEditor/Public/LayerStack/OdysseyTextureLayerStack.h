// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayerStack.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "Misc/OdysseyHandle.h"
#include "Tickable.h"
#include "ULISInvalidTileMap.h"
#include "OdysseyImageRenderingAbility.h"

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
class ODYSSEYTEXTUREEDITOR_API UOdysseyTextureLayerStack
    : public UOdysseyLayerStack
    , public FTickableGameObject
{
    GENERATED_BODY()

public:
    static UOdysseyTextureLayerStack* CreateEmptyFromTexture(UTexture2D* iTexture, UObject* iOuter);
    static UOdysseyTextureLayerStack* CreateFromTexture(UTexture2D* iTexture, UObject* iOuter);
    static UOdysseyTextureLayerStack* CreateWithEmptyVectorLayer(UTexture2D* iTexture, UObject* iOuter);
    ~UOdysseyTextureLayerStack();
    UOdysseyTextureLayerStack();

    virtual void PostLoad() override;

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|LayerStack")
    UTexture2D* GetTexture() const;

public:
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
    void UpdateTexture(bool iForceRefresh = false);

public:
    //FOdysseyImageRenderingAbility overrides
    virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;

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
    void OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent);

private:
    TSharedPtr<FOdysseySurfaceTexture2DEditable> mTextureFastUpdateSurface;
    int mTextureCompressionNone;

    EOdysseyTextureLayerStackTextureUpdateMode mTextureUpdateMode = EOdysseyTextureLayerStackTextureUpdateMode::OnTick;
    FULISInvalidTileMap mInvalidTileMap;
    TSharedPtr<IOdysseyImageRenderer> mRenderer; //Used for FastUpdateTexture to keep the LayerStack Ready to render
};
