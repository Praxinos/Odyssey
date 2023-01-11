#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorEngine.h"

namespace FOdysseyVectorExport
{
    // constants are initially computed from the CRC32 checksum of the constant's name, even though the constant's name may have changed over time
    static const uint32 CHUNK_VECTOR_MAGIC = 0x9680b8e1; // container
        static const uint32 CHUNK_OBJECTS_DECLARE = 0xfcde81bf ; // container
            static const uint32 CHUNK_OBJECTS_DECLARE_OBJECT = 0x58c21bfa ; // uint32(Type)

        static const uint32 CHUNK_OBJECTS_DEFINE = 0x5d3d18e9 ; // container
            static const uint32 CHUNK_OBJECTS_DEFINE_OBJECT = 0xb80d4393 ; // container
                static const uint32 CHUNK_OBJECTS_DEFINE_OBJECT_ID = 0x921ba730 ; // uint32(objectID)
                static const uint32 CHUNK_OBJECTS_DEFINE_OBJECT_PARENTID = 0xaeddfa4d; // uint32(ParentID)
                static const uint32 CHUNK_OBJECTS_DEFINE_OBJECT_TRANSFORM = 0x3bf02661; // container
                    static const uint32 CHUNK_OBJECTS_DEFINE_OBJECT_TRANSFORM_TRANSLATION = 0x621d21c; // double(X)-double(Y)
                    static const uint32 CHUNK_OBJECTS_DEFINE_OBJECT_TRANSFORM_ROTATION = 0xbf4d725c; // double(A)
                    static const uint32 CHUNK_OBJECTS_DEFINE_OBJECT_TRANSFORM_SCALING = 0x4cc21f6d; // double(X)-double(Y)
                static const uint32 CHUNK_OBJECTS_DEFINE_OBJECT_STROKECOLOR = 0xd490269b; // uint8(G)-uint8(R)-uint8(B)
                static const uint32 CHUNK_OBJECTS_DEFINE_OBJECT_FILLCOLOR = 0xd9c2a698; // uint8(G)-uint8(R)-uint8(B)
                static const uint32 CHUNK_OBJECTS_DEFINE_OBJECT_PATHCUBIC = 0x138d0bec; // container
                    static const uint32 CHUNK_OBJECTS_DEFINE_OBJECT_PATHCUBIC_GEOMETRY = 0x904a4229; // container
                    static const uint32 CHUNK_OBJECTS_DEFINE_OBJECT_PATHCUBIC_GEOMETRY_VERTICES = 0x1116a85d; // uint32(count), array[double(X)-double(Y)-double(Radius)]
                    static const uint32 CHUNK_OBJECTS_DEFINE_OBJECT_PATHCUBIC_GEOMETRY_SEGMENTS = 0x76cacf19; // uint32(count), array[uint32(P0ID)-uint32(P1ID)-double(CX0)-double(CY0)-double(CX1)-double(CY1)]

    void ODYSSEYVECTOR_API WriteChunk( uint32 iChunkID, FArchive &Ar, std::function<void(FArchive &Ar)> iCallback );

    void ODYSSEYVECTOR_API Write( FOdysseyVectorEngine* iVEngine, FArchive &Ar );
    void ODYSSEYVECTOR_API WriteObjectsDeclare( std::vector<UOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar );
    void ODYSSEYVECTOR_API WriteObjectsDefine( std::vector<UOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar );
    void ODYSSEYVECTOR_API WritePathCubic( UOdysseyVectorPathCubic& iScene, FArchive &Ar );
    void ODYSSEYVECTOR_API WriteCircle( UOdysseyVectorCircle& iScene, FArchive &Ar );
};
