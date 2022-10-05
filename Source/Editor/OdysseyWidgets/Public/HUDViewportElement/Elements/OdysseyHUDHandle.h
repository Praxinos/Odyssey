// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    void Init(FName iName, UOdysseyHUDElement* iParent, FVector2D* iReferencePoint, FTransform2D iTransform = FTransform2D());

//UOdysseyHUDElement overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    virtual void MouseMove(FViewport* iViewport, int32 iX, int32 iY) override;
    virtual FReply InputKey( FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply ) override;
    virtual void CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY ) override;
    void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

private:
    UOdysseyHUDElement* mParent;

    FVector2D* mReferencePoint;
    FVector2D mPreviousPosition;
    int mPreviousHandleSize;

public:
    UPROPERTY( EditAnywhere, Category="Odyssey HUD Handle" )
    int mHandleSize;
};
