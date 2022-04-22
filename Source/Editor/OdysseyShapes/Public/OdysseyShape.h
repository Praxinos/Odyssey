// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyShape.generated.h"

UCLASS(Abstract)
class ODYSSEYSHAPES_API UOdysseyShape : public UObject
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathBegin, const FOdysseyPoint&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathTo, const TArray<FOdysseyPoint>&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathEnd, const FOdysseyPoint&);
    DECLARE_MULTICAST_DELEGATE(FOnReset);

public:
    // Destructor
    virtual ~UOdysseyShape();

public:
    //Mouse events
    virtual void OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual void OnKeyDown(const FKey& iKey);
    virtual void OnKeyUp(const FKey& iKey);

    // Tick
    virtual void Tick(float iDeltaTime);
    
    // Applies the shapes specific overrides
    virtual void ApplyOverrides(const TMap<FName, UObject*>& iOverrides);

public:
    // Getters
    FOnPathBegin& OnPathBeginDelegate() { return mOnPathBeginDelegate; }
    FOnPathTo& OnPathToDelegate() { return mOnPathToDelegate; }
    FOnPathEnd& OnPathEndDelegate() { return mOnPathEndDelegate; }
    FOnReset& OnResetDelegate() { return mOnResetDelegate; }

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
    FOnReset                            mOnResetDelegate;
};
