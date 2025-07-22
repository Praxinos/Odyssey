// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "HitProxies.h"
#include "Input/OdysseyPoint.h"
#include "OdysseyHUD.h"
#include "UObject/GCObject.h"

/////////////////////////////////////////////////////
// UOdysseyHUDElement
class ODYSSEYHUD_API FOdysseyHUDElement
    : public TSharedFromThis<FOdysseyHUDElement>
    , public FGCObject //Allows us to register UObjects in Garbage Collector
{
public:
    // Destructor
    virtual ~FOdysseyHUDElement();

    //Constructor
    FOdysseyHUDElement();

public:
    void Draw(const FOdysseyHUD::FDrawHUDParams& iParams);

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

    bool IsVisible() const;
    void SetIsVisible(TAttribute<bool> iIsVisible);

protected:
    virtual void DrawHUD(const FOdysseyHUD::FDrawHUDParams& iParams);

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

private:
    TArray<TSharedPtr<FOdysseyHUDElement>> mElements;

    /** If this element is captured by mouse or keyboard shortcut, then the associated viewport shouldn't do anything else than manipulating this HUD */
    bool mIsCaptured;
    TAttribute<bool> mIsVisible;
};

struct ODYSSEYHUD_API HOdysseyHUDElementHitProxy : public HHitProxy
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
