#pragma once

#include <Core/Core.h>

class FOdysseyRasterBlock;

class FOdysseyRasterBlockImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     *
     * @param iRasterBlock
     * @param Ar
     */
    static bool ODYSSEYIMAGING_API Read( FOdysseyRasterBlock* iRasterBlock
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( FOdysseyRasterBlock* iRasterBlock
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
