// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
    virtual bool IsReady() const = 0;
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
