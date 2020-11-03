// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "OdysseyStrokePoint.h"

/**
 * Implements the interpolation for a stroke
 */
class ODYSSEYSTROKEPIPELINE_API IOdysseyInterpolation
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
    virtual const TArray< FOdysseyStrokePoint >& ComputePoints() = 0;

    void Reset();
    void AddPoint( const FOdysseyStrokePoint& iPoint );

    float GetStep() const;
    void SetStep( float iValue );

protected:
    TArray< FOdysseyStrokePoint > mInputPoints;
    TArray< FOdysseyStrokePoint > mResultPoints;
    float mStep;
    float mLastDrawnLength;
    float mTotalStrokeLength;
};
