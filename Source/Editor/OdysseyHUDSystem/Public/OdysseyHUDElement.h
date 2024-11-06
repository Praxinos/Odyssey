// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "HitProxies.h"
#include "Input/OdysseyPoint.h"
#include "OdysseyHUDSystem.h"
#include "UObject/GCObject.h"

/////////////////////////////////////////////////////
// UOdysseyHUDElement
class ODYSSEYHUDSYSTEM_API FOdysseyHUDElement
    : public TSharedFromThis<FOdysseyHUDElement>
    , public FGCObject //Allows us to register UObjects in Garbage Collector
{
public:
    // Destructor
    virtual ~FOdysseyHUDElement();

    //Constructor
    FOdysseyHUDElement();

public:
    virtual void DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams);

public:
    //HitProxy version
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
	virtual bool OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseEnter();
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture);
    virtual void OnMouseLeave();
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);

public:
    void AddElement( TSharedPtr<FOdysseyHUDElement> iElementToAdd );
    void RemoveElement( TSharedPtr<FOdysseyHUDElement> iElementToRemove );
    void EmptyElements();

    bool IsCaptured() const;
    void Capture(bool iCapture);

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

private:
    TArray<TSharedPtr<FOdysseyHUDElement>> mElements;

    /** If this element is captured by mouse or keyboard shortcut, then the associated viewport shouldn't do anything else than manipulating this HUD */
    bool mIsCaptured;
};

struct ODYSSEYHUDSYSTEM_API HOdysseyHUDElementHitProxy : public HHitProxy
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
