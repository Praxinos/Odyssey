// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Input/OdysseyPoint.h"
#include <ULIS>
#include "ULISLoaderModule.h"
#include "OdysseyShape.generated.h"

class FOdysseyHUDElement;

UENUM()
enum class EOdysseyShapeType : uint8
{
    kNone,
    kFreehand       UMETA(DisplayName = "Freehand"),
    kLine           UMETA(DisplayName = "Line"),
    kRectangle      UMETA(DisplayName = "Rectangle"),
    kPolygon        UMETA(DisplayName = "Polygon"),
    kEllipse        UMETA(DisplayName = "Ellipse"),
    kBezier         UMETA(DisplayName = "Bezier"),
};

USTRUCT()
struct ODYSSEYPAINTEREDITOR_API FOdysseyShapes
{
    GENERATED_BODY()

public:
    static void RegisterDetailCustomization();
    static void UnregisterDetailCustomization();

public:
    FOdysseyShapes();

public:
    UOdysseyShape* GetActiveShape() const;
    const TMap<EOdysseyShapeType, UOdysseyShape*>& GetShapes() const;
    EOdysseyShapeType GetActiveShapeType() const;
    void SetActiveShapeType(EOdysseyShapeType iType);

    TArray<EOdysseyShapeType> GetActiveShapeTypes() const;
    void AddShapeType(EOdysseyShapeType iType, UOdysseyShape* iShape);
    void RemoveShapeType(EOdysseyShapeType iType);

public:
    UPROPERTY(EditAnywhere, Category="Shapes")
    EOdysseyShapeType ActiveShapeType;

private:
    UPROPERTY()
    TMap<EOdysseyShapeType, UOdysseyShape*> Shapes;
};

UCLASS(Abstract)
class ODYSSEYPAINTEREDITOR_API UOdysseyShape : public UObject
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
    FSimpleMulticastDelegate& OnBegin() { return mOnBegin; }
    FOnInteractive& OnInteractive() { return mOnInteractive; }
    FOnCommit& OnCommit() { return mOnCommit; }
    FSimpleMulticastDelegate& OnAbort() { return mOnAbort; }

protected:
    TArray<FOdysseyPoint> GeneratePointsFromFunction(TFunction<FVector2D(float)> iFunction) const;

protected:
    FSimpleMulticastDelegate mOnBegin;
    FOnInteractive mOnInteractive;
    FOnCommit mOnCommit;
    FSimpleMulticastDelegate mOnAbort;

protected:
    //Borrowed HUD from the tool
    TSharedPtr<FOdysseyHUDElement> mHUD;
    bool mIsProgressive = false;
};
