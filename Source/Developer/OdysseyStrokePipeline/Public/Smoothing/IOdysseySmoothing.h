// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "OdysseyStrokePoint.h"
#include "OdysseySmoothingParameters.h"

/**
 * Implements the smoothing for a Bezier 3 Points
 */
class ODYSSEYSTROKEPIPELINE_API IOdysseySmoothing
{
public:
    virtual ~IOdysseySmoothing() = 0;
    IOdysseySmoothing(FOdysseySmoothingParameters* iParameters);

public:
    virtual bool IsReady() const = 0;
    virtual FOdysseyStrokePoint ComputePoint() = 0;
    virtual void AddPoint( const FOdysseyStrokePoint& iPoint );
    virtual void Reset();

protected:
    TArray< FOdysseyStrokePoint> mPoints;
    FOdysseySmoothingParameters* mParameters;
};
