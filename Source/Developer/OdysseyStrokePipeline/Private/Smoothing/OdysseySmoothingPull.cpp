// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Smoothing/OdysseySmoothingPull.h"
#include "OdysseyMathUtils.h"
#include "GenericPlatform/GenericPlatformMath.h"


#define LOCTEXT_NAMESPACE "OdysseySmoothingPull"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseySmoothingPull::~FOdysseySmoothingPull()
{
}

FOdysseySmoothingPull::FOdysseySmoothingPull(FOdysseySmoothingParameters* iParameters)
    : IOdysseySmoothing(iParameters)
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

    FOdysseyStrokePoint point = mPoints[mPoints.Num() - 1];
    float x = point.x - mPoints[0].x;
    float y = point.y - mPoints[0].y;
    float dist2 = x * x + y * y;
    return dist2 >= mParameters->GetStrength() * mParameters->GetStrength();
}

FOdysseyStrokePoint
FOdysseySmoothingPull::ComputePoint()
{
    if( !IsReady() )
        return FOdysseyStrokePoint();

	if (mPoints.Num() == 1)
		return mPoints[0];

	if (mParameters->GetStrength() <= 0)
	{
		FOdysseyStrokePoint outPoint = mPoints[mPoints.Num() - 1];
		mPoints.Empty();
		mPoints.Add(outPoint);
		return outPoint;
	}

    FOdysseyStrokePoint point = mPoints[mPoints.Num() - 1];
    float x = point.x - mPoints[0].x;
    float y = point.y - mPoints[0].y;
    float dist = FGenericPlatformMath::Sqrt(x * x + y * y);
    float ratio = (dist - mParameters->GetStrength()) / dist;

    point.x = mPoints[0].x + x * ratio;
    point.y = mPoints[0].y + y * ratio;
	mPoints.Empty();
	mPoints.Add(point);

    return point;
}

//---

#undef LOCTEXT_NAMESPACE
