#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include <ULIS>

class IOdysseyVectorCell;

class IOdysseyVectorLayer
{
    public:
        virtual IOdysseyVectorCell* GetCellByIndex( uint32 iIndex ) = 0;
        virtual IOdysseyVectorCell* GetLastCell() = 0;
        virtual IOdysseyVectorCell* GetFirstCell() = 0;
};
