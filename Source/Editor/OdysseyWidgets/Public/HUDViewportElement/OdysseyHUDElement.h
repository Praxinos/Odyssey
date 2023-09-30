// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Input/OdysseyPoint.h"
#include <ULIS>

/////////////////////////////////////////////////////
// UOdysseyHUDElement
class ODYSSEYWIDGETS_API FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDElement();

    //Constructor
    FOdysseyHUDElement(FName iName, FTransform2D iTransform = FTransform2D());

public:
    virtual void Invalidate();
    virtual void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D());
    virtual void MouseMove( const FOdysseyPoint& iPointInTexture );
    virtual bool OnKeyDown( const FOdysseyPoint& iPointInTexture, FKey iKey );
    virtual bool OnKeyUp( const FOdysseyPoint& iPointInTexture, FKey iKey );
    virtual void CapturedMouseMove( const FOdysseyPoint& iPointInTexture );
    virtual void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D());

public:
    void AddElement( FOdysseyHUDElement* iElementToAdd );
    void EmptyHUDElements();
    bool IsInvalid();
    bool IsCaptured();

private:
    void InternalIsInvalid( bool &ioIsInvalid );
    void InternalIsCaptured( bool& ioIsCaptured );


protected:
    FName mName;

    TMap<FString, FOdysseyHUDElement*> mElements;
    TSharedPtr<SScrollBox> mElementsWidget;

    /** The previous transform applied to the element*/
    FTransform2D mPreviousTransform;

    /** If this element is invalid, then, we'll need to redraw it*/
    bool mIsInvalid;

    /** If this element is captured by mouse or keyboard shortcut, then the associated viewport shouldn't do anything else than manipulating this HUD */
    bool mIsCaptured;
};
