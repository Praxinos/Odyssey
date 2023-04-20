// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "Image/OdysseyBlendingMode.h"

#include <ULIS>

#include "OdysseyAnimationLayerImageRaster.generated.h"

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayerImageRaster
    : public UOdysseyAnimationLayer
{
    GENERATED_BODY()

public:
    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsAlphaLockedChanged, UOdysseyAnimationLayerImageRaster*)

    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnBlendModeChanged, UOdysseyAnimationLayerImageRaster*)

    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnOpacityChanged, UOdysseyAnimationLayerImageRaster*)

    /**
     * @brief Delegate called when adding / removing cells
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnCellsChanged, UOdysseyAnimationLayerImageRaster*)

public:
    static FOnIsAlphaLockedChanged& OnIsAlphaLockedChanged();
    static FOnBlendModeChanged& OnBlendModeChanged();
    static FOnOpacityChanged& OnOpacityChanged();
    static FOnCellsChanged& OnCellsChanged();

public:
    ~UOdysseyAnimationLayerImageRaster();
    UOdysseyAnimationLayerImageRaster();

public:
    //UOdysseyLayer overrides
    virtual void OnCreated_Implementation() override;

public:
    //UOdysseyAnimationLayer overrides
    virtual FInt32Range GetFrameRange() const override;

public:
    //Public API - Cells
    UFUNCTION(BlueprintPure)
    int GetCellsCount() const;

    UFUNCTION(BlueprintPure)
    bool GetCellIndexAtFrame(int iFrameIndex, int& oCellIndex, int& oCellFrameIndex) const;

    UFUNCTION(BlueprintPure)
    bool GetCellFrameRange(int iIndex, FInt32Range& oFrameRange) const;
    
    UFUNCTION(BlueprintPure)
    bool GetCellLength(int iIndex, int& oLength) const;
    
    UFUNCTION(BlueprintPure)
    bool GetCellType(int iIndex, FName& oType) const;

    /**
     * @brief Inserts a Cell of given type at given index
     * 
     * @param iIndex 
     * @return uint32 
     */
    UFUNCTION(BlueprintCallable)
    void AddImageCell();

    UFUNCTION(BlueprintCallable)
    void RemoveCell(int iIndex);

    UFUNCTION(BlueprintCallable)
    void SetCellLength(int iIndex, int iLength);

public:
    TArray<TSharedPtr<FOdysseyAnimationCell>>& GetCells();
    TSharedPtr<FOdysseyAnimationCell> GetCell(int iIndex) const;
    TSharedPtr<FOdysseyAnimationCell> GetCellAtFrame(int iFrameIndex, int& iCelFrameIndex) const;

public:
    // UOdysseyLayer Overrides

    /**
     * @brief Merges this layer over iLayer (modifying its content)
     * Only works with Layer class being a child of classes returned by GetMergeLayerTypes()
     * 
     */
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

protected:
    void IsAlphaLockedChanged();
    void OpacityChanged();
    void BlendModeChanged();
    virtual void PropertyChanged(const FName& iPropertyName) override;

public:
    // UObject overrides
	virtual void PostInitProperties() override;
    virtual void PostDuplicate(bool bDuplicateForPIE) override;
    virtual void PostLoad() override;

    /**
     * @brief Serialize this object
     *
     * @param Ar
     */
    virtual void Serialize(FArchive& Ar) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation | LayerStack")
    bool IsAlphaLocked = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | LayerStack")
	EOdysseyBlendingMode BlendMode = EOdysseyBlendingMode::kNormal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | LayerStack")
    float Opacity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | LayerStack")
    int Offset = 0;

private:
    TArray<TSharedPtr<FOdysseyAnimationCell>> mCells;
};
