// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDSlateElement.h"
#include "IOdysseyHUDViewportElement.h"
#include "Widgets/Layout/SScrollBox.h"

#include "OdysseyHUDElement.generated.h"

/////////////////////////////////////////////////////
// UOdysseyHUDElement
UCLASS()
class ODYSSEYWIDGETS_API UOdysseyHUDElement : public UOdysseyHUDSlateElement,
                                              public IOdysseyHUDViewportElement
{
    DECLARE_DELEGATE_RetVal(FReply, FOnApplyHUDAction);
    GENERATED_BODY()

public:     
    void Init(FName iName, FOdysseyPaintEngineHUD* iPaintEngineHUD, FTransform2D const * iTransform = nullptr);

//UOdysseyHUDSlateElement overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;

//IOdysseyHUDViewportElement overrides
public:
    virtual void Draw() override;
    virtual void MouseMove(FViewport* iViewport, int32 iX, int32 iY) override;
    virtual FReply InputKey( FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply ) override;
    virtual void CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY ) override;
    virtual void Erase();

    //UObject overrides
public:
    virtual void PostEditChangeProperty(FPropertyChangedEvent& iPropertyChangedEvent) override;
    virtual void PreEditChange(FProperty* iPropertyAboutToChange) override;

public:
    void AddElement( UOdysseyHUDElement* iElementToAdd );
    bool IsInvalid();
    bool IsCaptured();
    FOnApplyHUDAction& OnApplyHUDAction();

private:
    void InternalIsInvalid( bool &ioIsInvalid );
    void InternalIsCaptured( bool& ioIsCaptured );


protected:
    TMap<FString, UOdysseyHUDElement*> mElements;
    TSharedPtr<SScrollBox> mElementsWidget;

    /** If this element is invalid, then, we'll need to redraw all mElements*/
    bool mIsInvalid;

    /** If this element is captured by mouse or keyboard shortcut, then the associated viewport shouldn't do anything else than manipulating this HUD */
    bool mIsCaptured;

private:
    /** Action linked to the "Apply" button of the HUD in the interface. If not bound, the button won't show */
    FOnApplyHUDAction mOnApplyHUDAction;
};
