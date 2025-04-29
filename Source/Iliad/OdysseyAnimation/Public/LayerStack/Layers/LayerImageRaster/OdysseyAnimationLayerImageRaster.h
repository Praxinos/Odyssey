// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetIsAlphaLocked(bool Value);

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
