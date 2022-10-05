// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

/////////////////////////////////////////////////////
// IOdysseyViewportElement
class ODYSSEYWIDGETS_API IOdysseyHUDViewportElement
{
public:
    IOdysseyHUDViewportElement();
    virtual ~IOdysseyHUDViewportElement();

public:
    virtual void Invalidate() = 0;
    virtual void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) = 0;
    virtual void MouseMove(FViewport* iViewport, int32 iX, int32 iY) = 0;
    virtual FReply InputKey( FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply ) = 0;
    virtual void CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY ) = 0;
    virtual void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) = 0;

protected:
    /** The previous transform applied to the element*/
    FTransform2D mPreviousTransform;

    /** If this element is invalid, then, we'll need to redraw it*/
    bool mIsInvalid;
};
