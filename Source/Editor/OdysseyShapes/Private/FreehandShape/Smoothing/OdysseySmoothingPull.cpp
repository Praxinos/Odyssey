// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "FreehandShape/Smoothing/OdysseySmoothingPull.h"
#include "Math/OdysseyMathUtils.h"
#include "GenericPlatform/GenericPlatformMath.h"


#define LOCTEXT_NAMESPACE "OdysseySmoothingPull"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseySmoothingPull::~FOdysseySmoothingPull()
{
}

FOdysseySmoothingPull::FOdysseySmoothingPull(FOdysseySmoothingOptions* iSmoothingOptions)
    : IOdysseySmoothing(iSmoothingOptions)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

bool
FOdysseySmoothingPull::IsReady() const
{
	if (mPoints.Num() <= 0)
		return false;

	if (mPoints.Num() == 1)
		return true;

    FOdysseyPoint point = mPoints[mPoints.Num() - 1];
    float x = point.x - mPoints[0].x;
    float y = point.y - mPoints[0].y;
    float dist2 = x * x + y * y;
    return dist2 >= mSmoothingOptions->SmoothingStrength * mSmoothingOptions->SmoothingStrength;
}

FOdysseyPoint
FOdysseySmoothingPull::ComputePoint()
{
    if( !IsReady() )
        return FOdysseyPoint();

	if (mPoints.Num() == 1)
		return mPoints[0];

	if (mSmoothingOptions->SmoothingStrength <= 0)
	{
		FOdysseyPoint outPoint = mPoints[mPoints.Num() - 1];
		mPoints.Empty();
		mPoints.Add(outPoint);
		return outPoint;
	}

    FOdysseyPoint point = mPoints[mPoints.Num() - 1];
    float x = point.x - mPoints[0].x;
    float y = point.y - mPoints[0].y;
    float dist = FGenericPlatformMath::Sqrt(x * x + y * y);
    float ratio = (dist - mSmoothingOptions->SmoothingStrength) / dist;

    point.x = mPoints[0].x + x * ratio;
    point.y = mPoints[0].y + y * ratio;
	mPoints.Empty();
	mPoints.Add(point);

    return point;
}

//---

#undef LOCTEXT_NAMESPACE
