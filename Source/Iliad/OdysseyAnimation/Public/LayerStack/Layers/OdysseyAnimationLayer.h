// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"
#include "OdysseyAnimationLayerStack.h"

#if WITH_EDITOR
#include "OdysseyAnimationLightTable.h"
#include "Templates/SubclassOf.h"
#endif

#include "OdysseyAnimationLayer.generated.h"

class UOdysseyAnimation;
class UOdysseyAnimationCell;

UCLASS(Abstract, BlueprintType, HideDropdown)
class ODYSSEYANIMATION_API UOdysseyAnimationLayer
    : public UOdysseyLayer
{
    GENERATED_BODY()

public:
    virtual void PostInitProperties() override;
    virtual UOdysseyAnimationLayerStack* GetLayerStack() const;

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    UOdysseyAnimation* GetAnimation() const;

#if WITH_EDITOR
public:
    FSimpleMulticastDelegate& OnLightTableChanged();

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    FOdysseyAnimationLightTable GetLighttable() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool HasLighttable() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetLighttable(FOdysseyAnimationLightTable Value);

public:
    void SetLighttableInteractive(FOdysseyAnimationLightTable Value);
    virtual TArray<FName> GetRows() const override;
    virtual int GetRowHeight(FName iSubRowName) const override;
    virtual bool IsRowVisible(FName iSubRowName) const override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;

protected:
    TArray<FGuid> GetLighttableImageRenderingComposition(int iFrameIndex) const;

protected:
    friend class FOdysseyAnimationCellsContainerImport;

    FSimpleMulticastDelegate mOnLightTableChanged;
#endif

#if WITH_EDITORONLY_DATA
protected:
    UPROPERTY(NonTransactional)
    FOdysseyAnimationLightTable Lighttable;

    UPROPERTY(NonTransactional)
    bool bHasLighttable = true;
#endif
};
