// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "HitProxies.h"
#include "Input/OdysseyPoint.h"
#include "OdysseyHUDElement.h"
#include "UObject/GCObject.h"


/////////////////////////////////////////////////////
// UOdysseyHUDElement
class ODYSSEYHUD_API FOdysseyHUDElement
    : public TSharedFromThis<FOdysseyHUDElement>
    , public FGCObject //Allows us to register UObjects in Garbage Collector
{

public:
    struct FHUDCustomization
    {
        // For dotted lines
        int32 mSpeed = 0; // Animate offset: mSpeed = world units per second
        bool mIsActive = false; // To toggle the customization on and off
        TArray<FLinearColor> mColors;
        float mSegmentLength = INFINITY;
        float mGapLength = 0.f;
    };

    struct FDrawHUDParams
    {
        DECLARE_DELEGATE_RetVal_OneParam(FVector2D, FTextureToHUD, const FVector2D&)

        FCanvas* mCanvas;
        FTextureToHUD mTextureToHUD;
        int32 mTextureWidth;
        int32 mTextureHeight;

        const FHUDCustomization* mCustomization = nullptr; // non-owning
    };

public:
    // Destructor
    virtual ~FOdysseyHUDElement();

    //Constructor
    FOdysseyHUDElement();

public:
    void Draw(const FOdysseyHUDElement::FDrawHUDParams& iParams);

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

    void SetCustomization(const FHUDCustomization& iCustomization);
    const FHUDCustomization& GetCustomization() const;
    void ActivateCustomization();
    void InactivateCustomization();

    bool IsCaptured() const;
    void Capture(bool iCapture);

    bool IsVisible() const;
    void SetIsVisible(TAttribute<bool> iIsVisible);

protected:
    virtual void DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams);
    void DrawCustomizedLine(FCanvas* iCanvas, const FVector2D& iStart, const FVector2D& iEnd, float iTimeOffset, float iPatternLength, float& ioCumulLength, int& ioColorIndex, const TArray<FLinearColor>& iColors, const FHUDCustomization& iCustomization, FBatchedElements* iBatchedElements) const;

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

protected:
    FHUDCustomization mCustomization;

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
