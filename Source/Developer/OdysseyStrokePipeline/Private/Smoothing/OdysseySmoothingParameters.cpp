// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Smoothing/OdysseySmoothingParameters.h"

#define LOCTEXT_NAMESPACE "FOdysseySmoothingParameters"

//---

FOdysseySmoothingParameters::~FOdysseySmoothingParameters()
{
}

FOdysseySmoothingParameters::FOdysseySmoothingParameters()
    : mStrength( 0 )
    , mCatchUp( false )
{
}

//---

int
FOdysseySmoothingParameters::GetStrength() const
{
    return mStrength;
}

void
FOdysseySmoothingParameters::SetStrength( int iValue )
{
    mStrength = iValue;
}

bool
FOdysseySmoothingParameters::GetCatchUp() const
{
    return mCatchUp;
}

void
FOdysseySmoothingParameters::SetCatchUp( bool iValue )
{
    mCatchUp = iValue;
}

//---

#undef LOCTEXT_NAMESPACE
