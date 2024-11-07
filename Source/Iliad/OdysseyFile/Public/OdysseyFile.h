#pragma once

#include "CoreMinimal.h"
#include <functional>

namespace FOdysseyFile
{
    // chunk IDs for module OdysseyVector version 1 (deprecated)
    namespace VectorV1
    {
        namespace ObjectType
        {
            // DO NOT CHANGE ! These values are saved in the save file.
            static const uint32 ROOT       = 0;
            static const uint32 NONE       = 1;
            static const uint32 GROUP      = 2;
            static const uint32 ELLIPSE    = 3;
            static const uint32 RECTANGLE  = 4;
            static const uint32 PATH       = 5;
            static const uint32 GROUPPAINT = 6;
        }

        // constants were initially computed from the CRC32 checksum of the constant's name, even though the constant's name may have changed over time
        // Just be sure the Chunk ID is unique and any ID will make it.
        // You can use website https://crc32.online/ to generate a code
        static const uint32 CHUNK_VECTOR_MAGIC_V1 = 0x9680b8e1; // container
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

                    static const uint32 CHUNK_OBJECT_FOREGROUNDBUCKET = 0x200f1262; // container
                        //static const uint32 CHUNK_BUCKET_ENTRY = ... // see CHUNK_BUCKET_ENTRY
                    static const uint32 CHUNK_OBJECT_BACKGROUNDBUCKET = 0x1816973a; // container
                        //static const uint32 CHUNK_BUCKET_ENTRY = ... // see CHUNK_BUCKET_ENTRY

                    static const uint32 CHUNK_OBJECT_PATH = 0x138d0bec; // container
                        static const uint32 CHUNK_PATH_JOINT = 0x272518f6; // uint32(type)
                        static const uint32 CHUNK_PATH_GEOMETRY = 0x904a4229; // container
                            static const uint32 CHUNK_PATH_GEOMETRY_VERTICES = 0x1116a85d; // uint32(count), array[double(X)-double(Y)-double(Radius)]
                            static const uint32 CHUNK_PATH_GEOMETRY_CUBICSEGMENTS = 0x76cacf19; // uint32(count), array[uint32(P0ID)-uint32(P1ID)-double(CX0)-double(CY0)-double(CX1)-double(CY1)]
                    static const uint32 CHUNK_OBJECT_GROUPPAINT = 0xac92b85d; // container
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
                                static const uint32 CHUNK_BUCKET_PALETTEENTRY = 0xac23cabe; // FName to load PaletteEntry
                                static const uint32 CHUNK_BUCKET_POSITION = 0xd492a193; // uint8(R)-uint8(G)-uint8(B)-uint8(A)
                                static const uint32 CHUNK_BUCKET_ROTATION =  0xbbc2dd97; // double(rotation)
                                static const uint32 CHUNK_BUCKET_SOLIDCOLOR = 0xd951af3c; // uint8(R)-uint8(G)-uint8(B)-uint8(A)
                                static const uint32 CHUNK_BUCKET_PROPAGATED = 0x2c1333ce; // uint32(bool)
                                static const uint32 CHUNK_BUCKET_GRADIENT = 0x5d7bbd93; // container
                                    static const uint32 CHUNK_BUCKET_GRADIENT_STOP = 0xd0dfb9d3; // uint8(R)-uint8(G)-uint8(B)-uint8(A)-float(stop)
                    static const uint32 CHUNK_OBJECT_ELLIPSE =  0x9c87ca8f; // container
                        static const uint32 CHUNK_ELLIPSE_FILLED = 0xf75a11b3; // uint32(bool)
                        static const uint32 CHUNK_ELLIPSE_GEOMETRY = 0xd2637c2e; // container
                            static const uint32 CHUNK_ELLIPSE_GEOMETRY_RADIUS = 0x831abdab ; // double(X)-double(Y)
    }

