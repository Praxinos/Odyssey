// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationLayer.h"
#include "Image/OdysseyBlendingMode.h"

#include <ULIS>

#include "OdysseyVector.h"
#include "OdysseyVectorLayer.h" // interface

#include "OdysseyAnimationLayerImageVector.generated.h"

class IOdysseyVectorCell;
class FInbetweenerBreakdown;

UCLASS(BlueprintType)
class ODYSSEYLAYERSTACKEDITOR_API UOdysseyAnimationLayerImageVector
    : public UOdysseyAnimationLayer, public IOdysseyVectorLayer
{
    GENERATED_BODY()

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
    UOdysseyAnimationLayerImageVector();

public:
    // UObject overrides
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void Serialize(FArchive& Ar) override;
    virtual void PreEditChange( FProperty* PropertyAboutToChange ) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

public:
    //UOdysseyLayer overrides
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

public:
    static uint32 GetInbetweeningRowHeight();

    // vector data shared between all cells
    FOdysseyVectorLayer* GetVectorLayer();
    void UpdateSharedEnv();

public:
    //IOdysseyRenderingAbility overrides
    virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(EOdysseyRenderingType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
    virtual TArray<FGuid> GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const override;

public:
    // Implements Interface IOdysseyVectorLayer
    virtual FOdysseyVectorCell* GetCellByIndex( uint32 iIndex ) override;
    virtual FOdysseyVectorCell* GetFirstCell() override;
    virtual FOdysseyVectorCell* GetLastCell() override;
    virtual bool Contains( FOdysseyVectorCell* iCandidateCell )override;
    virtual uint32 GetWidth() override;
    virtual uint32 GetHeight() override;

protected:
    void IsColoredChanged();
    void IsWireframeChanged();
    virtual void CellsChanged(bool iIsInteractive) override;
    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive) override;
    virtual void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive) override;
    void MakeBreakdownTargetMap();
    void CheckBreakdownTargetMap();

public:

#ifdef WITH_EDITOR

virtual TArray<FName> GetRows() const override;
virtual int GetRowHeight(FName iSubRowName) const override;
virtual bool IsRowVisible(FName iSubRowName) const override;

#endif //WITH_EDITOR

private:
    TSharedPtr<IOdysseyMedia> CreateMediaVector(int iFrameIndex);
    TSharedPtr<IOdysseyMedia> GetCellMediaVector(uint32 iFrameIndex) const;
    void AutoCreateCell(int iFrameIndex);

private:
    UFUNCTION(BlueprintSetter)
    void IsWireframeBlueprintSetter(bool Value);

    UFUNCTION(BlueprintSetter)
    void IsColoredBlueprintSetter(bool Value);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Layer", BlueprintSetter=IsWireframeBlueprintSetter, NonTransactional)
    bool IsWireframe = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Layer", BlueprintSetter=IsColoredBlueprintSetter, NonTransactional)
    bool IsColored = true;

private:
    // mSharedEnv MUST be before mCellsContainer because of the destruction order
    FOdysseyVectorLayer mVectorLayer;
    TMap<FInbetweenerBreakdown*, FOdysseyVectorCell*> mBreakdownTargetMap;
};
