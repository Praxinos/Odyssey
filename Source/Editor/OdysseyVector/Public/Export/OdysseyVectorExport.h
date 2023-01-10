#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroup.h"

namespace FOdysseyVectorExport
{
    // constants are computed from the CRC32 checksum of the constant's name
    static const uint32 EXPORT_VECTOR_MAGIC = 0x59011C47; // crc32 of EXPORT_VECTOR_MAGIC
      static const uint32 EXPORT_OBJECTS_DECLARE = 0x71FF6AF9; // crc32 of EXPORT_OBJECTS_DECLARE
        static const uint32 EXPORT_OBJECTS_DECLARE_OBJECT = 0xE008D8AA; // crc32 of EXPORT_OBJECTS_DECLARE_OBJECT

    void ODYSSEYVECTOR_API WriteChunk( uint32 iChunkID, FArchive &Ar, std::function<void(FArchive &Ar)> iCallback );

    void ODYSSEYVECTOR_API Write( UOdysseyVectorRoot& iScene, FArchive &Ar );
    void ODYSSEYVECTOR_API WritePathCubic( UOdysseyVectorPathCubic& iScene, FArchive &Ar );
    void ODYSSEYVECTOR_API WriteCircle( UOdysseyVectorCircle& iScene, FArchive &Ar );

    void ODYSSEYVECTOR_API WriteObjectsDeclareObject( UOdysseyVectorObject& iObject, FArchive &Ar );
};
