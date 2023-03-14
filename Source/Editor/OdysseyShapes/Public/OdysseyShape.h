// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyShape.generated.h"

UENUM()
enum class EOdysseyShape : uint8
{
    kNone           UMETA(DisplayName = "None"),
    kFreehand       UMETA(DisplayName = "Freehand"),
    kRectangle      UMETA(DisplayName = "Rectangle"),
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

public:
    // Getters
    FOnPathBegin& OnPathBeginDelegate() { return mOnPathBeginDelegate; }
    FOnPathTo& OnPathToDelegate() { return mOnPathToDelegate; }
    FOnPathEnd& OnPathEndDelegate() { return mOnPathEndDelegate; }
    FOnPathAbort& OnPathAbortDelegate() { return mOnPathAbortDelegate; }
    FOnPathReset& OnPathResetDelegate() { return mOnPathResetDelegate; }

public:
    //SHAPE PROPERTIES
    //UPROPERTY()
    //TArray<UOdysseyHUDElement*> mHUDs;

protected:
    // protected Data Members

    //---

    //Internal
    FOnPathBegin                        mOnPathBeginDelegate;
    FOnPathTo                           mOnPathToDelegate;
    FOnPathEnd                          mOnPathEndDelegate;
    FOnPathAbort                        mOnPathAbortDelegate;
    FOnPathReset                        mOnPathResetDelegate;
};
