#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVector.h"

namespace FOdysseyVectorExportV2
{
    // constants were initially computed from the CRC32 checksum of the constant's name, even though the constant's name may have changed over time
    // Just be sure the Chunk ID is unique and any ID will make it.
    // You can use website https://crc32.online/ to generate a code
    static const uint32 CHUNK_VECTOR_MAGIC_V2 =  0xd513065c; // container
        static const uint32 CHUNK_DECLARE_OBJECTS = 0xfcde81bf ; // container
            static const uint32 CHUNK_DECLARE_OBJECT_ENTRY = 0x58c21bfa ; // uint32(Type)

        static const uint32 CHUNK_DEFINE_OBJECTS = 0x5d3d18e9 ; // container
            static const uint32 CHUNK_DEFINE_OBJECT_ENTRY = 0xb80d4393 ; // container
                static const uint32 CHUNK_DEFINE_OBJECT_ID = 0x69abd8a3; // (uint32 objectID)

                static const uint32 CHUNK_OBJECT = 0x51334470; // container
                    static const uint32 CHUNK_OBJECT_PARENTID = 0xaeddfa4d; // uint32(ParentID)
                    static const uint32 CHUNK_OBJECT_TRANSFORM = 0x3bf02661; // container
                        static const uint32 CHUNK_OBJECT_TRANSFORM_TRANSLATION = 0x621d21c; // double(X)-double(Y)
                        static const uint32 CHUNK_OBJECT_TRANSFORM_ROTATION = 0xbf4d725c; // double(A)
                        static const uint32 CHUNK_OBJECT_TRANSFORM_SCALING = 0x4cc21f6d; // double(X)-double(Y)
                    static const uint32 CHUNK_OBJECT_FOREGROUNDCOLOR = 0xd490269b; // uint8(R)-uint8(G)-uint8(B)-uint8(A)
                    static const uint32 CHUNK_OBJECT_BACKGROUNDCOLOR = 0xd9c2a698; // uint8(R)-uint8(G)-uint8(B)-uint8(A)

                    static const uint32 CHUNK_OBJECT_FOREGROUNDBUCKET = 0x200f1262; // container
                        //static const uint32 CHUNK_BUCKET_ENTRY = ... // see CHUNK_BUCKET_ENTRY
                    static const uint32 CHUNK_OBJECT_BACKGROUNDBUCKET = 0x1816973a; // container
                        //static const uint32 CHUNK_BUCKET_ENTRY = ... // see CHUNK_BUCKET_ENTRY

                static const uint32 CHUNK_PATH = 0x138d0bec; // container
                    // ... inherited chunks.
                    static const uint32 CHUNK_PATH_JOINT = 0x272518f6; // uint32(type)
                    static const uint32 CHUNK_PATH_GEOMETRY = 0x904a4229; // container
                        // packed version
                        static const uint32 CHUNK_PATH_GEOMETRY_VERTICES = 0x1116a85d; // uint32(count), array[double(X)-double(Y)-double(Radius)]
                        // per-vertex version. Slower but more convenient to handle
                        static const uint32 CHUNK_PATH_GEOMETRY_VERTEX =  0x1f128031; // container
                            static const uint32 CHUNK_PATH_GEOMETRY_VERTEX_POSITION = 0xc66f513b; // double(X)-double(Y)-double(Radius)
                            static const uint32 CHUNK_PATH_GEOMETRY_VERTEX_HANDLEALIGNMENT = 0x285aa8d4; // uint32(bool)

                        static const uint32 CHUNK_PATH_GEOMETRY_CUBICSEGMENTS = 0x76cacf19; // uint32(count), array[uint32(P0ID)-uint32(P1ID)-double(CX0)-double(CY0)-double(CX1)-double(CY1)]

