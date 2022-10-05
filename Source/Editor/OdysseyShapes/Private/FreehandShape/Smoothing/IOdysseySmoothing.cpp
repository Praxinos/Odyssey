// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "FreehandShape/Smoothing/IOdysseySmoothing.h"

#define LOCTEXT_NAMESPACE "IOdysseySmoothing"

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

//---

#undef LOCTEXT_NAMESPACE
