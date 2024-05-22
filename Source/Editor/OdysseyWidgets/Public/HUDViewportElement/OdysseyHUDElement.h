// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Input/OdysseyPoint.h"
#include "OdysseyHUDSystem.h"
#include <ULIS>

class FOdysseyHUDElement;
struct ODYSSEYWIDGETS_API HOdysseyHUDElementHitProxy : public HHitProxy
{
	DECLARE_HIT_PROXY();

    TSharedPtr<FOdysseyHUDElement> mHUDElement;
	TOptional<EMouseCursor::Type> mMouseCursor;

    HOdysseyHUDElementHitProxy(TSharedPtr<FOdysseyHUDElement> iHUDElement, TOptional<EMouseCursor::Type> iMouseCursor)
        : HHitProxy(HPP_Foreground)
        , mHUDElement(iHUDElement)
		, mMouseCursor(iMouseCursor)
	{
	}

	virtual EMouseCursor::Type GetMouseCursor() override
	{
        if (!mMouseCursor.IsSet())
            return HHitProxy::GetMouseCursor();

        return mMouseCursor.GetValue();
	}

    virtual TSharedPtr<FOdysseyHUDElement> HUDElement()
    {
        return mHUDElement;
    };
};

/////////////////////////////////////////////////////
// UOdysseyHUDElement
class ODYSSEYWIDGETS_API FOdysseyHUDElement
    : public TSharedFromThis<FOdysseyHUDElement>
{
public:
    // Destructor
    virtual ~FOdysseyHUDElement();

    //Constructor
    FOdysseyHUDElement(FName iName);

public:
    //virtual void Invalidate();
    //virtual void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D());
    virtual void DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams);
    /*virtual void MouseMove( const FOdysseyPoint& iPointInTexture );
    virtual bool OnKeyDown( const FOdysseyPoint& iPointInTexture, FKey iKey );
    virtual bool OnKeyUp( const FOdysseyPoint& iPointInTexture, FKey iKey );
    virtual void CapturedMouseMove( const FOdysseyPoint& iPointInTexture );
    virtual void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()); */

public:
    //HitProxy version
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);

public:
    void AddElement( TSharedPtr<FOdysseyHUDElement> iElementToAdd );
    void RemoveElement( TSharedPtr<FOdysseyHUDElement> iElementToRemove );
    void EmptyHUDElements();
    //bool IsInvalid();
    FName GetName() const;
    bool IsCaptured() const;
    void Capture(bool iCapture);

//private:
    //void InternalIsInvalid( bool &ioIsInvalid );
    //void InternalIsCaptured( bool& ioIsCaptured );

private:
    FName mName;

    TMap<FString, TSharedPtr<FOdysseyHUDElement>> mElements;
    //TSharedPtr<SScrollBox> mElementsWidget;

    /** The previous transform applied to the element*/
    //FTransform2D mPreviousTransform;

    /** If this element is invalid, then, we'll need to redraw it*/
    //bool mIsInvalid;

    /** If this element is captured by mouse or keyboard shortcut, then the associated viewport shouldn't do anything else than manipulating this HUD */
    bool mIsCaptured;
};
