// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "StrokeEngine/Smoothing/OdysseySmoothingAverage.h"
#include "Math/OdysseyMathUtils.h"


#define LOCTEXT_NAMESPACE "OdysseySmoothingAverage"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseySmoothingAverage::~FOdysseySmoothingAverage()
{
}

FOdysseySmoothingAverage::FOdysseySmoothingAverage(FOdysseyStrokeOptions* iStrokeOptions)
    : IOdysseySmoothing(iStrokeOptions)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

bool
FOdysseySmoothingAverage::IsReady() const
{
    return mPoints.Num() > mStrokeOptions->SmoothingStrength;
}

void
FOdysseySmoothingAverage::AddPoint( const FOdysseyPoint& iPoint )
{
    for (int i = mPoints.Num(); i < mStrokeOptions->SmoothingStrength + 1; i++)
    {
        IOdysseySmoothing::AddPoint(iPoint);
    }
}

FOdysseyPoint
FOdysseySmoothingAverage::ComputePoint()
{
    if( !IsReady() )
        return FOdysseyPoint();

    FOdysseyPoint outPoint = FOdysseyPoint::Average( mPoints );

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
