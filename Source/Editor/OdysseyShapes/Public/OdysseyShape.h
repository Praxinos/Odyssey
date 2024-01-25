// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Input/OdysseyPoint.h"
#include <ULIS>
#include "ULISLoaderModule.h"
#include "OdysseyShape.generated.h"

class FOdysseyHUDElement;

UENUM()
enum class EOdysseyDrawingPrecision : uint8
{
    kRaw  UMETA(DisplayName = "Raw"),
    kAA   UMETA(DisplayName = "AntiAliased"),
    kSP   UMETA(DisplayName = "SubPixel")
};

struct FOdysseyShapeDrawOptions
{
    //float size;
    ::ULIS::FColor mColor = ::ULIS::FColor::FromRGBA8( 0, 0, 0, 255);
    bool mFilled = false;
    EOdysseyDrawingPrecision mPrecision = EOdysseyDrawingPrecision::kRaw;
};

UENUM()
enum class EOdysseyShape : uint8
{
    kFreehand       UMETA(DisplayName = "Freehand"),
    kLine           UMETA(DisplayName = "Line"),
    kRectangle      UMETA(DisplayName = "Rectangle"),
    kPolygon        UMETA(DisplayName = "Polygon"),
    kEllipse        UMETA(DisplayName = "Ellipse"),
    kBezier         UMETA(DisplayName = "Bezier"),
};

UCLASS(Abstract)
class ODYSSEYSHAPES_API UOdysseyShape : public UObject
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathBegin, const FOdysseyPoint&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathTo, const TArray<FOdysseyPoint>&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathEnd, const FOdysseyPoint&);
    DECLARE_MULTICAST_DELEGATE(FOnPathAbort);
    DECLARE_MULTICAST_DELEGATE(FOnPathReset);

public:
    // Destructor
    virtual ~UOdysseyShape();

public:
    //Mouse events
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnKeyDown(const FKey& iKey);
    virtual bool OnKeyUp(const FKey& iKey);

    // Tick
    virtual void Tick(float iDeltaTime);
    
    // Applies the shapes specific overrides
    virtual void ApplyOverrides(const TMap<TObjectPtr<UClass>, TObjectPtr<UObject>>& iOverrides);

    void SetHUD( FOdysseyHUDElement* iHUD );
    virtual float GetStep() const;
    virtual bool AbortShape();

    virtual void Draw( ::ULIS::FBlock* iBlock, FOdysseyShapeDrawOptions& iOptions ); //Draw this shape onto a block

public:
    // Getters
    FOnPathBegin& OnPathBeginDelegate() { return mOnPathBeginDelegate; }
    FOnPathTo& OnPathToDelegate() { return mOnPathToDelegate; }
    FOnPathEnd& OnPathEndDelegate() { return mOnPathEndDelegate; }
    FOnPathAbort& OnPathAbortDelegate() { return mOnPathAbortDelegate; }
    FOnPathReset& OnPathResetDelegate() { return mOnPathResetDelegate; }


public:
    UPROPERTY(EditInstanceOnly, Category = "Interpolation", meta = (ClampMin = "1", UIMin = "1", LinearDeltaSensitivity = "15", Delta = "1", Multiple = "1", DisplayPriority="0"))
    float   Step = 1.0;

    UPROPERTY(EditAnywhere, Category = "Shape")
    bool Uniform = false;

protected:
    // protected Data Members

    //---

    //Internal
    FOnPathBegin                        mOnPathBeginDelegate;
    FOnPathTo                           mOnPathToDelegate;
    FOnPathEnd                          mOnPathEndDelegate;
    FOnPathAbort                        mOnPathAbortDelegate;
    FOnPathReset                        mOnPathResetDelegate;

protected:
    //Borrowed HUD from the tool
    FOdysseyHUDElement* mHUD;
};
