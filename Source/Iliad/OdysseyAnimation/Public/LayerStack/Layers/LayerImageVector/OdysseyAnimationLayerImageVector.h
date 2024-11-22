// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "Image/OdysseyBlendingMode.h"

#include <ULIS>

#include "OdysseyVector.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorLayer.h" // interface

#include "OdysseyAnimationLayerImageVector.generated.h"

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
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsColoredChanged, UOdysseyAnimationLayerImageVector*)
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsWireframeChanged, UOdysseyAnimationLayerImageVector*)

public:
    static FOnIsColoredChanged& OnIsColoredChanged();
    static FOnIsWireframeChanged& OnIsWireframeChanged();

public:
    // UObject overrides
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void Serialize(FArchive& Ar) override;

public:
    //UOdysseyLayer overrides
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

public:
    // vector data shared between all cells
    FOdysseyVectorSharedEnv* GetSharedEnv();
    void UpdateSharedEnv();

public:
    //FOdysseyImageRenderingAbility overrides
    virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
    virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;

public:
    // Implements Interface IOdysseyVectorAnimationLayer
    virtual IOdysseyVectorCell* GetCellByIndex( uint32 iIndex ) override;
    virtual IOdysseyVectorCell* GetFirstCell() override;
    virtual IOdysseyVectorCell* GetLastCell() override;
    virtual bool Contains( IOdysseyVectorCell* iCandidateCell )override;

protected:
    void IsColoredChanged();
    void IsWireframeChanged();
    virtual void CellsChanged(bool iIsInteractive) override;
    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive) override;

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
    FOdysseyVectorSharedEnv mSharedEnv;
};
