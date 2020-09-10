// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Smoothing/OdysseySmoothingPull.h"
#include "OdysseyMathUtils.h"
#include "GenericPlatform/GenericPlatformMath.h"


#define LOCTEXT_NAMESPACE "OdysseySmoothingPull"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseySmoothingPull::~FOdysseySmoothingPull()
{
}

FOdysseySmoothingPull::FOdysseySmoothingPull()
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
    return dist2 >= mStrength * mStrength;
}

int
FOdysseySmoothingPull::MinimumRequiredPoints() const
{
    return 1;
}

FOdysseyStrokePoint
FOdysseySmoothingPull::ComputePoint()
{
    if( !IsReady() )
        return FOdysseyStrokePoint();

	if (mPoints.Num() == 1)
		return mPoints[0];

	if (mStrength <= 0)
	{
		FOdysseyStrokePoint outPoint = mPoints[mPoints.Num() - 1];
		mPoints.Empty();
		mPoints.Add(outPoint);
		return outPoint;
	}

    FOdysseyStrokePoint distPoint = mPoints[mPoints.Num() - 1] - mPoints[0];
    float dist2 = distPoint.x * distPoint.x + distPoint.y * distPoint.y;
    float ratio = FGenericPlatformMath::Sqrt(dist2 / (mStrength * mStrength));

	FOdysseyStrokePoint outPoint = mPoints[mPoints.Num() - 1];
    outPoint.x = mPoints[0].x + distPoint.x * (ratio - 1.0);
    outPoint.y = mPoints[0].y + distPoint.y * (ratio - 1.0);
	mPoints.Empty();
	mPoints.Add(outPoint);

    return outPoint;
}

//---

#undef LOCTEXT_NAMESPACE