                static const uint32 CHUNK_GROUPPAINT = 0xac92b85d; // container
                    // ... +inherited chunks.
                    static const uint32 CHUNK_GROUPPAINT_PAINTED = 0x89854E17; // uint32(bool)
                    static const uint32 CHUNK_GROUPPAINT_MONOCHROME = 0x8278C142; // uint32(bool)
                    static const uint32 CHUNK_GROUPPAINT_MONOCHROMECOLOR = 0x2E97CB32; //uint8(R)-uint8(G)-uint8(B)-uint8(A)
                    static const uint32 CHUNK_GROUPPAINT_WIREFRAME =  0x54F22893; // uint32(bool)
                    static const uint32 CHUNK_GROUPPAINT_WIREFRAMECOLOR = 0x0AB56FA6; //uint8(R)-uint8(G)-uint8(B)-uint8(A)
                    static const uint32 CHUNK_GROUPPAINT_GAP = 0x7602b7f4; // container
                        static const uint32 CHUNK_GROUPPAINT_GAP_TOLERANCE = 0x960a3861; // double(tolerance)
                    static const uint32 CHUNK_GROUPPAINT_BUCKETS = 0x5791cb88; // container
                        static const uint32 CHUNK_BUCKET_ENTRY = 0x94267aa5; // container
                            static const uint32 CHUNK_BUCKET_SPREADING = 0x402c03f1;  // uint32(spreadingPolicy)
                            static const uint32 CHUNK_BUCKET_POSITION = 0xd492a193; // uint8(R)-uint8(G)-uint8(B)-uint8(A)
                            static const uint32 CHUNK_BUCKET_ROTATION =  0xbbc2dd97; // double(rotation)
                            static const uint32 CHUNK_BUCKET_PROPAGATED = 0x2c1333ce; // uint32(bool)
                            static const uint32 CHUNK_BUCKET_COLORMODE = 0x9529f13e;  // uint32(colorMode)
                            static const uint32 CHUNK_BUCKET_PALETTEENTRY = 0xac23cabe; // FName to load PaletteEntry
                            static const uint32 CHUNK_BUCKET_SOLIDCOLOR = 0xd951af3c; // uint8(R)-uint8(G)-uint8(B)-uint8(A)
                            static const uint32 CHUNK_BUCKET_GRADIENT = 0x5d7bbd93; // container
                                static const uint32 CHUNK_BUCKET_GRADIENT_STOP = 0xd0dfb9d3; // uint8(R)-uint8(G)-uint8(B)-uint8(A)-float(stop)

                static const uint32 CHUNK_SCENE =  0xfe491c66; // container
                    // ... +inherited chunks.

    void ODYSSEYVECTOR_API WriteChunk( uint32 iChunkID, FArchive &Ar, std::function<void(FArchive &Ar)> iCallback );

    void ODYSSEYVECTOR_API Write( FOdysseyVectorScene* iScene, FArchive &Ar );

    ////////////////////////////////////
    void ODYSSEYVECTOR_API WriteObject( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteDefineObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar );
    void WriteDefineObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteDefineObjectID( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectChunks( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectBackgroundColor( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectForegroundColor( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectBackgroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectForegroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectID( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectParentID( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectTransform( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectTransformTranslation( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectTransformRotation( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectTransformScaling( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteDeclareObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar );
    void WriteDeclareObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar );

    ////////////////////////////////////
    void ODYSSEYVECTOR_API WriteBucket( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketColor( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketGradient( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketGradientStop( FColor& iStopColor, double iStopAt, FArchive &Ar );
    void WriteBucketRotation( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketPosition( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketSpreading( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketPropagated( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketPaletteEntry( FOdysseyVectorBucket& iBucket, FArchive& Ar);
    void WriteBucketColorMode( FOdysseyVectorBucket& iBucket, FArchive &Ar );

    ////////////////////////////////////
    void ODYSSEYVECTOR_API WritePath( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathGeometrySegments( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathGeometryVertices( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathGeometryVertex( FOdysseyVectorVertex& iVertex, FArchive &Ar );
    void WritePathGeometryVertexPosition( FOdysseyVectorVertex& iVertex, FArchive &Ar );
    void WritePathGeometryVertexHandleAlignment( FOdysseyVectorVertex& iVertex, FArchive &Ar );
    void WritePathGeometry( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathJoint( FOdysseyVectorPath& iPath, FArchive &Ar );

    ////////////////////////////////////
    void ODYSSEYVECTOR_API WriteGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintChunks( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintBuckets( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintGapTolerance( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintGap( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintMonochromeColor( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintMonochrome( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintWireframeColor( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintWireframe( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintPainted( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );

    void ODYSSEYVECTOR_API WriteScene( FOdysseyVectorScene& iScene, FArchive &Ar );
    void WriteSceneChunks( FOdysseyVectorScene& iScene, FArchive &Ar );
}
