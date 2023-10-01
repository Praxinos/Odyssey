// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"

#include "OdysseyLineShape.generated.h"

class FOdysseyHUDLine;
class FOdysseyHUDHandle;

UCLASS(meta=(DisplayName="Line Shape"))
class ODYSSEYSHAPES_API UOdysseyLineShape : public UOdysseyShape
{
    GENERATED_UCLASS_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathBegin, const FOdysseyPoint&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathTo, const TArray<FOdysseyPoint>&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathEnd, const FOdysseyPoint&);
    DECLARE_MULTICAST_DELEGATE(FOnReset);

    DECLARE_DELEGATE_RetVal_OneParam(float, FAdaptStep, float);

public:
    // Destructor
    virtual ~UOdysseyLineShape();

public:
    //Mouse events
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnKeyDown(const FKey& iKey);
    virtual bool OnKeyUp(const FKey& iKey);

public:
    FOnPathBegin& OnPathBeginDelegate() { return mOnPathBeginDelegate; }
    FOnPathTo& OnPathToDelegate() { return mOnPathToDelegate; }
    FOnPathEnd& OnPathEndDelegate() { return mOnPathEndDelegate; }
    FOnReset& OnResetDelegate() { return mOnResetDelegate; }

    FAdaptStep& AdaptStepDelegate() { return mAdaptStepDelegate; }

private:
    // Internal - Stroke Construction

    //Begins a stroke at iPoint
    //Some value are computed from the last call to MoveTo(), like direction for example
    bool BeginStroke(const FOdysseyPoint& iPoint);

    //Draws a Stroke from the last position to iPoint
    bool StrokeTo(const FOdysseyPoint& iPoint);

    //Ends the stroke
    bool EndStroke();

    //Aborts the stroke
    bool AbortStroke();

protected:
    // protected Data Members

    //---

    //Internal
    TArray< FOdysseyPoint >             mRawStroke; //Raw Stroke (basically mouse positions)

    bool                                mHasStrokeBegun;

    FOnPathBegin                        mOnPathBeginDelegate;
    FOnPathTo                           mOnPathToDelegate;
    FOnPathEnd                          mOnPathEndDelegate;
    FOnReset                            mOnResetDelegate;

    FAdaptStep                          mAdaptStepDelegate;

private:
    FOdysseyHUDLine* mLine;
    TArray<FOdysseyHUDHandle*> mHandles;
};
