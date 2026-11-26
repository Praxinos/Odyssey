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
     * Returns the pixels of the cell, to read or write them; see FOdysseyRasterContent.
     * Its functions fail while the cell is not in an animation.
     */
    FOdysseyRasterContent& GetRasterContent() const;

    /** Returns the tiled image holding the pixels, without creating it. */
    const UOdysseyTiledImage* GetTiledImage() const;
    virtual FOdysseyMediaProvider GetMediaProvider() const override;
    virtual void Clear() override;

public:
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
    virtual void BeginDestroy() override;

private:
    /**
     * Moves the pixels of a legacy asset into the tiled image, and converts it to the format of the animation.
     * Called after loading, including by OldSerialize(), after duplication and before saving.
     * @return True when legacy pixels were moved into the tiled image.
     */
    bool UpdateContent();
    ::ULIS::eFormat GetRasterFormat() const;

private:
    //Import
    friend class FOdysseyAnimationCellImageRasterImport;

private:
    /** Pixels read from assets saved before the tiled image storage. Moved into the tiled image after loading. */
    TSharedPtr<::ULIS::FBlock> mLegacyPixels;
    mutable TSharedPtr<FOdysseyRasterContent> mRasterContent;
#endif

#if WITH_EDITORONLY_DATA
private:
    /** The pixels of the cell. Its identity never changes; its tiles are transactional. Never cooked. */
    UPROPERTY(Instanced, NonTransactional)
    TObjectPtr<UOdysseyTiledImage> TiledImage;

    /**
     * The render texture of assets saved before the tiled image storage, which held the pixels of the cell.
     * Only loaded: moved into the tiled image after loading, then dropped. Named as the former property to load it.
     */
    UPROPERTY(NonTransactional)
    TObjectPtr<UTexture2D> Texture;
#endif
};
