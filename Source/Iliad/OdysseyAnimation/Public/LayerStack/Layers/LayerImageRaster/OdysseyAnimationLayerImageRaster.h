// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationLayerImageRaster.generated.h"

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayerImageRaster
    : public UOdysseyAnimationLayer
{
    GENERATED_BODY()

public:
    //IOdysseyRenderingAbility overrides
    virtual void PostInitProperties() override;

#if WITH_EDITOR
public:
    /**
     * Lock or unlock the Alpha Mask of this Raster Layer.
     *
     * When locked, transparency is blocked and you can only paint
     * on existing pixels (useful to recolor an outline, for example).
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetIsAlphaLocked(bool Value);

    /**
     * Return true if the Alpha Mask of this Raster Layer is locked.
     *
     * When locked, you can only paint on existing pixels.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    bool IsAlphaLocked() const;

public:
    //UOdysseyLayer overrides
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

public:
    // UOdysseyLayer Overrides

    /**
     * @brief Merges this layer over iLayer (modifying its content)
     * Only works with Layer class being a child of classes returned by GetMergeLayerTypes()
     *
     */
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

private:
    TSharedPtr<IOdysseyMedia> CreateMediaRaster(int iFrameIndex);
    TSharedPtr<IOdysseyMedia> GetCellMediaRaster(uint32 iFrameIndex) const;
    void AutoCreateCell(int iFrameIndex);
    void CreateCell( const FName& iCellType);

private:
    friend class FOdysseyAnimationLayerImageRasterImport;
#endif

#if WITH_EDITORONLY_DATA
public:
    // UObject overrides
    virtual void Serialize(FArchive& Ar) override;

private:
    UPROPERTY(NonTransactional)
    bool bIsAlphaLocked = false;
#endif
};
