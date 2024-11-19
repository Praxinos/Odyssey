// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
