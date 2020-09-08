// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "IOdysseySerializable.h"

//---

IOdysseySerializable::~IOdysseySerializable()
{
}

IOdysseySerializable::IOdysseySerializable( const uint8 iVersion )
    : mVersion( iVersion )
{
}

//---

uint8
IOdysseySerializable::GetVersion() const
{
    return mVersion;
}
