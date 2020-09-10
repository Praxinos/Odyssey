// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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

//---

#undef LOCTEXT_NAMESPACE
