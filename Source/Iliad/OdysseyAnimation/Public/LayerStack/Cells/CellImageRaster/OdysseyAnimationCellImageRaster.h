// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationCell.h"

#if WITH_EDITOR
#include "OdysseyInvalidTileMap.h"
#endif

#include "OdysseyAnimationCellImageRaster.generated.h"

#if WITH_EDITOR
class FOdysseyRasterBlock;
class FOdysseyMediaRaster;
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

    virtual bool BuildRenderPipeline(
        FFrameNumber iFrame,
        EOdysseyRenderingType iType,
        FOdysseyTextureRenderFunction& oRenderFunction
    ) const override;
    virtual TArray<FGuid> GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const override;
    virtual FIntRect GetDefaultRenderRect() const override;

#if WITH_EDITORONLY_DATA
public:
    virtual void Serialize(FArchive& Ar) override;
    virtual void OldSerialize(FArchive& Ar) override; //DEPRECATED: Keep that for compatibility with early versions of Odyssey
#endif

#if WITH_EDITOR
public:
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;
    UTexture2D* GetTexture() const;
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

public:
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
    FCriticalSection* GetImageRenderingMutex() const;

private:
    void InitTexture() const;
    void InitRasterBlock() const;
    TArray<::ULIS::FEvent> RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FOdysseyInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList);
    void OnBlockChanged(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockCommited(const TArray<::ULIS::FRectI>& iRects);

private:
    //Import/Export
    friend class FOdysseyAnimationCellImageRasterExport;
    friend class FOdysseyAnimationCellImageRasterImport;

private:
    mutable TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
    mutable FCriticalSection mImageRenderingMutex;
    mutable TWeakPtr<FOdysseyMediaRaster> mMediaRaster;
#endif
};
