// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "Smoothing/OdysseySmoothingParameters.h"

#define LOCTEXT_NAMESPACE "FOdysseySmoothingParameters"

//---

FOdysseySmoothingParameters::~FOdysseySmoothingParameters()
{
}

FOdysseySmoothingParameters::FOdysseySmoothingParameters()
    : mStrength( 0 )
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

//---

#undef LOCTEXT_NAMESPACE
