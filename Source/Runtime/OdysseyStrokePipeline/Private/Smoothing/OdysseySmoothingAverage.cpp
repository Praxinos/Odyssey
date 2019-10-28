// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "Smoothing/OdysseySmoothingAverage.h"
#include "OdysseyMathUtils.h"


#define LOCTEXT_NAMESPACE "OdysseySmoothingAverage"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseySmoothingAverage::FOdysseySmoothingAverage()
{
}


FOdysseySmoothingAverage::~FOdysseySmoothingAverage()
{
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

bool
FOdysseySmoothingAverage::IsReady() const
{
    return ( points.Num() > MinimumRequiredPoints() );
}


int
FOdysseySmoothingAverage::MinimumRequiredPoints()  const
{
    return  strength;
}


FOdysseyStrokePoint
FOdysseySmoothingAverage::ComputePoint()
{
    if( !IsReady() )
        return FOdysseyStrokePoint();

    FOdysseyStrokePoint outPoint = FOdysseyStrokePoint::ZeroPoint();
    for( int i = 0; i < points.Num(); ++i )
        outPoint += points[i];

    outPoint /= points.Num();

    points.RemoveAt( 0 );
    return  outPoint;
}


#undef LOCTEXT_NAMESPACE
