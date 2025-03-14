// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
