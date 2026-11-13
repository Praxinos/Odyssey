// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyAnimationCell.h"

#if WITH_EDITOR
#include "OdysseyInvalidTileMap.h"
#include <ULIS>
#endif

#include "OdysseyAnimationCellImageRaster.generated.h"

#if WITH_EDITOR
class FOdysseyRasterBlock;
class FOdysseyMediaRaster;
class UOdysseyTiledImage;
#endif

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationCellImageRaster
    : public UOdysseyAnimationCell
{
    GENERATED_BODY()

public:
    UOdysseyAnimationCellImageRaster();

public:
    //IOdysseyRenderingAbility overrides
#if WITH_EDITOR
    virtual void InitTexture() override;
#endif
    virtual bool BuildRenderPipelineInternal(
        FFrameNumber iFrame,
        uint64 iType,
        IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
        const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents
    ) const override;
    virtual TArray<FGuid> GetRenderingComposition(uint64 iRenderType, int iFrameIndex) const override;
    virtual FIntRect GetDefaultRenderRect() const override;

#if WITH_EDITORONLY_DATA
public:
    virtual void Serialize(FArchive& Ar) override;
    virtual void OldSerialize(FArchive& Ar) override; //DEPRECATED: Keep that for compatibility with early versions of Odyssey
#endif

#if WITH_EDITOR
public:
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;
    /** Returns the existing editor snapshot without creating or synchronizing it. */
    const UOdysseyTiledImage* GetTiledImage() const;
    /** Synchronizes the editor snapshot from the authoritative raster render texture. */
    bool UpdateTiledImage();
    virtual FOdysseyMediaProvider GetMediaProvider() const override;
    virtual void Clear() override;

public:
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
    virtual void PostTransacted(const FTransactionObjectEvent& TransactionEvent) override;
    FCriticalSection* GetImageRenderingMutex() const;

private:
    void InitTexture() const;
    void InitRasterBlock() const;
    TArray<::ULIS::FEvent> RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FOdysseyInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList);
    void OnBlockChanged(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockCommited(const TArray<::ULIS::FRectI>& iRects);
    void InvalidateTiledImage(const TArray<::ULIS::FRectI>& Rects);

private:
    //Import/Export
    friend class FOdysseyAnimationCellImageRasterExport;
    friend class FOdysseyAnimationCellImageRasterImport;

private:
    mutable TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
    mutable FCriticalSection mImageRenderingMutex;
    mutable TWeakPtr<FOdysseyMediaRaster> mMediaRaster;
    bool bTiledImageDirty = true;
    TSet<FIntPoint> PendingTiledImageTiles;
    FIntPoint TiledImageSize = FIntPoint::ZeroValue;
    FGuid TiledImageSourceId;
#endif

#if WITH_EDITORONLY_DATA
private:
    // UObject avoids an UHT dependency on the Editor-only OdysseyImaging module in Game targets.
    // Access through the typed editor API above. This reference and its image are never cooked.
    UPROPERTY(Instanced, NonTransactional)
    TObjectPtr<UObject> TiledImage;
#endif
};
