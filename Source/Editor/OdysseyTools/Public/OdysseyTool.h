// IDDN FR.001.250001.005.S.P.2019.000.00000
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
    //Activates the tool
    virtual void Activate();

    //Inactivates the tool
    virtual void Inactivate();

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
    //Drawing the HUD associated to the tool
    //virtual void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D());

    /* virtual void MouseMove(FViewport* iViewport, int32 iX, int32 iY);
    virtual FReply InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply);
    virtual void CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY);
    virtual ::ULIS::TArray<::ULIS::FVec2I> GenerateToolPoints() { return ::ULIS::TArray<::ULIS::FVec2I>(); }; */

public:
    //Sets the transform in which the tool is working (transform of the 2D viewport)
    virtual void SetTransform(const FTransform2D& iTransform);

public:
    // Interface
    virtual void BindShortcuts(class FBaseToolkit* iToolkit);
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName );
    virtual TSharedPtr<SWidget> GetWidget();

protected:
    // bool mIsReadyToBeApplied;
    FTransform2D mTransform;
};
