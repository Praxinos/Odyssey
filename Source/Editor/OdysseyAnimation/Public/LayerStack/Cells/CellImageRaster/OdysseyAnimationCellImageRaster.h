// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "ULISInvalidTileMap.h"

#include "OdysseyAnimationCellImageRaster.generated.h"

class UOdysseyAnimationLayerImageRaster;
class FOdysseyRasterBlock;
class FOdysseyMediaRaster;

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationCellImageRaster
    : public UOdysseyAnimationCell
{
	GENERATED_BODY()

public:
	TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

public:
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
	virtual void Serialize(FArchive& Ar) override;
	virtual void OldSerialize(FArchive& Ar) override; //DEPRECATED: Keep that for compatibility with early versions of Odyssey

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;
    FCriticalSection* GetImageRenderingMutex() const;
    bool IsImageRenderingGameThreadOnly() const;

private:
    void InitFromFormat(int iWidth, int iHeight, ::ULIS::eFormat iFormat);
    void InitFromBlock(TSharedPtr<::ULIS::FBlock> iBlock);
    TArray<::ULIS::FEvent> RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FULISInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList);
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
};