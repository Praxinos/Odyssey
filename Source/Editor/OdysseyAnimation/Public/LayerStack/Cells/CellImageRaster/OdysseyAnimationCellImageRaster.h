// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyAnimationImageRenderingAbility.h"

class UOdysseyAnimationLayerImageRaster;
class FOdysseyRasterBlock;

class ODYSSEYANIMATION_API FOdysseyAnimationCellImageRaster
    : public FOdysseyAnimationCell
{    
public:
    static TSharedPtr<FOdysseyAnimationCellImageRaster> Create(UOdysseyAnimationLayerImageRaster* iLayer, int iWidth, int iHeight, ::ULIS::eFormat iFormat);
    static TSharedPtr<FOdysseyAnimationCellImageRaster> Create(UOdysseyAnimationLayerImageRaster* iLayer, TSharedPtr<::ULIS::FBlock> iBlock);
    static const FName& StaticType();

public:
    virtual ~FOdysseyAnimationCellImageRaster();
    FOdysseyAnimationCellImageRaster(UOdysseyAnimationLayerImageRaster* iLayer);

    void Init(int iWidth, int iHeight, ::ULIS::eFormat iFormat);
    void Init(TSharedPtr<::ULIS::FBlock> iBlock);
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

public:
    virtual const FName& GetType() const override;
    virtual void Serialize(FArchive& Ar);

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;

private:
    TArray<::ULIS::FEvent> RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FULISInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList);
    void OnBlockChanged(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockCommited(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockPtrChanged();

private:
    UOdysseyAnimationLayerImageRaster* mLayer;
    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
};