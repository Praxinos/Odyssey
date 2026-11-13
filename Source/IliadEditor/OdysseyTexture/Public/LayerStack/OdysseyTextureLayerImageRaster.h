// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyTextureLayer.h"
#include "OdysseyBlendingMode.h"
#include "Misc/TransactionObjectEvent.h"
#include "Misc/ITransaction.h"
#include "Misc/ITransactionObjectAnnotation.h"
#include "OdysseyRasterBlock.h"

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
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;

    /** Returns the existing snapshot without creating or updating it. May be null or out of date. */
    const UOdysseyTiledImage* GetTiledImage() const;

    /** Creates or synchronizes the snapshot from the authoritative render texture on the game thread.
     * Returns false when the layer has no valid source image.
     */
    bool UpdateTiledImage();

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

protected:
    void OnBlockChanged(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockCommited(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockPtrChanged();

public:
    // UObject overrides
    virtual void PostLoad() override;
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
    virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
    virtual void PostTransacted(const FTransactionObjectEvent& TransactionEvent) override;

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
    virtual bool BuildRenderPipelineInternal(FFrameNumber Frame, uint64 Type, FRenderFunction& OutRenderFunction,
        const FCanRenderFunction& CanRenderFunction, const TArray<const IOdysseyTextureRenderingAbility*>& Parents) const override;

private:
    void InitTexture();
    void InitRasterBlock();
    void ConvertRasterBlock();
    void BindRasterBlockDelegates();
    void UnbindRasterBlockDelegates();
    void SanitizeRasterBlock(const TArray<::ULIS::FRectI>& iRects);
    void InvalidateTiledImage(const TArray<::ULIS::FRectI>& Rects);

    TArray<::ULIS::FEvent> RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FOdysseyInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList);

private:
    //Import/Export
    friend class FOdysseyTextureLayerImageRasterExport;
    friend class FOdysseyTextureLayerImageRasterImport;

private:
    mutable TSharedPtr<FOdysseyRasterBlock> RasterBlock;

    // A derived representation, excluded from transactions: the raster undo path invalidates it.
    UPROPERTY(Instanced, NonTransactional)
    TObjectPtr<UOdysseyTiledImage> TiledImage;

    bool bTiledImageDirty = true;
    TSet<FIntPoint> PendingTiledImageTiles;
    FIntPoint TiledImageSize = FIntPoint::ZeroValue;
    FGuid TiledImageSourceId;

private:
    UPROPERTY(NonTransactional)
    bool bIsAlphaLocked = false;
};
