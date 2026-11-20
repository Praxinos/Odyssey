// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyTextureLayer.h"
#include "OdysseyBlendingMode.h"
#include "Misc/TransactionObjectEvent.h"
#include "Misc/ITransaction.h"
#include "Misc/ITransactionObjectAnnotation.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyRasterContent.h"

#include <ULIS>

#include "OdysseyTextureLayerImageRaster.generated.h"

class UOdysseyTiledImage;

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayerImageRaster
    : public UOdysseyTextureLayer
{
    GENERATED_BODY()

public:
    ~UOdysseyTextureLayerImageRaster();
    UOdysseyTextureLayerImageRaster();

public:
    //UOdysseyLayer overrides
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

public:
    // Public API

    /**
     * Returns the pixels of the layer, to read or write them; see FOdysseyRasterContent.
     * Its functions fail while the layer is not in a layer stack.
     */
    FOdysseyRasterContent& GetRasterContent() const;


    /** Returns the tiled image holding the pixels, without creating it. */
    const UOdysseyTiledImage* GetTiledImage() const;

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetIsAlphaLocked(bool Value);

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool IsAlphaLocked() const;

public:
    // UOdysseyLayer Overrides

    /**
     * @brief Merges this layer over iLayer (modifying its content)
     * Only works with Layer class being a child of classes returned by GetMergeLayerTypes()
     *
     */
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

public:
    // UObject overrides
    virtual void PostLoad() override;
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
    virtual void BeginDestroy() override;

public:
    //UObject overrides

    /**
     * @brief Serialize this object
     *
     * @param Ar
     */
    virtual void Serialize(FArchive& Ar) override;

public:
    //IOdysseyRenderingAbility overrides
    virtual TArray<FGuid> GetRenderingComposition(uint64 iRenderType, int iFrame = 0) const override;

    /** Converts the tiled image when the sRGB setting of the texture changed. */
    virtual void ColorSettingsChanged() override;
    virtual bool BuildRenderPipelineInternal(FRenderFunction& OutRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& Parents, const FOdysseyRenderContext* RenderContext) const override;

private:
    ::ULIS::eFormat GetRasterFormat() const;
    void OnContentChanged(const TArray<FIntRect>& Rects, bool bInteractive);
    void MigrateLegacyContent();

private:
    //Import
    friend class FOdysseyTextureLayerImageRasterImport;

private:
    /** Pixels read from assets saved before the tiled image storage. Moved into the tiled image after loading. */
    TSharedPtr<FOdysseyRasterBlock> LegacyRasterBlock;

    /**
     * The render texture of assets saved before the tiled image storage, which held the pixels of the layer.
     * Only loaded: moved into the tiled image after loading, then dropped. Named as the former property to load it.
     */
    UPROPERTY(NonTransactional)
    TObjectPtr<UTexture2D> Texture;

    /** The pixels of the layer. Its identity never changes; its tiles are transactional. */
    UPROPERTY(Instanced, NonTransactional)
    TObjectPtr<UOdysseyTiledImage> TiledImage;

    mutable TSharedPtr<FOdysseyRasterContent> RasterContent;

private:
    UPROPERTY(NonTransactional)
    bool bIsAlphaLocked = false;
};
