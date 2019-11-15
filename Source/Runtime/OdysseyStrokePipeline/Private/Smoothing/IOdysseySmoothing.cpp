// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "Smoothing/IOdysseySmoothing.h"

#define LOCTEXT_NAMESPACE "IOdysseySmoothing"

//---

IOdysseySmoothing::~IOdysseySmoothing()
{
}

IOdysseySmoothing::IOdysseySmoothing()
    : mPoints()
    , mStrength( 0 )
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

int
IOdysseySmoothing::GetStrength() const
{
    return mStrength;
}

void
IOdysseySmoothing::SetStrength( int iValue )
{
    mStrength = iValue;
}

//---

#undef LOCTEXT_NAMESPACE
