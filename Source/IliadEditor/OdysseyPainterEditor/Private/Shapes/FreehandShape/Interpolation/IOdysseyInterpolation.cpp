// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "FreehandShape/Interpolation/IOdysseyInterpolation.h"

//---

IOdysseyInterpolation::~IOdysseyInterpolation()
{
}

IOdysseyInterpolation::IOdysseyInterpolation()
    : mInputPoints()
    , mResultPoints()
    , mStep(1.f)
    , mLastDrawnLength(0.f)
    , mTotalStrokeLength(0.f)
{
}

//---

void
IOdysseyInterpolation::Reset()
{
    mInputPoints.Empty();
    mResultPoints.Empty();
    mLastDrawnLength = 0.f;
    mTotalStrokeLength = 0.f;
}

void
IOdysseyInterpolation::AddPoint(const FOdysseyPoint& iPoint)
{
    do
    {
        mInputPoints.Add(iPoint);
    } while (mInputPoints.Num() < MinimumRequiredPoints());
}

float
IOdysseyInterpolation::GetStep() const
{
    return mStep;
}

void
IOdysseyInterpolation::SetStep(float iValue)
{
    mStep = iValue;
}
