// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Smoothing/OdysseySmoothingAverage.h"
#include "OdysseyMathUtils.h"


#define LOCTEXT_NAMESPACE "OdysseySmoothingAverage"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseySmoothingAverage::~FOdysseySmoothingAverage()
{
}

FOdysseySmoothingAverage::FOdysseySmoothingAverage(FOdysseySmoothingParameters* iParameters)
    : IOdysseySmoothing(iParameters)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

bool
FOdysseySmoothingAverage::IsReady() const
{
    return mPoints.Num() > mParameters->GetStrength();
}

void
FOdysseySmoothingAverage::AddPoint( const FOdysseyStrokePoint& iPoint )
{
    for (int i = mPoints.Num(); i < mParameters->GetStrength() + 1; i++)
    {
        IOdysseySmoothing::AddPoint(iPoint);
    }
}

FOdysseyStrokePoint
FOdysseySmoothingAverage::ComputePoint()
{
    if( !IsReady() )
        return FOdysseyStrokePoint();

    FOdysseyStrokePoint outPoint = FOdysseyStrokePoint::ZeroPoint();
    for( int i = 0; i < mPoints.Num(); ++i )
        outPoint += mPoints[i];

    outPoint /= mPoints.Num();

    mPoints.RemoveAt( 0 );

    return outPoint;
}

bool
FOdysseySmoothingAverage::CanCatchUp() const
{
    return true;
}

//---

#undef LOCTEXT_NAMESPACE
