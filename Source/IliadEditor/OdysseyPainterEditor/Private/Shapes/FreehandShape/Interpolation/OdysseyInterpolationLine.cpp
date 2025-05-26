// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "FreehandShape/Interpolation/OdysseyInterpolationLine.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyInterpolationLine::~FOdysseyInterpolationLine()
{
}

FOdysseyInterpolationLine::FOdysseyInterpolationLine()
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
bool
FOdysseyInterpolationLine::IsReady() const
{
    //We need 4 points to draw a spline with Catmul-Rom
    return (mInputPoints.Num() >= 2);
}

int
FOdysseyInterpolationLine::MinimumRequiredPoints() const
{
    //The line starts at the second point in Catmul-Rom
    return 1;
}

const TArray< FOdysseyPoint >& FOdysseyInterpolationLine::ComputePoints()
{
    mResultPoints.Empty();

    if (!IsReady())
        return mResultPoints;

    const FOdysseyPoint& P0 = mInputPoints[0];
    const FOdysseyPoint& P1 = mInputPoints[1];

    float pos = mLastDrawnLength + mStep;

    float length = FVector2D::Distance(P0, P1);
    float start = mTotalStrokeLength;
    float next = pos - mTotalStrokeLength;

    mTotalStrokeLength += length;

    if (next < 0)
        return mResultPoints;

    for(float i = next; i <= length; i += mStep)
    {
        FOdysseyPoint point = FOdysseyPoint::Lerp(P0, P1, i / length);
        mResultPoints.Add(point);
        mLastDrawnLength += mStep;
    }

    /* if (mTotalStrokeLength < 1.f)
    {
        mLastDrawnLength = 0.f;
        mResultPoints.Add(mInputPoints[1]);
    }*/

    mInputPoints.RemoveAt(0);

    return  mResultPoints;
}
