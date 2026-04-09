// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Input/OdysseyPoint.h"
#include "OdysseyStyle.h"
#include "OdysseyHUDElement.h"
#include "ToolMenuOwner.h"
#include "Widgets/SWidget.h"

#include "OdysseyPainterEditor.h"

#include "OdysseyPainterEditorTool.generated.h"

class FOdysseyPainterEditor;
class FOdysseyHUDElement;
class FOdysseyPainterEditorToolInputProcessor;

UENUM()
enum class EPainterEditorToolRadiusReference
{
    Texture,
    HUD
};

UCLASS(DefaultToInstanced)
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

    UFUNCTION(BlueprintPure, Category="Tools")
    virtual bool HasRadius() const;

    UFUNCTION(BlueprintCallable, Category="Tools")
    virtual void SetRadius(float Radius);

    UFUNCTION(BlueprintPure, Category="Tools")
    virtual float GetRadius() const;

    UFUNCTION(BlueprintPure, Category="Tools")
    virtual EPainterEditorToolRadiusReference GetRadiusReference() const;

    UFUNCTION(BlueprintCallable, Category="Tools")
    void StartRadiusInteractiveModifier();

    virtual void Reset();

protected:
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

public:
    // Tick
    virtual void Tick(float iDeltaTime);

    //Finishes any action currently running, does not validate the action (example, ensure any drawing in queue is done)
    virtual void Flush();

    //Validates any action that finished. (example, any drawing in queue is finished and validated so that it creates an undoable state)
    virtual void Commit();

    virtual FText GetTooltip() const;

public:
    // Interface
    virtual void BindShortcuts(TSharedPtr<FUICommandList> iCommandList);
    virtual void ExtendMenu( TSharedRef<FExtender> iExtender );
    virtual void ExtendToolbar( UToolMenu* iToolMenu );
    virtual TSharedPtr<FOdysseyHUDElement> GetHUD();
    virtual EMouseCursor::Type GetMouseCursor() const;

    virtual bool SupportsColorType(EOdysseyPainterEditorColorType iType);

protected:
    virtual void PropertyChanged(const FName& iPropertyName);
    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive);
    virtual void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive);

protected:
    // UObject overrides
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void PostInitProperties() override;
    virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;

public:
    void SetEditor(class FOdysseyPainterEditor* iEditor);
    FOdysseyPainterEditor* GetEditor() const;
    //template<class T> T* GetEditorAs() const { return static_cast<T*>(mEditor); };

public:
    /**
     * Those functions allows the tool to handle some behaviours without having
     * the actual tool getting in the way.
     * Example : Set Radius shortcut
     */
    bool ProcessMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    bool ProcessMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey );
    bool ProcessMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    bool ProcessMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    void ProcessMouseHover(const FOdysseyPoint& iPointInTexture);
    void ProcessMouseDrag(const FOdysseyPoint& iPointInTexture);
    bool ProcessKeyDown(const FKey& iKey);
    bool ProcessKeyUp(const FKey& iKey);
    bool ProcessKeyUpGlobal(const FKeyEvent& InKeyEvent);
    bool ProcessKeyDownGlobal(const FKeyEvent& InKeyEvent);

private:
    void RIMOnMouseMove(const FOdysseyPoint& iPointInTexture);
    void RIMOnMouseDrag(const FOdysseyPoint& iPointInTexture);
    void CancelRIM();
    void EndRIM();
    FVector2D GetRIMCenter(const FVector2D& iMousePositionInTexture) const;

protected:
    TSharedPtr<FOdysseyPainterEditorToolInputProcessor> mInputProcessor;
    FOdysseyPainterEditor*              mEditor;
    TSharedPtr<FOdysseyHUDElement>      mHUD;

    TSharedPtr<FUICommandList>          mCommandList;

public:
    UPROPERTY(EditDefaultsOnly, Category = "Tool")
    FName mIconStyleSet;
    bool mIsActivated;
    bool mIsTemporaryTool = false;

private:
    FOdysseyPoint mPreviousMousePosition;

    //RIM : Radius Interactive Modifier
    bool mIsRIMActive = false;
    TSharedPtr<class FOdysseyHUDCircle> mRIMHUD;
    TSharedPtr<class FOdysseyHUDLine> mRIMHorizontalHUD;
    TSharedPtr<class FOdysseyHUDLine> mRIMVerticalHUD;
    float mRIMStartRadius;
};
