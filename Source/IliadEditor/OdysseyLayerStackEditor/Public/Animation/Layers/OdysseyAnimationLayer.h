// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLightTable.h"
#include "OdysseyLayer.h"
#include "Templates/SubclassOf.h"

#include "OdysseyAnimationLayer.generated.h"

class UOdysseyAnimation;
class UOdysseyAnimationCell;

UCLASS(Abstract, BlueprintType, HideDropdown)
class ODYSSEYLAYERSTACKEDITOR_API UOdysseyAnimationLayer
    : public UOdysseyLayer
{
    GENERATED_BODY()

public:
    FSimpleMulticastDelegate& OnLightTableChanged();

public:
    virtual void PostInitProperties() override;
    virtual UOdysseyAnimationLayerStack* GetLayerStack() const;

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    UOdysseyAnimation* GetAnimation() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    FOdysseyAnimationLightTable GetLighttable() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool HasLighttable() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetLighttable(FOdysseyAnimationLightTable Value);

public:
#ifdef WITH_EDITOR
    void SetLighttableInteractive(FOdysseyAnimationLightTable Value);
    virtual TArray<FName> GetRows() const override;
    virtual int GetRowHeight(FName iSubRowName) const override;
    virtual bool IsRowVisible(FName iSubRowName) const override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
#endif

protected:
    TArray<FGuid> GetLighttableImageRenderingComposition(int iFrameIndex) const;

protected:
    friend class FOdysseyAnimationCellsContainerImport;

    UPROPERTY(NonTransactional)
    FOdysseyAnimationLightTable Lighttable;

    UPROPERTY(NonTransactional)
    bool bHasLighttable = true;

    FSimpleMulticastDelegate mOnLightTableChanged;
};
