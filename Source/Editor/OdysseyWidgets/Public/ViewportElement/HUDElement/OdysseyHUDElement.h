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
    GENERATED_BODY()

public:     
    void Init(FName iName, FTransform2D iTransform = FTransform2D());

//UOdysseyHUDSlateElement overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;

//IOdysseyHUDViewportElement overrides
public:
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    virtual void MouseMove(FViewport* iViewport, int32 iX, int32 iY) override;
    virtual FReply InputKey( FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply ) override;
    virtual void CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY ) override;
    virtual void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D());

    //UObject overrides
public:
    virtual void PostEditChangeProperty(FPropertyChangedEvent& iPropertyChangedEvent) override;
    virtual void PreEditChange(FProperty* iPropertyAboutToChange) override;

public:
    void AddElement( UOdysseyHUDElement* iElementToAdd );
    bool IsInvalid();
    bool IsCaptured();

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
};
