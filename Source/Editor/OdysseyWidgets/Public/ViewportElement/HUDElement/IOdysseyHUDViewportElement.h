// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPaintEngineHUD.h"
#include "IOdysseyViewportElement.h"

/////////////////////////////////////////////////////
// IOdysseyViewportElement
class ODYSSEYWIDGETS_API IOdysseyHUDViewportElement : public IOdysseyViewportElement
{
public:
    virtual void Draw() = 0;
    virtual void MouseMove(FViewport* iViewport, int32 iX, int32 iY) = 0;
    virtual FReply InputKey( FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply ) = 0;
    virtual void CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY ) = 0;
    virtual void Erase() = 0;

//Getters/setters
public:
    void SetPaintEngineHUD( FOdysseyPaintEngineHUD* iPaintEngineHUD);
    FOdysseyPaintEngineHUD* GetPaintEngineHUD();

protected:
    FOdysseyPaintEngineHUD* mPaintEngineHUD;
};
