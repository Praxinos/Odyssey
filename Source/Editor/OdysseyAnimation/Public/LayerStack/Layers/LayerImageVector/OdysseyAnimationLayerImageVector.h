// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "Image/OdysseyBlendingMode.h"

#include <ULIS>

#include "OdysseyVector.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorLayer.h" // interface

#include "OdysseyAnimationLayerImageVector.generated.h"

class FOdysseyAnimationCell;

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayerImageVector
    : public UOdysseyAnimationLayer, public IOdysseyVectorLayer
{
    GENERATED_BODY()

public:
    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnBlendModeChanged, UOdysseyAnimationLayerImageVector*)

    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnOpacityChanged, UOdysseyAnimationLayerImageVector*)

    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsColoredChanged, UOdysseyAnimationLayerImageVector*)
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsWireframeChanged, UOdysseyAnimationLayerImageVector*)


public:
    static FOnBlendModeChanged& OnBlendModeChanged();
    static FOnOpacityChanged& OnOpacityChanged();
    static FOnIsColoredChanged& OnIsColoredChanged();
    static FOnIsColoredChanged& OnIsWireframeChanged();

public:
    ~UOdysseyAnimationLayerImageVector();
    UOdysseyAnimationLayerImageVector();

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
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

public:
    //UOdysseyAnimationLayer overrides
    virtual FInt32Range GetFrameRange() const override;

public:
    // vector data shared between all cells
    FOdysseyVectorSharedEnv* GetSharedEnv();

public:
    virtual TSharedPtr<FOdysseyAnimationLightTable> GetLightTable() const override;
    virtual bool GetIsLightTableActivated() const override;
    virtual TSharedPtr<FOdysseyAnimationCellsContainer> GetCellsContainer() const override;

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
    virtual ::ULIS::eBlendMode GetImageRenderingBlendMode() const override;
    virtual float GetImageRenderingOpacity() const override;

public:
    // Implements Interface IOdysseyVectorAnimationLayer
    virtual IOdysseyVectorCell* GetCellByIndex( uint32 iIndex ) override;
    virtual IOdysseyVectorCell* GetFirstCell() override;
    virtual IOdysseyVectorCell* GetLastCell() override;

protected:
    void IsColoredChanged();
    void IsWireframeChanged();
    void IsLightTableActivatedChanged();
    void OpacityChanged();
    void BlendModeChanged();
    virtual void PropertyChanged(const FName& iPropertyName) override;
    
private:
    void OnCellsChanged();
    TSharedPtr<FOdysseyAnimationCell> CreateCell( const FName& iCellType, bool iForSerialization);

private:
    TSharedPtr<IOdysseyMedia> CreateMediaVector(int iFrameIndex);
    TSharedPtr<IOdysseyMedia> GetCellMediaVector(uint32 iFrameIndex) const;
    void AutoCreateCell(int iFrameIndex);
    void CreateCell( const FName& iCellType);

private:
    //Import/Export
    friend class FOdysseyAnimationLayerImageVectorExport;
    friend class FOdysseyAnimationLayerImageVectorImport;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation | LayerStack")
    bool IsWireframe = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation | LayerStack")
    bool IsColored = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, NonTransactional, Category="Animation | LayerStack")
    bool IsAlphaLocked = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | LayerStack")
	EOdysseyBlendingMode BlendMode = EOdysseyBlendingMode::kNormal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | LayerStack")
    float Opacity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | LayerStack")
    bool bIsLightTableActivated = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | LayerStack")
    bool bAutoAddCells = true;

private:
    // mSharedEnv MUST be before mCellsContainer because of the destruction order
    FOdysseyVectorSharedEnv mSharedEnv;
    TSharedRef<FOdysseyAnimationCellsContainer> mCellsContainer;
    TSharedPtr<FOdysseyAnimationLightTable> mLightTable;
};
