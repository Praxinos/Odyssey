// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyToolSet.generated.h"

UCLASS()
class ODYSSEYTOOLS_API UOdysseyToolSet : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Tools")
    void AddTool(UOdysseyTool* Tool);

    UFUNCTION(BlueprintCallable, Category="Tools")
    void RemoveTool(UOdysseyTool* Tool);

    //UFUNCTION(BlueprintPure, Category="Tools")
    //const TSet<UOdysseyTool*> GetTools() const;
    
    UFUNCTION(BlueprintCallable, Category="Tools")
    void SelectTool(UOdysseyTool* Tool);

    UFUNCTION(BlueprintPure, Category="Tools")
    UOdysseyTool* GetSelectedTool() const;

protected:
    void SelectedToolChanged();
    void ToolsChanged();

    virtual void PropertyChanged(const FName& iPropertyName);

public:
    // UObject overrides
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;

private:
    UPROPERTY()
    TObjectPtr<UOdysseyTool> SelectedTool;

    UPROPERTY()
    TSet<TObjectPtr<UOdysseyTool>> Tools;
};
