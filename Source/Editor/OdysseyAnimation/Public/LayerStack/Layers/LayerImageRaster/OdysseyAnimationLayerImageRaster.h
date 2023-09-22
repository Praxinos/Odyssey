// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "Image/OdysseyBlendingMode.h"

#include <ULIS>

#include "OdysseyAnimationLayerImageRaster.generated.h"

class FOdysseyAnimationLightTable;
class FOdysseyAnimationCellsContainer;

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
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnBlendModeChanged, UOdysseyAnimationLayerImageRaster*)

    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnOpacityChanged, UOdysseyAnimationLayerImageRaster*)

public:
    static FOnBlendModeChanged& OnBlendModeChanged();
    static FOnOpacityChanged& OnOpacityChanged();

public:
    ~UOdysseyAnimationLayerImageRaster();
    UOdysseyAnimationLayerImageRaster();

public:
    // UObject overrides
	virtual void PostInitProperties() override;

    /**
     * @brief Serialize this object
     *
     * @param Ar
     */
    virtual void Serialize(FArchive& Ar) override;

public:
    //UOdysseyLayer overrides
    virtual void OnCreated_Implementation() override;
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

public:
    //UOdysseyAnimationLayer overrides
    virtual FInt32Range GetFrameRange() const override;

public:
    TSharedPtr<FOdysseyAnimationLightTable> GetLightTable() const;
    TSharedRef<FOdysseyAnimationCellsContainer> GetCellsContainer() const;

public:
    // UOdysseyLayer Overrides

    /**
     * @brief Merges this layer over iLayer (modifying its content)
     * Only works with Layer class being a child of classes returned by GetMergeLayerTypes()
     * 
     */
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
    virtual ::ULIS::eBlendMode GetImageRenderingBlendMode() const override;
    virtual float GetImageRenderingOpacity() const override;

protected:
    void IsLightTableActivatedChanged();
    void OpacityChanged();
    void BlendModeChanged();
    virtual void PropertyChanged(const FName& iPropertyName) override;
    
private:
    void OnCellsChanged();
    TSharedPtr<FOdysseyAnimationCell> CreateCell( const FName& iCellType, bool iForSerialization);

private:
    TSharedPtr<IOdysseyMedia> CreateMediaRaster(int iFrameIndex);
    TSharedPtr<IOdysseyMedia> GetCellMediaRaster(uint32 iFrameIndex) const;
    void AutoCreateCell(int iFrameIndex);
    void CreateCell( const FName& iCellType);

private:
    //Import/Export
    friend class FOdysseyAnimationLayerImageRasterExport;
    friend class FOdysseyAnimationLayerImageRasterImport;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, NonTransactional, Category="Animation | LayerStack")
    bool IsAlphaLocked = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | LayerStack")
	EOdysseyBlendingMode BlendMode = EOdysseyBlendingMode::kNormal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | LayerStack")
    float Opacity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | LayerStack")
    bool bIsLightTableActivated = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | LayerStack")
    bool bAutoBreakCells = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | LayerStack")
    bool bAutoAddCells = true;

private:
    TSharedRef<FOdysseyAnimationCellsContainer> mCellsContainer;
    TSharedPtr<FOdysseyAnimationLightTable> mLightTable;
};
