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
