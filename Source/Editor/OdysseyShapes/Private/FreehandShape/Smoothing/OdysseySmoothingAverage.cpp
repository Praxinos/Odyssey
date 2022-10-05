// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "FreehandShape/Smoothing/OdysseySmoothingAverage.h"
#include "Math/OdysseyMathUtils.h"


#define LOCTEXT_NAMESPACE "OdysseySmoothingAverage"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseySmoothingAverage::~FOdysseySmoothingAverage()
{
}

FOdysseySmoothingAverage::FOdysseySmoothingAverage(FOdysseySmoothingOptions* iSmoothingOptions)
    : IOdysseySmoothing(iSmoothingOptions)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

bool
FOdysseySmoothingAverage::IsReady() const
{
    return mPoints.Num() > mSmoothingOptions->SmoothingStrength;
}

void
FOdysseySmoothingAverage::AddPoint( const FOdysseyPoint& iPoint )
{
    for (int i = mPoints.Num(); i < mSmoothingOptions->SmoothingStrength + 1; i++)
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
