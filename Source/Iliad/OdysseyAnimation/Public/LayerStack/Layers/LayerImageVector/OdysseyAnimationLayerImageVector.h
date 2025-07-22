// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyAnimationLayer.h"
#include "OdysseyBlendingMode.h"

#if WITH_EDITOR
#include "OdysseyVector.h"
#include "OdysseyVectorLayer.h" // interface
#endif

#include "OdysseyAnimationLayerImageVector.generated.h"

#if WITH_EDITOR
class IOdysseyVectorCell;
class FInbetweenerBreakdown;
#endif

#if !WITH_EDITOR
class IOdysseyVectorLayer {};
#endif

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayerImageVector
    : public UOdysseyAnimationLayer
    , public IOdysseyVectorLayer
{
    GENERATED_BODY()

public:
    UOdysseyAnimationLayerImageVector();
    //IOdysseyRenderingAbility overrides
    virtual void PostInitProperties() override;

#if WITH_EDITOR
public:
    /**
     * @brief Delegate called when something changed the result of RenderImage()
     *
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsColoredChanged, UOdysseyAnimationLayerImageVector*)
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsWireframeChanged, UOdysseyAnimationLayerImageVector*)

public:
    static FOnIsColoredChanged& OnIsColoredChanged();
    static FOnIsWireframeChanged& OnIsWireframeChanged();

    ~UOdysseyAnimationLayerImageVector();

public:
    // UObject overrides
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void PreEditChange( FProperty* PropertyAboutToChange ) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

public:
    //UOdysseyLayer overrides
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

public:
    static uint32 GetInbetweeningRowHeight();

    // vector data shared between all cells
    TSharedPtr<FOdysseyVectorLayer> GetVectorLayer();
    void UpdateSharedEnv();

public:
    // Implements Interface IOdysseyVectorLayer
    virtual FOdysseyVectorCell* GetCellByIndex( uint32 iIndex ) override;
    virtual FOdysseyVectorCell* GetFirstCell() override;
    virtual FOdysseyVectorCell* GetLastCell() override;
    virtual bool Contains( FOdysseyVectorCell* iCandidateCell )override;
    virtual uint32 GetWidth() override;
    virtual uint32 GetHeight() override;
    virtual FOdysseyVectorCell* GetMaxCellFrom( uint32 iIndex ) override;
    virtual FOdysseyVectorCell* GetMinCellFrom( uint32 iIndex ) override;
    virtual FGuid GetPaletteSetID( UOdysseyPalette* iPalette ) override;

protected:
    void IsColoredChanged();
    void IsWireframeChanged();
    virtual void CellsChanged() override;
    void MakeBreakdownTargetMap();
    void CheckBreakdownTargetMap();

public:
    virtual TArray<FName> GetRows() const override;
    virtual int GetRowHeight(FName iSubRowName) const override;
    virtual bool IsRowVisible(FName iSubRowName) const override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;

private:
    TSharedPtr<IOdysseyMedia> CreateMediaVector(int iFrameIndex);
    TSharedPtr<IOdysseyMedia> GetCellMediaVector(uint32 iFrameIndex) const;
    void AutoCreateCell(int iFrameIndex);

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetIsWireframe(bool Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetIsColored(bool Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    bool IsWireframe() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    bool IsColored() const;

private:
    friend class FOdysseyAnimationLayerImageVectorImport;
    TSharedPtr<FOdysseyVectorLayer> mVectorLayer;
    TMap<FInbetweenerBreakdown*, FOdysseyVectorCell*> mBreakdownTargetMap;
#endif


#if WITH_EDITORONLY_DATA
public:
    virtual void Serialize(FArchive& Ar) override;

protected:
    UPROPERTY(NonTransactional)
    bool bIsWireframe = false;

    UPROPERTY(NonTransactional)
    bool bIsColored = true;
#endif
};
