// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "OdysseyStrokePoint.h"

/**
 * Implements the smoothing for a Bezier 3 Points
 */
class ODYSSEYSTROKEPIPELINE_API IOdysseySmoothing
{
public:
    virtual ~IOdysseySmoothing() = 0;
    IOdysseySmoothing();

public:
    virtual bool IsReady() const = 0;
    virtual int MinimumRequiredPoints() const = 0;
    virtual FOdysseyStrokePoint ComputePoint() = 0;

    void Reset();
    void AddPoint( const FOdysseyStrokePoint& iPoint );

    int GetStrength() const;
    void SetStrength( int iValue );

protected:
    TArray< FOdysseyStrokePoint> mPoints;
    int mStrength;
};
