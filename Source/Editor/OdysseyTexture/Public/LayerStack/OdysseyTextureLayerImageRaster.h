// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/OdysseyTextureLayer.h"
#include "Image/OdysseyBlendingMode.h"
#include "Misc/TransactionObjectEvent.h"
#include "Misc/ITransaction.h"
#include "Misc/ITransactionObjectAnnotation.h"
#include "OdysseyRasterBlock.h"

#include <ULIS>

#include "OdysseyTextureLayerImageRaster.generated.h"

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayerImageRaster
    : public UOdysseyTextureLayer
{
    GENERATED_BODY()

public:
    ~UOdysseyTextureLayerImageRaster();
    UOdysseyTextureLayerImageRaster();

public:
    //UOdysseyLayer overrides
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

public:
    // Public API
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;

public:
    // UOdysseyLayer Overrides

    /**
     * @brief Merges this layer over iLayer (modifying its content)
     * Only works with Layer class being a child of classes returned by GetMergeLayerTypes()
     * 
     */
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

protected:
    void OnBlockChanged(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockCommited(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockPtrChanged();

public:
    // UObject overrides
    virtual void PostInitProperties() override;
    virtual void PostDuplicate(bool bDuplicateForPIE) override;

public:
    //UObject overrides

    /**
     * @brief Serialize this object
     *
     * @param Ar
     */
    virtual void Serialize(FArchive& Ar) override;

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame = 0, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame = 0) const override;

private:
    TArray<::ULIS::FEvent> RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FULISInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList);

private:
    //Import/Export
    friend class FOdysseyTextureLayerImageRasterExport;
    friend class FOdysseyTextureLayerImageRasterImport;

private:
    TSharedPtr<FOdysseyRasterBlock> RasterBlock;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, NonTransactional, Category="Odyssey|Layer")
    bool IsAlphaLocked = false;
};