    // chunk IDs for module OdysseyVector version 2 (current)
    namespace VectorV2
    {
        namespace ObjectType
        {
            // DO NOT CHANGE ! These values are saved in the save file.
            static const uint32 ROOT       = 0; // deprecated
            static const uint32 NONE       = 1;
            static const uint32 GROUP      = 2;
            static const uint32 ELLIPSE    = 3;
            static const uint32 RECTANGLE  = 4;
            static const uint32 PATH       = 5;
            static const uint32 GROUPPAINT = 6;
        }

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
                        static const uint32 CHUNK_OBJECT_NAME =  0x5dbafbd0; // FString(Name)
                        static const uint32 CHUNK_OBJECT_TRANSFORM = 0x3bf02661; // container
                            static const uint32 CHUNK_OBJECT_TRANSFORM_TRANSLATION = 0x621d21c; // double(X)-double(Y)
                            static const uint32 CHUNK_OBJECT_TRANSFORM_ROTATION = 0xbf4d725c; // double(A)
                            static const uint32 CHUNK_OBJECT_TRANSFORM_SCALING = 0x4cc21f6d; // double(X)-double(Y)
                        static const uint32 CHUNK_OBJECT_OPACITY =  0x93792183; // double(opacity)
                        static const uint32 CHUNK_OBJECT_EXPANSION = 0x02e0858e; // uint32(bool)
                        static const uint32 CHUNK_OBJECT_FOREGROUNDCOLOR = 0xd490269b; // uint8(R)-uint8(G)-uint8(B)-uint8(A)
                        static const uint32 CHUNK_OBJECT_BACKGROUNDCOLOR = 0xd9c2a698; // uint8(R)-uint8(G)-uint8(B)-uint8(A)

                        static const uint32 CHUNK_OBJECT_FOREGROUNDBUCKET = 0x200f1262; // container
                            //static const uint32 CHUNK_BUCKET_ENTRY = ... // see CHUNK_BUCKET_ENTRY
                        static const uint32 CHUNK_OBJECT_BACKGROUNDBUCKET = 0x1816973a; // container
                            //static const uint32 CHUNK_BUCKET_ENTRY = ... // see CHUNK_BUCKET_ENTRY

                    static const uint32 CHUNK_PATH = 0x138d0bec; // container
                        // ... inherited chunks.
                        static const uint32 CHUNK_PATH_JOINT = 0x272518f6; // uint32(type)
                        static const uint32 CHUNK_PATH_MITERLIMIT = 0x9c7e37dd; // double(MiterLimit)
                        static const uint32 CHUNK_PATH_GEOMETRY = 0x904a4229; // container
                            // packed version
                            static const uint32 CHUNK_PATH_GEOMETRY_VERTICES = 0x1116a85d; // uint32(count), array[double(X)-double(Y)-double(Radius)]
                            // per-vertex version. Slower but more convenient to handle
                            static const uint32 CHUNK_PATH_GEOMETRY_VERTEX =  0x1f128031; // container
                                static const uint32 CHUNK_PATH_GEOMETRY_VERTEX_POSITION = 0xc66f513b; // double(X)-double(Y)-double(Radius)
                                static const uint32 CHUNK_PATH_GEOMETRY_VERTEX_HANDLEALIGNMENT = 0x285aa8d4; // uint32(bool)
                                static const uint32 CHUNK_PATH_GEOMETRY_VERTEX_LOCK =  0x4fdf582c; // uint32(bool)

                            static const uint32 CHUNK_PATH_GEOMETRY_CUBICSEGMENTS = 0x76cacf19; // uint32(count), array[uint32(P0ID)-uint32(P1ID)-double(CX0)-double(CY0)-double(CX1)-double(CY1)]
                        static const uint32 CHUNK_PATH_BRUSH =  0x4b77b632; // container
                            static const uint32 CHUNK_BRUSH =  0xd697217f; // container
                                static const uint32 CHUNK_BRUSH_TEXTURE =  0xd3070f03; // FName(path)
                                static const uint32 CHUNK_BRUSH_COLORFROMBRUSH =  0xc42013ad; // uint32(bool)
                                static const uint32 CHUNK_BRUSH_EXTENSIONMODE =  0x6965cf17; // uint32
                                static const uint32 CHUNK_BRUSH_REVERT = 0x53d1e69f; // uint32(bool)

