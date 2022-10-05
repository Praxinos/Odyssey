// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Input/OdysseyPoint.h"
#include "FreehandShape/Smoothing/OdysseySmoothingOptions.h"

/**
 * Implements the smoothing for a Bezier 3 Points
 */
class ODYSSEYSHAPES_API IOdysseySmoothing
{
public:
    virtual ~IOdysseySmoothing() = 0;
    IOdysseySmoothing(FOdysseySmoothingOptions* iSmoothingOptions);

public:
    virtual bool IsReady() const = 0;
    virtual FOdysseyPoint ComputePoint() = 0;
    virtual void AddPoint( const FOdysseyPoint& iPoint );
    virtual void Reset();
    virtual bool CanCatchUp() const;

protected:
    TArray< FOdysseyPoint> mPoints;
    FOdysseySmoothingOptions* mSmoothingOptions;
};
