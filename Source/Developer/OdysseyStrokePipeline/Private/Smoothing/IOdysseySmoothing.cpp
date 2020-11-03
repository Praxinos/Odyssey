// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Smoothing/IOdysseySmoothing.h"

#define LOCTEXT_NAMESPACE "IOdysseySmoothing"

//---

IOdysseySmoothing::~IOdysseySmoothing()
{
}

IOdysseySmoothing::IOdysseySmoothing(FOdysseySmoothingParameters* iParameters)
    : mPoints()
    , mParameters(iParameters)
{
}

//---

void
IOdysseySmoothing::Reset()
{
    mPoints.Empty();
}

void
IOdysseySmoothing::AddPoint( const FOdysseyStrokePoint& iPoint )
{
    mPoints.Add( iPoint );
}

bool
IOdysseySmoothing::CanCatchUp() const
{
    return false;
}

//---

#undef LOCTEXT_NAMESPACE
