// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "ToolMenuOwner.h"
#include "Widgets/SWidget.h"

#include "Input/OdysseyPoint.h"
#include "OdysseyHUDElement.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorToolMouseCursor.h"
#include "OdysseyStyle.h"

#include "OdysseyPainterEditorTool.generated.h"

//---

class FExtender;
class UToolMenu;
class FOdysseyPainterEditor;
class FOdysseyHUDElement;
class FOdysseyPainterEditorToolInputProcessor;

//---

UENUM()
enum class EPainterEditorToolRadiusReference
{
    Texture,
    HUD
};

//---

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

    /** Refresh the tool based on its properties. Useful for toolCollection that don't call propertyChanged when loading a tool from it */
    virtual void RefreshToolFromProperties();

    /**
     * Starts an interactive operation
     *
     * Every call between BeginInteractiveMode() and EndInteractiveMode()
     * is considered as interactive, meaning the value which is set is not guaranteed
     * to the definitive value the user wants (example : we are dragging a slider)
     *
     * Also, it is recommended to not do any heavy computation and limit callback calls while in Interactive Mode
     * to avoid lag while dragging sliders for example.
     *
     * This system is NOT meant to replace the interactive system in PostEditPropertyChanged
     * PostEditPropertyChanged() is specific to modifications made in the DetailsViews (most of the time)
     *
     * This system is meant to be used in other C++ functions (usually setters)
     */
    virtual void BeginInteractiveMode();

    /**
     * End the Interactive Mode
     *
     * If you want callbacks to be called please call your setters again after this function
     * as InteractiveMode does not track function calls or modified properties.
     *
     * example: when releaseing an opacity slider call
     *   tool->EndInteractiveMode();
     *   tool->SetOpacity(tool->GetOpacity());
     */
    virtual void EndInteractiveMode();

    /**
     * Returns wether Interactive Mode is active or not
     */
    bool IsInInteractiveMode() const;

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

    virtual FText GetTooltip() const PURE_VIRTUAL( UOdysseyPainterEditorTool::GetTooltip, return FText::GetEmpty(); );

public:
    // Interface
    virtual void BindShortcuts(TSharedPtr<FUICommandList> iCommandList);
    virtual void ExtendMenu( TSharedRef<FExtender> iExtender );
    virtual void ExtendToolbar( UToolMenu* iToolMenu );
    virtual TSharedPtr<FOdysseyHUDElement> GetHUD();

    /** Get the mouse cursor of the the tool
      * - it can be a native one
      * - or a custom one if native == custom
      *
      * DON'T make it virtual, it's wanted to not be able to override it !!!
      * Override GetMouseCursorOverride() if you need to change the cursor "on the fly" when GetMouseCursor() is called
      *
      * The goal of this pattern is to be able to store the real value of the cursor in mMouseCursor
      * and change it where you need in every functions of the class
      * OR
      * by overriding GetMouseCursorOverride(), it's possible to use a value "on the fly" of the cursor
      * without having to change mMouseCursor and store its previous value, restore it when needed, ...
      *
      * A typical example is to display EMouseCursor::SlashedCircle when drawing is not possible,
      * unlike to add a callback to the lock/unlock layer function,
      * just set it in the GetMouseCursorOverride() by checking its value and as it is called in each tick
      * everything will be display correctly, and as soon, the layer is unlock, in the next tick,
      * GetMouseCursorOverride() will return an empty value, and so mMouseCursor will be reused
      *
      * WARNING: with this way, the displayed cursor WON'T be necessary the same as the one stored in mMouseCursor !
      */
    /*virtual*/ FMouseCursor GetMouseCursor() const;

    virtual bool UsesRasterSelection() const;
    virtual bool SupportsColorType(EOdysseyPainterEditorColorType iType);

protected:
    /** Called inside GetMouseCursor()
      * Override this function to override mMouseCursor value
      * (see also comment of GetMouseCursor())
      */
    virtual TOptional<FMouseCursor> GetMouseCursorOverride() const;

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
    TSharedPtr<FOdysseyHUDElement>      mRootHUD;
    TSharedPtr<FOdysseyHUDElement>      mHUD;

    TSharedPtr<FUICommandList>          mCommandList;

    FMouseCursor                        mMouseCursor;

public:
    UPROPERTY(EditDefaultsOnly, Category = "Tool")
    FName mIconStyleSet;
    bool mIsActivated;
    bool mIsTemporaryTool = false;

private:
    FOdysseyPoint mPreviousMousePosition;
    bool mIsInInteractiveMode = false;

    //RIM : Radius Interactive Modifier
    bool mIsRIMActive = false;
    TSharedPtr<class FOdysseyHUDCircle> mRIMHUD;
    TSharedPtr<class FOdysseyHUDLine> mRIMHorizontalHUD;
    TSharedPtr<class FOdysseyHUDLine> mRIMVerticalHUD;
    float mRIMStartRadius;
    FOdysseyPoint mRIMStartMousePosition;

private:
    // Telemetry
    FDateTime SessionStartTime;
};
