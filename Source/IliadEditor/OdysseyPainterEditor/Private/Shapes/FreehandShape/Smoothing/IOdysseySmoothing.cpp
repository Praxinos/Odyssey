// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "FreehandShape/Smoothing/IOdysseySmoothing.h"

//---

IOdysseySmoothing::~IOdysseySmoothing()
{
}

IOdysseySmoothing::IOdysseySmoothing(FOdysseySmoothingOptions* iSmoothingOptions)
    : mPoints()
    , mSmoothingOptions(iSmoothingOptions)
{
}

//---

void
IOdysseySmoothing::Reset()
{
    mPoints.Empty();
}

void
IOdysseySmoothing::AddPoint( const FOdysseyPoint& iPoint )
{
    mPoints.Add( iPoint );
}

bool
IOdysseySmoothing::CanCatchUp() const
{
    return false;
}
