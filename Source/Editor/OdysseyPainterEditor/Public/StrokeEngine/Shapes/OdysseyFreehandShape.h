// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "StrokeEngine/OdysseyStrokeOptions.h"
#include "StrokeEngine/Smoothing/IOdysseySmoothing.h"

#include "OdysseyFreehandShape.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyFreehandShape : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathBegin, const FOdysseyPoint&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathTo, const TArray<FOdysseyPoint>&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPathEnd, const FOdysseyPoint&);
    DECLARE_MULTICAST_DELEGATE(FOnReset);

public:
    // Destructor
    virtual ~UOdysseyFreehandShape();

public:
    // Paint Engine Stroke API

    //Begins a stroke at iPoint
    //Some value are computed from the last call to MoveTo(), like direction for example
    bool Begin( const FOdysseyPoint& iPoint );

    //Draws a Stroke from the last position to iPoint
    bool To( const FOdysseyPoint& iPoint );

    //Ends the stroke
    bool End();

    //Aborts the stroke
    bool Abort();

public:
    // Tick
    void Tick(float iDeltaTime);

    // Applies the shapes specific overrides
    void ApplyOverrides(const TMap<FName, UObject*>& iOverrides);

public:
    // Getters
    //Returns the StrokeOptions
    FOdysseyStrokeOptions& GetStrokeOptions();

    FOnPathBegin& OnPathBeginDelegate() { return mOnPathBeginDelegate; }
    FOnPathTo& OnPathToDelegate() { return mOnPathToDelegate; }
    FOnPathEnd& OnPathEndDelegate() { return mOnPathEndDelegate; }
    FOnReset& OnResetDelegate() { return mOnResetDelegate; }

private:
    // Internal Stroke API
    
    //Begins the stroke, can be used by Begin() and ApplySmoothing() too
    bool InternalStrokeBegin(const FOdysseyPoint& iPoint);

    //Continues the stroke, can be used by Begin() and ApplySmoothing() too
    bool InternalStrokeTo(const FOdysseyPoint& iPoint, bool iIsStrokeEnd);

private:
    // Internal - Smoothing

    // Applies the CatchUp if needed
    void CatchUp();

    // Applies the smoothing if enabled and not in realtime
    void ApplySmoothing();

    //Reset the smoother to use the one selected in the StrokeOptions
    void ResetSmoother();

private:
    //PROPERTIES
    
    UPROPERTY(EditInstanceOnly)
    FOdysseyStrokeOptions StrokeOptions;

protected:
    // protected Data Members

    //---

    //Internal
    TArray< FOdysseyPoint >             mRawStroke;
    TSharedPtr<IOdysseySmoothing>       mSmoother;
    bool                                mIsPainting;

    FOnPathBegin                        mOnPathBeginDelegate;
    FOnPathTo                           mOnPathToDelegate;
    FOnPathEnd                          mOnPathEndDelegate;
    FOnReset                            mOnResetDelegate;
};
