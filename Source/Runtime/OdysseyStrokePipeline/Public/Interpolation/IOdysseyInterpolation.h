// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OdysseyStrokePoint.h"

/**
 * Implements the interpolation for a stroke
 */
class ODYSSEYSTROKEPIPELINE_API IOdysseyInterpolation
{
public:
    virtual ~IOdysseyInterpolation() {}
    IOdysseyInterpolation()
    : step( 1.f )
    , fLastDrawnLength( 0.f )
    , fTotalStrokeLength( 0.f )
    {}

public:
    virtual bool IsReady() const = 0;
    virtual int MinimumRequiredPoints() const = 0;
    virtual const TArray< FOdysseyStrokePoint >& ComputePoints() = 0;

    void Reset() { inputPoints.Empty(); resultPoints.Empty(); fLastDrawnLength = 0.f; fTotalStrokeLength = 0.f; }
    void AddPoint( const FOdysseyStrokePoint& iPoint ) { inputPoints.Add( iPoint ); }

    float GetStep() const { return  step; }
    void SetStep( float iValue ) { step = iValue; }


protected:
    TArray< FOdysseyStrokePoint > inputPoints;
    TArray< FOdysseyStrokePoint > resultPoints;
    float step;
    float fLastDrawnLength;
    float fTotalStrokeLength;
};

