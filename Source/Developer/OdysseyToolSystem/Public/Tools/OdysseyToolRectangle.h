// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "OdysseyHUDRectangle.h"
#include "IOdysseyTool.h"

class ODYSSEYTOOLSYSTEM_API FOdysseyToolRectangle : public IOdysseyTool
{
public:
    // Construction / Destruction
    virtual ~FOdysseyToolRectangle();

    FOdysseyToolRectangle( FVector2D iStartPoint );

public:
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

public:
    //Mouse events
    virtual void MouseMove(FViewport* iViewport, int32 iX, int32 iY);
    virtual FReply InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply);
    virtual void CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY);

public:
    ::ULIS::TArray<::ULIS::FVec2I> GenerateToolPoints() override;

private:
    UOdysseyHUDRectangle* mRectangle;
};
