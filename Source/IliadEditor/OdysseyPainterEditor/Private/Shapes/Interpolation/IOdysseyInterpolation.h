// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Input/OdysseyPoint.h"

/**
* Implements the interpolation for a stroke
*/
class IOdysseyInterpolation
{
public:
    // Construction / Destruction
    virtual ~IOdysseyInterpolation() = 0;
    IOdysseyInterpolation();

public:
    //Returns true when the interpolation is ready to compute at least 1 point
    virtual bool IsReady() const = 0;

    //Returns the amount of point needed to define the starting point correctly
    // Example : for Catmul-Rom, the curve starts at the second point, so it will return 2
    // Example : for Bezier, the curve starts at the first point, so it will return 1
    virtual int MinimumRequiredPoints() const = 0;
    virtual const TArray< FOdysseyPoint >& ComputePoints() = 0;

    void Reset();
    void AddPoint(const FOdysseyPoint& iPoint);

    float GetStep() const;
    void SetStep(float iValue);

protected:
    TArray< FOdysseyPoint > mInputPoints;
    TArray< FOdysseyPoint > mResultPoints;
    float mStep;
    float mLastDrawnLength;
    float mTotalStrokeLength;
};
