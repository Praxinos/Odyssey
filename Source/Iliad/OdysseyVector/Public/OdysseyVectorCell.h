// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include <ULIS>

class FOdysseyVectorEngine;

class IOdysseyVectorCell
{
    public:
        virtual FOdysseyVectorEngine* GetEngine() = 0;
        virtual int32 GetIndex() = 0;
        virtual uint32 GetLength() = 0;
        virtual uint32 GetFrame() = 0;
};
