#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVector.h"

namespace FOdysseyVectorExport
{
    // constants were initially computed from the CRC32 checksum of the constant's name, even though the constant's name may have changed over time
    // Just be sure the Chunk ID is unique and any ID will make it.
    // You can use website https://crc32.online/ to generate a code
    static const uint32 CHUNK_VECTOR_MAGIC = 0x9680b8e1; // container
        static const uint32 CHUNK_DECLARE_OBJECTS = 0xfcde81bf ; // container
            static const uint32 CHUNK_DECLARE_OBJECT_ENTRY = 0x58c21bfa ; // uint32(Type)

        static const uint32 CHUNK_DEFINE_OBJECTS = 0x5d3d18e9 ; // container
            static const uint32 CHUNK_DEFINE_OBJECT_ENTRY = 0xb80d4393 ; // container
                static const uint32 CHUNK_OBJECT_ID = 0x921ba730 ; // uint32(objectID)
                static const uint32 CHUNK_OBJECT_PARENTID = 0xaeddfa4d; // uint32(ParentID)
                static const uint32 CHUNK_OBJECT_TRANSFORM = 0x3bf02661; // container
                    static const uint32 CHUNK_OBJECT_TRANSFORM_TRANSLATION = 0x621d21c; // double(X)-double(Y)
                    static const uint32 CHUNK_OBJECT_TRANSFORM_ROTATION = 0xbf4d725c; // double(A)
                    static const uint32 CHUNK_OBJECT_TRANSFORM_SCALING = 0x4cc21f6d; // double(X)-double(Y)
                static const uint32 CHUNK_OBJECT_FOREGROUNDCOLOR = 0xd490269b; // uint8(R)-uint8(G)-uint8(B)-uint8(A)
                static const uint32 CHUNK_OBJECT_BACKGROUNDCOLOR = 0xd9c2a698; // uint8(R)-uint8(G)-uint8(B)-uint8(A)
                static const uint32 CHUNK_OBJECT_PATHCUBIC = 0x138d0bec; // container
                    static const uint32 CHUNK_PATH_JOINT = 0x272518f6; // uint32(type)
                    static const uint32 CHUNK_PATHCUBIC_GEOMETRY = 0x904a4229; // container
                        static const uint32 CHUNK_PATHCUBIC_GEOMETRY_VERTICES = 0x1116a85d; // uint32(count), array[double(X)-double(Y)-double(Radius)]
                        static const uint32 CHUNK_PATHCUBIC_GEOMETRY_SEGMENTS = 0x76cacf19; // uint32(count), array[uint32(P0ID)-uint32(P1ID)-double(CX0)-double(CY0)-double(CX1)-double(CY1)]
                static const uint32 CHUNK_OBJECT_GROUPPAINT = 0xac92b85d; // container
                    static const uint32 CHUNK_GROUPPAINT_BUCKETS = 0x5791cb88; // container
                        static const uint32 CHUNK_BUCKET_ENTRY = 0x94267aa5; // container
                            static const uint32 CHUNK_BUCKET_POSITION = 0xd492a193; // uint8(R)-uint8(G)-uint8(B)-uint8(A)
                            static const uint32 CHUNK_BUCKET_COLOR = 0xd951af3c; // uint8(R)-uint8(G)-uint8(B)-uint8(A)
                            static const uint32 CHUNK_BUCKET_GRADIENT = 0x5d7bbd93; // container
                                static const uint32 CHUNK_BUCKET_GRADIENT_HANDLE =  0x575a6a77; // container
                                static const uint32 CHUNK_BUCKET_GRADIENT_HANDLE_POSITION = 0xed18a100; // // double(X)-double(Y)
                                static const uint32 CHUNK_BUCKET_GRADIENT_STOP = 0xd0dfb9d3; // uint8(R)-uint8(G)-uint8(B)-uint8(A)-float(stop)
                static const uint32 CHUNK_OBJECT_ELLIPSE =  0x9c87ca8f; // container
                    static const uint32 CHUNK_ELLIPSE_FILLED = 0xf75a11b3; // uint32(bool)
                    static const uint32 CHUNK_ELLIPSE_GEOMETRY = 0xd2637c2e; // container
                        static const uint32 CHUNK_ELLIPSE_GEOMETRY_RADIUS = 0x831abdab ; // double(X)-double(Y)

    void ODYSSEYVECTOR_API WriteChunk( uint32 iChunkID, FArchive &Ar, std::function<void(FArchive &Ar)> iCallback );

    void ODYSSEYVECTOR_API Write( UOdysseyVectorScene* iScene, FArchive &Ar );
    void ODYSSEYVECTOR_API WriteDeclareObjects( std::vector<UOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar );
    void ODYSSEYVECTOR_API WriteDefineObjects( std::vector<UOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar );
    void ODYSSEYVECTOR_API WriteObjectPathCubic( UOdysseyVectorPathCubic& iCubicPath, FArchive &Ar );
    void ODYSSEYVECTOR_API WriteObjectGroupPaint( UOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void ODYSSEYVECTOR_API WriteObjectEllipse( UOdysseyVectorEllipse& iCircle, FArchive &Ar );
};
