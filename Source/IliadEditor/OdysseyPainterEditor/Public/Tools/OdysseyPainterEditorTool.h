// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "ToolMenuOwner.h"
#include "Input/OdysseyPoint.h"
#include "OdysseyStyleSet.h"
#include "OdysseyHUDSystem.h"
#include "OdysseyPainterEditorToolContext.h"
#include "OdysseyPainterEditorToolInputProcessor.h"
#include "Widgets/SWidget.h"

#include "OdysseyPainterEditorTool.generated.h"

class FOdysseyPainterEditor;
class FOdysseyHUDElement;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorTool : public UObject
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorTool();

    //Constructor
    UOdysseyPainterEditorTool();

public:
    //Activates the tool
    UFUNCTION(BlueprintCallable, Category="Tools")
    virtual void Activate();

    //Inactivates the tool
    UFUNCTION(BlueprintPure, Category="Tools")
    virtual bool IsActivable() const;

    //Inactivates the tool
    UFUNCTION(BlueprintPure, Category="Tools")
    virtual bool IsActivated() const;

    //Inactivates the tool
    UFUNCTION(BlueprintCallable, Category="Tools")
    virtual void Inactivate();

    virtual void Load();
    virtual void Unload();

public:
    //Mouse events
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey );
    virtual bool OnMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnKeyDown(const FKey& iKey);
    virtual bool OnKeyUp(const FKey& iKey);
    // For global key press events
    virtual bool OnKeyUpGlobal(const FKeyEvent& InKeyEvent);
    virtual bool OnKeyDownGlobal(const FKeyEvent& InKeyEvent);

    // Tick
    virtual void Tick(float iDeltaTime);

    //Finishes any action currently running, does not validate the action (example, ensure any drawing in queue is done)
    virtual void Flush();

    //Validates any action that finished. (example, any drawing in queue is finished and validated so that it creates an undoable state)
    virtual void Commit();

    virtual FText GetTooltip() const;

public:
    // Interface
    virtual void BindShortcuts(class FBaseToolkit* iToolkit);
    virtual void ExtendMenu( TSharedRef<FExtender> iExtender );
    virtual void ExtendToolbar( FToolBarBuilder& iBuilder );
    virtual TSharedPtr<FOdysseyHUDElement> GetHUD();
    virtual EMouseCursor::Type GetMouseCursor() const;
    virtual void DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams);

protected:
    virtual void PropertyChanged(const FName& iPropertyName);
    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive);
    virtual void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive);

protected:
    // UObject overrides
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void PostInitProperties() override;

public:
    void SetEditor(class FOdysseyPainterEditor* iEditor);
    FOdysseyPainterEditor* GetEditor() const;
    //template<class T> T* GetEditorAs() const { return static_cast<T*>(mEditor); };

protected:
    TSharedPtr<FOdysseyPainterEditorToolInputProcessor> mInputProcessor;
    FOdysseyPainterEditor*              mEditor;
    TSharedPtr<FOdysseyHUDElement>      mHUD;

public:
    UPROPERTY(EditDefaultsOnly, Category="Tool")
    FSlateBrush Icon;
    bool mIsActivated;
    bool mIsTemporaryTool = false;
};
