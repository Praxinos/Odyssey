// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Input/OdysseyPoint.h"
#include <ULIS>
#include "ULISLoaderModule.h"
#include "OdysseyShape.generated.h"

class FOdysseyHUDElement;

/* UENUM()
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
}; */

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
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractive, const TArray<FOdysseyPoint>& /*iPoints*/);
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCommit, const TArray<FOdysseyPoint>& /*iPoints*/, bool /*iReset*/);

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

    virtual void Abort();

    // Tick
    virtual void Tick(float iDeltaTime);
    
    // Applies the shapes specific overrides
    virtual void ApplyOverrides(const TMap<TObjectPtr<UClass>, TObjectPtr<UObject>>& iOverrides);

    void SetHUD( TSharedPtr<FOdysseyHUDElement> iHUD );
    bool IsProgressive() const;

public:
    // Getters
    FOnInteractive& OnInteractive() { return mOnInteractive; }
    FOnCommit& OnCommit() { return mOnCommit; }
    FSimpleMulticastDelegate& OnAbort() { return mOnAbort; }

public:
    //Property to hide the step property depending on how the shape is used. If used in a primitive tool, step is irrelevant and is hidden. Else, it is visible and editable
    //The EditCondition property here should always be false, else, IsPrimitive will always be visible as a checkbox next to Step for some reason. So... Yeah.
    //UPROPERTY(EditAnywhere, Category = "Shape", meta = (EditCondition = "1==0", EditConditionHides))
    //bool IsPrimitive = false;

    //UPROPERTY(EditInstanceOnly, Category = "Interpolation", meta = (ClampMin = "1", UIMin = "1", LinearDeltaSensitivity = "15", Delta = "1", Multiple = "1", DisplayPriority="0"), meta = (EditCondition = "!IsPrimitive", EditConditionHides))
    //float   Step = 1.0;

    //UPROPERTY(EditAnywhere, Category = "Shape")
    //bool Uniform = false;

protected:
    TArray<FOdysseyPoint> GeneratePointsFromFunction(TFunction<FVector2D(float)> iFunction) const;

protected:
    FOnInteractive mOnInteractive;
    FOnCommit mOnCommit;
    FSimpleMulticastDelegate mOnAbort;

protected:
    //Borrowed HUD from the tool
    TSharedPtr<FOdysseyHUDElement> mHUD;
    bool mIsProgressive = false;
};
