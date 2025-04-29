// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationLayerImageRaster.generated.h"

UCLASS(BlueprintType)
class ODYSSEYLAYERSTACKEDITOR_API UOdysseyAnimationLayerImageRaster
    : public UOdysseyAnimationLayer
{
    GENERATED_BODY()

public:
    // UObject overrides
    virtual void PostInitProperties() override;
    virtual void Serialize(FArchive& Ar) override;

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

public:
    //IOdysseyRenderingAbility overrides
    virtual TArray<FGuid> GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const override;
    virtual void RenderToTexture(FCanvas* iCanvas, FFrameNumber iFrame, const FIntRect& iSrcRect, const FIntRect& iDstRect) const override;

private:
    TSharedPtr<IOdysseyMedia> CreateMediaRaster(int iFrameIndex);
    TSharedPtr<IOdysseyMedia> GetCellMediaRaster(uint32 iFrameIndex) const;
    void AutoCreateCell(int iFrameIndex);
    void CreateCell( const FName& iCellType);

private:
    friend class FOdysseyAnimationLayerImageRasterImport;

    UFUNCTION(BlueprintSetter)
    void IsAlphaLockedBlueprintSetter(bool Value);

public:
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Layer", BlueprintSetter=IsAlphaLockedBlueprintSetter, NonTransactional)
    bool IsAlphaLocked = false;
};
