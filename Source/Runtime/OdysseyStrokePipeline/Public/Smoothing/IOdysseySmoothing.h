// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OdysseyStrokePoint.h"

/**
 * Implements the smoothing for a Bezier 3 Points
 */
class ODYSSEYSTROKEPIPELINE_API IOdysseySmoothing
{
public:
    virtual ~IOdysseySmoothing() {}
    IOdysseySmoothing()
    : strength( 0 )
    {}

public:
    virtual bool IsReady() const = 0;
    virtual int MinimumRequiredPoints() const = 0;
    virtual FOdysseyStrokePoint ComputePoint() = 0;

    void Reset() { points.Empty(); }
    void AddPoint( const FOdysseyStrokePoint& iPoint ) { points.Add( iPoint ); }

    int GetStrength() const { return strength; }
    void SetStrength( int iValue ) { strength = iValue; }

protected :
    TArray< FOdysseyStrokePoint> points;
    int strength;
};

