// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationCell.h"
#include "OdysseyInvalidTileMap.h"

#include "OdysseyAnimationCellImageRaster.generated.h"

class UOdysseyAnimationLayerImageRaster;
class FOdysseyRasterBlock;
class FOdysseyMediaRaster;

UCLASS(BlueprintType)
class ODYSSEYLAYERSTACKEDITOR_API UOdysseyAnimationCellImageRaster
    : public UOdysseyAnimationCell
{
    GENERATED_BODY()

public:
    UOdysseyAnimationCellImageRaster();

    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

public:
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void Serialize(FArchive& Ar) override;
    virtual void OldSerialize(FArchive& Ar) override; //DEPRECATED: Keep that for compatibility with early versions of Odyssey

public:
    //IOdysseyRenderingAbility overrides
    virtual void RenderToTexture_RenderThread(FRDGBuilder& iGraphBuilder, FRDGTextureRef iDestinationTexture, ERHIFeatureLevel::Type iFeatureLevel, FFrameNumber iFrame, const FIntRect& iSrcRect, const FIntRect& iDstRect) const override;
    virtual void BlendToTexture_RenderThread(FRDGBuilder& iGraphBuilder, FRDGTextureRef iDestinationTexture, ERHIFeatureLevel::Type iFeatureLevel, FFrameNumber iFrame, const FIntRect& iSrcRect, const FIntRect& iDstRect, EOdysseyBlendingMode iBlendMode, float iOpacity) const override;
    virtual TArray<FGuid> GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const override;
    virtual FIntRect GetDefaultRenderRect() const override;
    FCriticalSection* GetImageRenderingMutex() const;
    bool IsImageRenderingGameThreadOnly() const;

private:
    void InitFromFormat(int iWidth, int iHeight, ::ULIS::eFormat iFormat);
    void InitFromBlock(TSharedPtr<::ULIS::FBlock> iBlock);
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

    UPROPERTY(NonTransactional)
    UTexture2D* Texture = nullptr;
};
