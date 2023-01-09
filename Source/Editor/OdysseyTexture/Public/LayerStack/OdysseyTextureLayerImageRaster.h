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
    virtual void PostDuplicate(bool bDuplicateForPIE) override;
    virtual void PostLoad() override;

protected:
    virtual void ApplyPerformanceMode(eOdysseyPerformanceMode iPerformanceMode) override;

private:
    UPROPERTY()
    TObjectPtr<UOdysseyRasterBlock> RasterBlock;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Texture | LayerStack")
    bool IsAlphaLocked = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture | LayerStack")
	EOdysseyBlendingMode BlendMode = EOdysseyBlendingMode::kNormal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture | LayerStack")
    float Opacity = 1.0f;
};
