// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyAnimationImageRenderingAbility.h"
#include "ULISInvalidTileMap.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"

class UOdysseyAnimationLayerImageRaster;
class FOdysseyRasterBlock;
class FOdysseyMediaRaster;

class ODYSSEYANIMATION_API FOdysseyAnimationCellImageRaster
    : public FOdysseyAnimationCell
{    
public:
    static TSharedRef<FOdysseyAnimationCellImageRaster> Create(UOdysseyAnimationLayerImageRaster* iLayer, int iLength, int iWidth, int iHeight, ::ULIS::eFormat iFormat);
    static TSharedRef<FOdysseyAnimationCellImageRaster> Create(UOdysseyAnimationLayerImageRaster* iLayer, int iLength, TSharedPtr<::ULIS::FBlock> iBlock);
    static const FName& StaticType();

public:
    virtual ~FOdysseyAnimationCellImageRaster();
    FOdysseyAnimationCellImageRaster(UOdysseyAnimationLayerImageRaster* iLayer, int iLength);

    void Init(int iWidth, int iHeight, ::ULIS::eFormat iFormat);
    void Init(TSharedPtr<::ULIS::FBlock> iBlock);
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;
    virtual TSharedPtr<FOdysseyAnimationCell> CreateCellFromFrame(uint32 iFrameIndex) const override;
    virtual UOdysseyAnimationLayerImageRaster* GetLayer() const override;

public:
    virtual TSharedPtr<FOdysseyAnimationCell> Clone(UOdysseyAnimationLayer* iLayer, int iLength) const override;
    virtual const FName& GetType() const override;
    virtual void Serialize(FArchive& Ar);
    virtual void PostDuplicate() override;

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;
    FCriticalSection* GetImageRenderingMutex() const;
    bool IsImageRenderingGameThreadOnly() const;

private:
    TArray<::ULIS::FEvent> RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FULISInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList);
    void OnBlockChanged(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockCommited(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockPtrChanged();

private:
    //Import/Export
    friend class FOdysseyAnimationCellImageRasterExport;
    friend class FOdysseyAnimationCellImageRasterImport;

private:
    UOdysseyAnimationLayerImageRaster* mLayer;
    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
    mutable FCriticalSection mImageRenderingMutex;
    mutable TWeakPtr<FOdysseyMediaRaster> mMediaRaster;
};