                    static const uint32 CHUNK_GROUP = 0x4af0b779; // container
                        // ... +inherited chunks.

                    static const uint32 CHUNK_GROUPPAINT = 0xac92b85d; // container
                        // ... +inherited chunks.
                        static const uint32 CHUNK_GROUPPAINT_PAINTED = 0x89854E17; // uint32(bool)
                        static const uint32 CHUNK_GROUPPAINT_INTERSECTSCANEVAS = 0x8c0632f2; // uint32(bool)
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
                                static const uint32 CHUNK_BUCKET_PALETTEENTRY_MK1 = 0xac23cabe; // FName to load PaletteEntry
                                static const uint32 CHUNK_BUCKET_PALETTEENTRY_MK2 = 0x116eaba8; // FString to load PaletteEntry
                                static const uint32 CHUNK_BUCKET_SOLIDCOLOR = 0xd951af3c; // uint8(R)-uint8(G)-uint8(B)-uint8(A)
                                static const uint32 CHUNK_BUCKET_GRADIENT = 0x5d7bbd93; // container
                                    static const uint32 CHUNK_BUCKET_GRADIENT_STOP = 0xd0dfb9d3; // uint8(R)-uint8(G)-uint8(B)-uint8(A)-float(stop)
                                    static const uint32 CHUNK_BUCKET_GRADIENT_RADIALRADIUS = 0xcdbd0ebb; // double(radius)
                                    static const uint32 CHUNK_BUCKET_GRADIENT_RADIALOFFSET = 0xafcbaa31; // double(x),double(y)

                    static const uint32 CHUNK_SCENE =  0xfe491c66; // container
                        // ... +inherited chunks.

    }

    // chunk IDs for module OdysseyAnimation
    namespace Animation
    {
        // constants were initially computed from the CRC32 checksum of the constant's name, even though the constant's name may have changed over time
        // Just be sure the Chunk ID is unique and any ID will make it.
        // You can use website https://crc32.online/ to generate a code
        static const uint32 CHUNK_CELL =  0xf93591b3; // container
            static const uint32 CHUNK_CELL_LENGTH = 0x5dbe40b7; // uint32 (Length)
            static const uint32 CHUNK_CELL_MARKID = 0x866d04a6; // FString (MarkId)

        static const uint32 CHUNK_CELLIMAGEVECTOR =  0x60fdee3a; // container
            static const uint32 CHUNK_CELLIMAGEVECTOR_RESOLUTION = 0xc1bb48e1; // uint32(Width)-uint32(Height)
            static const uint32 CHUNK_CELLIMAGEVECTOR_BLOCK = 0xf0a9e121; // container
                static const uint32 CHUNK_CELLIMAGEVECTOR_BLOCK_ID =  0x194b349c; // FGuid(VectorBlockId)
            //static const uint32 CHUNK_VECTOR_MAGIC_V2 //see FOdysseyFile::VectorV2::CHUNK_VECTOR_MAGIC_V2

        static const uint32 CHUNK_CELLIMAGERASTER =  0xa37598ec; // container
            static const uint32 CHUNK_CELLIMAGERASTER_RASTERBLOCK = 0xf83974c6; // FOdysseyRasterBlock(Block)

        static const uint32 CHUNK_CELLIMAGESTAGGER = 0x559ff069; // container
            static const uint32 CHUNK_CELLIMAGESTAGGER_BEHAVIOUR = 0xdd0542e9; // uint32 (Behaviour)
            static const uint32 CHUNK_CELLIMAGESTAGGER_REACH = 0x98930ed0; // uint32 (Reach)

