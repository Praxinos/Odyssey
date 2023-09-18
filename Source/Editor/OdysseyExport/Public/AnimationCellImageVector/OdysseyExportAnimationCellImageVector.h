#pragma once

#include "CoreMinimal.h"
#include "OdysseyVector.h"
#include "OdysseyExport.h"

class FOdysseyAnimationCellImageVector;

namespace FOdysseyExportAnimationCellImageVector
{
    // constants were initially computed from the CRC32 checksum of the constant's name, even though the constant's name may have changed over time
    // Just be sure the Chunk ID is unique and any ID will make it.
    // You can use website https://crc32.online/ to generate a code
    static const uint32 CHUNK_ANIMATIONCELLIMAGEVECTOR =  0x60fdee3a; // container
        static const uint32 CHUNK_ANIMATIONCELLIMAGEVECTOR_RESOLUTION = 0xc1bb48e1; // uint32(Width)-uint32(Height)
        static const uint32 CHUNK_ANIMATIONCELLIMAGEVECTOR_BLOCK = 0xf0a9e121; // container
            static const uint32 CHUNK_ANIMATIONCELLIMAGEVECTOR_BLOCK_ID =  0x194b349c; // FGuid(VectorBlockId)
        //static const uint32 CHUNK_VECTOR_MAGIC_V2 // see CHUNK_VECTOR_MAGIC_V2

    void ODYSSEYEXPORT_API Write( FOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );

    ////////////////////////////////////
    void WriteResolution( FOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
    void WriteVectorBlock( FOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
    void WriteVectorBlockID( FOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
}
