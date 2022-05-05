// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "FreehandShape/Interpolation/OdysseyInterpolationTypes.h"
#include "FreehandShape/Interpolation/IOdysseyInterpolation.h"
#include "FreehandShape/Smoothing/OdysseySmoothingOptions.h"
#include "FreehandShape/Smoothing/IOdysseySmoothing.h"
#include "OdysseyShape.h"

#include "OdysseyFreehandShape.generated.h"

UCLASS(meta=(DisplayName="Freehand Shape"))
class ODYSSEYSHAPES_API UOdysseyFreehandShape : public UOdysseyShape
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
    virtual ~UOdysseyFreehandShape();

public:
    //Mouse events
    virtual void OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual void OnKeyDown(const FKey& iKey);
    virtual void OnKeyUp(const FKey& iKey);

public:
    // Tick
    void Tick(float iDeltaTime);

    // Applies the shapes specific overrides
    void ApplyOverrides(const TMap<FName, UObject*>& iOverrides);

public:
    // Getters
    //Returns the SmoothingOptions
    FOdysseySmoothingOptions& GetSmoothingOptions();

    //Returns the interpolation step
    float GetStep() const;

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

private:
    // Internal - Smoothing

    //Init the Smoothing, choosing the right smoother
    void InitSmoothing();

    // Begins the smoothing process
    void BeginSmoothing();

    // Smoothes the given point
    // Returns false if no smoothing point has been generated
    bool SmoothTo(const FOdysseyPoint& iPoint);

    // Ends the smoothing process
    void EndSmoothing();

    // Aborts the smoothing process
    void AbortSmoothing();

    // Resets the smoothing process, not ending nor aborting it, just retrieving a state where the smoother has been initialized
    void ResetSmoothing();

    // Applies the CatchUp if needed
    void CatchUp();

    // Applies the smoothing if enabled and not in realtime
    void ReapplySmoothing();

private:
    // Internal - Interpolation

    //Init the Interpolation, choosing the right interpolator
    void InitInterpolation();

    //Begins an interpolation
    void BeginInterpolation();

    //Interpolates to the given point
    TArray<FOdysseyPoint> InterpolateTo(const FOdysseyPoint& iPoint);

    //Ends the interpolation
    void EndInterpolation();

    //Aborts the interpolation
    void AbortInterpolation();

    // Resets the interpolation, not ending nor aborting it, just retrieving a state where the interpolator has been initialized
    void ResetInterpolation();

private:
    //PROPERTIES
    
    //The smoothing options
    UPROPERTY( EditInstanceOnly, Category="Smoothing" )
    FOdysseySmoothingOptions SmoothingOptions;

    UPROPERTY( EditInstanceOnly, Category="Interpolation")
    bool    AdaptativeStep = false;

    UPROPERTY( EditInstanceOnly, Category="Interpolation", meta = ( ClampMin = "1", ClampMax = "200", UIMin = "1", UIMax = "200", SliderExponent = "1" ) )
    float   Step = 20.f;

    UPROPERTY( EditInstanceOnly, Category="Interpolation")
    EOdysseyInterpolationType InterpolationType = EOdysseyInterpolationType::kCatmullRom;

protected:
    // protected Data Members

    //---

    //Internal
    TArray< FOdysseyPoint >             mRawStroke; //Raw Stroke (basically mouse positions)
    TArray< FOdysseyPoint >             mSmoothedStroke; //The raw stroke once smoothed using the smoother
    TArray< FOdysseyPoint >             mInterpolatedStroke; //the smoothed stroke once interpolated using the interpolator

    TSharedPtr<IOdysseySmoothing>       mSmoother;
    TSharedPtr<IOdysseyInterpolation>   mInterpolator;
    bool                                mHasStrokeBegun;

    FOnPathBegin                        mOnPathBeginDelegate;
    FOnPathTo                           mOnPathToDelegate;
    FOnPathEnd                          mOnPathEndDelegate;
    FOnReset                            mOnResetDelegate;

    FAdaptStep                          mAdaptStepDelegate;
};
