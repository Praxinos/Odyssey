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
    virtual void MouseMove( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnKeyDown( const FOdysseyPoint& iPointInTexture, FKey iKey ) override;
    virtual bool OnKeyUp( const FOdysseyPoint& iPointInTexture, FKey iKey ) override;
    virtual void CapturedMouseMove( const FOdysseyPoint& iPointInTexture ) override;
    void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

public:
    void SetPosition(FVector2D iNewPosition);
    FVector2D GetPosition();

private:
    UOdysseyHUDElement* mParent;

    FVector2D* mReferencePoint;
    FVector2D mPreviousPosition;
    int mPreviousHandleSize;

public:
    UPROPERTY( EditAnywhere, Category="Odyssey HUD Handle" )
    int mHandleSize;
};
