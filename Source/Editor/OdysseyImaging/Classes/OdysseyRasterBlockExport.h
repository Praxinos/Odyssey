#pragma once

#include "CoreMinimal.h"

class FOdysseyRasterBlock;

class FOdysseyRasterBlockExport
{
public:
    static void ODYSSEYIMAGING_API Write( FOdysseyRasterBlock* iRasterBlock, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteId( FOdysseyRasterBlock* iRasterBlock, FArchive &Ar );
    static void WriteResolution( FOdysseyRasterBlock* iRasterBlock, FArchive &Ar );
    static void WriteFormat( FOdysseyRasterBlock* iRasterBlock, FArchive &Ar );
    static void WriteBulkData( FOdysseyRasterBlock* iRasterBlock, FArchive &Ar );
};
