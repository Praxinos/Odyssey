// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

#include "OdysseyHUDHandle.generated.h"

/////////////////////////////////////////////////////
// UOdysseyHUDHandle
UCLASS()
class ODYSSEYWIDGETS_API UOdysseyHUDHandle : public UOdysseyHUDElement
{
    GENERATED_BODY()

public:
    void Init(FName iName, UOdysseyHUDElement* iParent, FVector2D* iReferencePoint, FOdysseyPaintEngineHUD* iPaintEngineHUD, FTransform2D iTransform = FTransform2D());

//UObject overrides
public:
    virtual void PostEditChangeProperty(FPropertyChangedEvent& iPropertyChangedEvent) override;
    virtual void PreEditChange(FProperty* iPropertyAboutToChange) override;

//UOdysseyHUDElement overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;
    void Draw() override;
    virtual void MouseMove(FViewport* iViewport, int32 iX, int32 iY) override;
    virtual FReply InputKey( FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply ) override;
    virtual void CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY ) override;
    void Erase() override;

private:
    UOdysseyHUDElement* mParent;

    FVector2D* mReferencePoint;

public:
    UPROPERTY( EditAnywhere )
    int mHandleSize;
};
