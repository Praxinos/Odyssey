// IDDN.FR.001.060015.013.S.X.2019.000.00000
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

private:
    void InitTexture();
    void InitRasterBlock();
    void ConvertRasterBlock();
    void BindRasterBlockDelegates();
    void UnbindRasterBlockDelegates();
    void SanitizeRasterBlock(const TArray<::ULIS::FRectI>& iRects);

    TArray<::ULIS::FEvent> RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FOdysseyInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList);

private:
    //Import/Export
    friend class FOdysseyTextureLayerImageRasterExport;
    friend class FOdysseyTextureLayerImageRasterImport;

private:
    mutable TSharedPtr<FOdysseyRasterBlock> RasterBlock;

private:
    UPROPERTY(NonTransactional)
    bool bIsAlphaLocked = false;
};
