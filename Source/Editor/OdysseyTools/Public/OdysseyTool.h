// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "ToolMenuOwner.h"
#include "Input/OdysseyPoint.h"
#include "Widgets/SWidget.h"

#include <ULIS>
#include "ULISLoaderModule.h"

#include "OdysseyTool.generated.h"

UCLASS(Abstract)
class ODYSSEYTOOLS_API UOdysseyTool : public UObject
{
    GENERATED_BODY()

public:
    UOdysseyTool();

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsActivableChanged, UOdysseyTool*);

public:
    //Delegates

    /**
     * @brief Returns the OnIsActivableChanged delegate
     */
    static FOnIsActivableChanged& OnIsActivableChanged();

public:
    //Activates the tool
    UFUNCTION(BlueprintCallable, Category="Tools")
    virtual void Activate();

    //Inactivates the tool
    UFUNCTION(BlueprintPure, Category="Tools")
    virtual bool IsActivable() const;

    //Inactivates the tool
    UFUNCTION(BlueprintCallable, Category="Tools")
    virtual void Inactivate();

public:
    //Mouse events
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnKeyDown(const FKey& iKey);
    virtual bool OnKeyUp(const FKey& iKey);

    // Tick
    virtual void Tick(float iDeltaTime);

    //Finishes any action currently running, does not validate the action (example, ensure any drawing in queue is done)
    virtual void Flush();

    //Validates any action that finished. (example, any drawing in queue is finished and validated so that it creates an undoable state)
    virtual void Commit();

public:
    // Interface
    virtual void BindShortcuts(class FBaseToolkit* iToolkit);
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName );

protected:
    virtual void PropertyChanged(const FName& iPropertyName);

protected:
    // UObject overrides
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;

public:
    UPROPERTY(EditDefaultsOnly, Category="Tool")
    FSlateBrush Icon;
};
