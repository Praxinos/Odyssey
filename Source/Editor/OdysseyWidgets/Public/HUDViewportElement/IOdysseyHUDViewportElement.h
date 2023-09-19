// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Input/OdysseyPoint.h"
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
    virtual void MouseMove( const FOdysseyPoint& iPointInTexture ) = 0;
    virtual bool OnKeyDown( const FOdysseyPoint& iPointInTexture, FKey iKey ) = 0;
    virtual bool OnKeyUp( const FOdysseyPoint& iPointInTexture, FKey iKey ) = 0;
    virtual void CapturedMouseMove( const FOdysseyPoint& iPointInTexture ) = 0;
    virtual void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) = 0;

protected:
    /** The previous transform applied to the element*/
    FTransform2D mPreviousTransform;

    /** If this element is invalid, then, we'll need to redraw it*/
    bool mIsInvalid;
};
