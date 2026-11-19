// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Engine/TextureRenderTarget2D.h"

#include "OdysseyLayerStack.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "TickableEditorObject.h"
#include "OdysseyInvalidTileMap.h"
#include "OdysseyRenderingAbility.h"

#include <ULIS>

#include "OdysseyTextureLayerStack.generated.h"

class FOdysseyRenderPreview;
class UOdysseyTextureLayer;
class IOdysseyImageRenderer;
class UTextureRenderTarget2D;

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
    , public FTickableEditorObject //Allows us to react to Tick events
{
    GENERATED_BODY()

public:
    static UOdysseyTextureLayerStack* CreateEmptyFromTexture(UTexture2D* iTexture, UObject* iOuter);
    static UOdysseyTextureLayerStack* CreateFromTexture(UTexture2D* iTexture, UObject* iOuter);
    static UOdysseyTextureLayerStack* CreateWithEmptyVectorLayer(UTexture2D* iTexture, UObject* iOuter);
    static UOdysseyTextureLayerStack* CreateWithEmptyRasterLayer(UTexture2D* iTexture, UObject* iOuter);
    ~UOdysseyTextureLayerStack();
    UOdysseyTextureLayerStack();

public:
    //virtual int GetWidth() const override;
    //virtual int GetHeight() const override;

    virtual void PostInitProperties() override;
    virtual void PostLoad() override;

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|LayerStack")
    UTexture2D* GetTexture() const;

    /** Returns true when the texture holds 8-bit sRGB encoded colors, which the layers hold decoded. */
    bool IsSRGB() const;

    /** Returns true when the layers of an sRGB texture are blended with linear colors rather than encoded colors. */
    bool UsesLinearCompositing() const;

    /** Returns true when the sRGB setting of the texture changes its colors rather than its bytes. */
    bool KeepsColorsOnSRGBChange() const;

    /** Returns the conversion of the linear colors of a layer into the colors this stack blends. */
    EOdysseyColorConversion GetLayerColorConversion() const;

    /** The views display the bytes of the texture: the stack renders them, a layer blended with linear colors renders them decoded. */
    virtual EOdysseyColorConversion GetDisplayColorConversion(const UOdysseyLayer* RenderedLayer) const override;

    /** Applies a change of the color settings of the texture or of the layer stack to the layers and the texture. */
    void ColorSettingsChanged();

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
     * Sets the view preview rendered into the texture while it is fast updated, e.g. the uncommitted edits of a tool.
     * The texture source, which is saved, never holds the preview.
     *
     * @param Preview The preview; null to render without preview.
     */
    void SetRenderPreview(TSharedPtr<FOdysseyRenderPreview> Preview);

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

protected:
    /** Encodes the blended colors when they are linear: the texture holds encoded colors. */
    virtual EOdysseyColorConversion GetContentColorConversion(const FOdysseyRenderContext* RenderContext) const override;

public:
    //IOdysseyRenderingAbility overrides
    virtual UTextureRenderTarget2D* CreateRenderingRenderTarget() const override;
    virtual UTexture2D* CreateExportTexture( const FString& iAssetName, const FString& iPackagePath, UClass* iAssetClass, UFactory* iFactory ) override;
    virtual FIntRect GetDefaultRenderRect() const override;

private:
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT( UOdysseyTextureLayerStack, STATGROUP_Tickables); }

private:
    void CreateRenderTarget();
    void UpdateRenderTargetFormat();

    void CompressTexture();
    void UncompressTexture();
    void FastUpdateTexture(const TArray<FIntRect>& iRects, bool bWithPreview = true);
    void OnRenderPreviewInvalidated(const TArray<FIntRect>& Rects);
    bool HasPreviewOverrides() const;
    /** Renders the whole texture without the preview, before its source is written. */
    void RenderWithoutPreview();
    void UpdateTextureSource();

    //React to Texture property changes, updating texture source if we are in fast update
    void OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain);
    void OnPackagePreSave(UPackage* iPackage, FObjectPreSaveContext ObjectSaveContext);
    void OnPackageSaved(const FString& iPackageFilename, UPackage* iPackage, FObjectPostSaveContext ObjectSaveContext);
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);
    void OnObjectPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent);
    /** Returns the render target format blending the layers. */
    ETextureRenderTargetFormat GetCompositingFormat() const;

private:
    TSharedPtr<FOdysseySurfaceTexture2DEditable> mTextureFastUpdateSurface;
    int mTextureCompressionNone;
    TextureMipGenSettings mPreviousMipSettings;

    EOdysseyTextureLayerStackTextureUpdateMode mTextureUpdateMode = EOdysseyTextureLayerStackTextureUpdateMode::OnTick;
    FOdysseyInvalidTileMap mInvalidTileMap;
    TSharedPtr<FOdysseyRenderPreview> mRenderPreview;
    FDelegateHandle mRenderPreviewInvalidatedHandle;

public:
    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> RenderTarget;
};
