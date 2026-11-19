// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyAnimationCell.h"

#if WITH_EDITOR
#include "OdysseyInvalidTileMap.h"
#include "OdysseyRasterContent.h"
#include <ULIS>
#endif

#include "OdysseyAnimationCellImageRaster.generated.h"

class UOdysseyTiledImage;
#if WITH_EDITOR
class FOdysseyRasterBlock;
class FOdysseyMediaRaster;
#endif

UCLASS(BlueprintType)
class ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationCellImageRaster
    : public UOdysseyAnimationCell
{
    GENERATED_BODY()

public:
    UOdysseyAnimationCellImageRaster();

public:
    //IOdysseyRenderingAbility overrides
#if WITH_EDITOR
    /** Raster cells have no render texture: their pixels are held by the tiled image. */
    virtual void InitTexture() override;
#endif
    virtual bool BuildRenderPipelineInternal(
        IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
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
    /**
     * Transitional: returns the raster block edited by tools, derived from the tiled image.
     * Its committed edits are written into the tiled image. Null when the cell is not in an animation.
     */
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;
    /** Returns the tiled image holding the pixels, without creating it. */
    const UOdysseyTiledImage* GetTiledImage() const;
    virtual FOdysseyMediaProvider GetMediaProvider() const override;
    virtual void Clear() override;

public:
    virtual void PostLoad() override;
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
    virtual void BeginDestroy() override;
    FCriticalSection* GetImageRenderingMutex() const;

private:
    FOdysseyRasterContent& GetRasterContent() const;
    ::ULIS::eFormat GetRasterFormat() const;
    void OnContentChanged(const TArray<FIntRect>& Rects, bool bInteractive);
    void MigrateLegacyContent();
    TArray<::ULIS::FEvent> RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FOdysseyInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList);

private:
    //Import/Export
    friend class FOdysseyAnimationCellImageRasterExport;
    friend class FOdysseyAnimationCellImageRasterImport;

private:
    /** Pixels read from assets saved before the tiled image storage. Moved into the tiled image after loading. */
    TSharedPtr<FOdysseyRasterBlock> mLegacyRasterBlock;
    mutable TUniquePtr<FOdysseyRasterContent> mRasterContent;
    mutable FCriticalSection mImageRenderingMutex;
    mutable TWeakPtr<FOdysseyMediaRaster> mMediaRaster;
#endif

#if WITH_EDITORONLY_DATA
private:
    /** The pixels of the cell. Its identity never changes; its tiles are transactional. Never cooked. */
    UPROPERTY(Instanced, NonTransactional)
    TObjectPtr<UOdysseyTiledImage> TiledImage;
#endif
};