        static const uint32 CHUNK_CELLSCONTAINER =  0x5024f733; // container
            static const uint32 CHUNK_CELLSCONTAINER_OFFSET =  0x3e91fadd; // uint32 (offset)
            static const uint32 CHUNK_CELLSCONTAINER_CELLS =  0x13bb0a63; // container
                static const uint32 CHUNK_CELLSCONTAINER_CELLTYPE =  0x513beb3b; // FName(CellType)
                static const uint32 CHUNK_CELLSCONTAINER_CELL =  0xd8f8456d; // FOdysseyAnimationCell(Cell)

        static const uint32 CHUNK_LAYERIMAGERASTER = 0xc19199d8; //container
            static const uint32 CHUNK_LAYERIMAGERASTER_CELLSCONTAINER = 0x8334465; //FOdysseyAnimationCellsContainer(CellContainer)
            static const uint32 CHUNK_LAYERIMAGERASTER_LIGHTTABLE = 0x914a7195; //FOdysseyAnimationLightTable(LightTable)

        static const uint32 CHUNK_LAYERIMAGEVECTOR = 0x576c21b; //container
            static const uint32 CHUNK_LAYERIMAGEVECTOR_CELLSCONTAINER = 0x75494fd4; //FOdysseyAnimationCellsContainer(CellContainer)
            static const uint32 CHUNK_LAYERIMAGEVECTOR_LIGHTTABLE = 0x484ee3b4; //FOdysseyAnimationLightTable(LightTable)

        static const uint32 CHUNK_LIGHTTABLE = 0x5240eec2; //container
            static const uint32 CHUNK_LIGHTTABLE_DISPLAY_POSITION = 0x472555c; //EOdysseyLightTableDisplayPosition
            static const uint32 CHUNK_LIGHTTABLE_COLORS = 0x854e88aa; //EOdysseyLightTableDisplayPosition
            static const uint32 CHUNK_LIGHTTABLE_CONTRAST = 0x4e98233c; //FLinearColor(PreviousKeysColor) + FLinearColor(NextKeysColor)
            static const uint32 CHUNK_LIGHTTABLE_KEYS = 0x825ce6e5; //20x FOdysseyAnimationLightTableKey
                static const uint32 CHUNK_LIGHTTABLE_KEY = 0x5b785423; //FOdysseyAnimationLightTableKey
                    static const uint32 CHUNK_LIGHTTABLE_KEY_ISACTIVATED = 0xea8a5d86; //bool
                    static const uint32 CHUNK_LIGHTTABLE_KEY_OPACITY = 0x21ab66; //float

    }

    namespace RasterBlock
    {
        static const uint32 CHUNK_RASTERBLOCK = 0x97c86c9c; //container
            static const uint32 CHUNK_RASTERBLOCK_ID = 0x6055ee70; //FGuid(Id)
            static const uint32 CHUNK_RASTERBLOCK_RESOLUTION = 0xab8b7918; //uint32(Width) + uint32(Height)
            static const uint32 CHUNK_RASTERBLOCK_FORMAT = 0x79ddfbb4; //uint32(Format)
            static const uint32 CHUNK_RASTERBLOCK_BULKDATA = 0xe85808c4; //FEditorBulkData(BulkData)
    }

    namespace Texture
    {
        // constants were initially computed from the CRC32 checksum of the constant's name, even though the constant's name may have changed over time
        // Just be sure the Chunk ID is unique and any ID will make it.
        // You can use website https://crc32.online/ to generate a code
        static const uint32 CHUNK_TEXTURELAYERIMAGEVECTOR =  0x42c274ca; // container
            //static const uint32 CHUNK_VECTOR_MAGIC_V2 //see FOdysseyFile::VectorV2::CHUNK_VECTOR_MAGIC_V2

        static const uint32 CHUNK_TEXTURELAYERIMAGERASTER =  0x86252f09; // container
            static const uint32 CHUNK_TEXTURELAYERIMAGERASTER_RASTERBLOCK = 0x2fcd2a9a; // FOdysseyRasterBlock(Block)
    }

    void ODYSSEYFILE_API WriteChunk( uint32 iChunkID, FArchive &Ar, std::function<void(FArchive &Ar)> iCallback );
    void ODYSSEYFILE_API ReadChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback );
}
