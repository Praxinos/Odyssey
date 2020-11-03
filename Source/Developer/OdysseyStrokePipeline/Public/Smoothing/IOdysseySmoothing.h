// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
    virtual bool CanCatchUp() const;

protected:
    TArray< FOdysseyStrokePoint> mPoints;
    FOdysseySmoothingParameters* mParameters;
};
