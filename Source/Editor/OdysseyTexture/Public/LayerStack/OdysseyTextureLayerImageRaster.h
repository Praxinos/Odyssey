// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/OdysseyTextureLayer.h"
#include "Image/OdysseyBlendingMode.h"
#include "Misc/TransactionObjectEvent.h"
#include "Misc/ITransaction.h"
#include "Misc/ITransactionObjectAnnotation.h"

#include <ULIS>

#include "OdysseyTextureLayerImageRaster.generated.h"

class UOdysseyRasterBlock;

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayerImageRaster
    : public UOdysseyTextureLayer
{
    GENERATED_BODY()
    
public:
    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsAlphaLockedChanged, UOdysseyTextureLayerImageRaster*)

    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnBlendModeChanged, UOdysseyTextureLayerImageRaster*)

    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnOpacityChanged, UOdysseyTextureLayerImageRaster*)

public:
    static FOnIsAlphaLockedChanged& OnIsAlphaLockedChanged();
    static FOnBlendModeChanged& OnBlendModeChanged();
    static FOnOpacityChanged& OnOpacityChanged();

public:
    ~UOdysseyTextureLayerImageRaster();
    UOdysseyTextureLayerImageRaster();

public:
    //UOdysseyLayer overrides
    virtual void OnCreated_Implementation() override;

public:
    // Public API
    UOdysseyRasterBlock* GetRasterBlock() const;
    //const ::ULIS::FBlock* GetBlock() const;
    //void  UpdateBlock(const ::ULIS::FBlock& iSourceBlock, const TArray<::ULIS::FRectI>& iRects, const ::ULIS::FVec2I& iSourceOffset, const TArray<::ULIS::FEvent>& iWaitList, bool iTransaction = true);

    /**
     * @brief Sets a block to be used as a replacement the internal block when rendering an image
     * Useful to display the editedBlock while drawing instead of the internal block which will only change when UpdateBlock is called (when the stroke ends)
     * 
     */
    //void SetRenderBlockOverride(::ULIS::FBlock* iBlock);

public:
    //IOdysseyTextureLayerImageRenderer implementation
    
    /**
     * @brief Renders an image in the given Block
     * Takes into account the size / format of the given block
     * 
     */
    virtual TArray<::ULIS::FEvent> RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>  ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) override;

    /**
     * @brief Copies an image in the given Block
     * Takes into account the size / format of the given block
     */
    virtual TArray<::ULIS::FEvent> CopyImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>  ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) override;

public:
    // UOdysseyLayer Overrides

    /**
     * @brief Merges this layer over iLayer (modifying its content)
     * Only works with Layer class being a child of classes returned by GetMergeLayerTypes()
     * 
     */
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

protected:
    void OnPixelsChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);
    void OnBlockChanged();

    void IsAlphaLockedChanged();
    void OpacityChanged();
    void BlendModeChanged();
    virtual void PropertyChanged(const FName& iPropertyName) override;

public:
    // UObject overrides
#if WITH_EDITOR
	/** Internal struct to track currently active transactions */
    /* friend class FBlockTransactionAnnotation;
	class FBlockTransactionAnnotation : public ITransactionObjectAnnotation
	{
	public:
        virtual ~FBlockTransactionAnnotation() {};

        FBlockTransactionAnnotation();
        FBlockTransactionAnnotation(const UOdysseyTextureLayerImageRaster* iLayer, const TArray<::ULIS::FRectI>& iPrevRects, const TArray<::ULIS::FRectI>& iNextRects);

    public:
        void CopyToBlock();

	public:
		//~ ITransactionObjectAnnotation interface
		virtual void Serialize(FArchive& Ar) override;
        virtual void AddReferencedObjects(class FReferenceCollector& Collector) override {};

    private:
        UOdysseyTextureLayerImageRaster* mLayer;
        TArray<::ULIS::FBlock> mPrevBlocks;
        TArray<::ULIS::FBlock> mNextBlocks;
        TArray<::ULIS::FRectI> mPrevRects;
        TArray<::ULIS::FRectI> mNextRects;
	};

	virtual TSharedPtr<ITransactionObjectAnnotation> FactoryTransactionAnnotation(const ETransactionAnnotationCreationMode InCreationMode) const override;
	virtual void PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation) override; */
#endif

    virtual void PostDuplicate(bool bDuplicateForPIE) override;

    virtual void PostLoad() override;
    
    /**
     * @brief Handles Saving / Loading
     * 
     * @param Ar 
     */
    //virtual void Serialize(FArchive& Ar);

private:
    // PRIVATE API

    /**Called when mBlock is dirtied */
    // static void OnBlockInvalidated(const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo);
    // void OnTransactionStateChanged(const FTransactionContext& TransactionContext, ETransactionStateEventType TransactionState);

private:
    UPROPERTY()
    TObjectPtr<UOdysseyRasterBlock> RasterBlock;

    /* ::ULIS::FBlock* mBlock = nullptr;
    ::ULIS::FBlock* mRenderBlockOverride = nullptr;
    TArray<::ULIS::FRectI> mPrevDirtyRects;
    TArray<::ULIS::FRectI> mNextDirtyRects; */

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Texture | LayerStack")
    bool IsAlphaLocked = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture | LayerStack")
	EOdysseyBlendingMode BlendMode = EOdysseyBlendingMode::kNormal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture | LayerStack")
    float Opacity = 1.0f;
};
