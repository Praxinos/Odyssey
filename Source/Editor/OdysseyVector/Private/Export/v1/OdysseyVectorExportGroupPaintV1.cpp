#include "Export/v1/OdysseyVectorExport.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorGroupPaint.h"

void
FOdysseyVectorExportV1::WriteGroupPaintBuckets( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    if ( iPaintGroup.GetBucketList().size() )
    {
        FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_GROUPPAINT_BUCKETS
                                , Ar
                                , [&iPaintGroup](FArchive &Ar) -> void
        {
            for( FOdysseyVectorBucket* bucket : iPaintGroup.GetBucketList() )
            {
                FOdysseyVectorExportV1::WriteBucket( *bucket, Ar );
            }
        } );
    }
}

void
FOdysseyVectorExportV1::WriteGroupPaintGapTolerance( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_GROUPPAINT_GAP_TOLERANCE
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        double gapTolerance = iPaintGroup.GetGapTolerance();

        Ar << gapTolerance;
    } );
}

void
FOdysseyVectorExportV1::WriteGroupPaintGap( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_GROUPPAINT_GAP
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        WriteGroupPaintGapTolerance( iPaintGroup, Ar );
    } );
}

void
FOdysseyVectorExportV1::WriteGroupPaintMonochromeColor( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_GROUPPAINT_MONOCHROMECOLOR
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint8 R, G, B, A;

        iPaintGroup.GetMonochromeColor( R, G, B, A );

        Ar << R;
        Ar << G;
        Ar << B;
        Ar << A;
    } );
}

void
FOdysseyVectorExportV1::WriteGroupPaintMonochrome( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_GROUPPAINT_MONOCHROME
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 monochrome = static_cast<uint32>(iPaintGroup.IsMonochrome());

        Ar << monochrome;
    } );
}

void
FOdysseyVectorExportV1::WriteGroupPaintWireframeColor( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_GROUPPAINT_WIREFRAMECOLOR
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint8 R, G, B, A;

        iPaintGroup.GetWireframeColor( R, G, B, A );

        Ar << R;
        Ar << G;
        Ar << B;
        Ar << A;
    } );
}

void
FOdysseyVectorExportV1::WriteGroupPaintWireframe( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_GROUPPAINT_WIREFRAME
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 wireframe = static_cast<uint32>(iPaintGroup.IsWireframe());

        Ar << wireframe;
    } );
}

void
FOdysseyVectorExportV1::WriteGroupPaintPainted( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_GROUPPAINT_PAINTED
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 painted = static_cast<uint32>(iPaintGroup.IsPainted());

        Ar << painted;
    } );
}

void
FOdysseyVectorExportV1::WriteGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_OBJECT_GROUPPAINT
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        WriteGroupPaintPainted( iPaintGroup, Ar );
        WriteGroupPaintMonochrome( iPaintGroup, Ar );
        WriteGroupPaintMonochromeColor( iPaintGroup, Ar );
        WriteGroupPaintWireframe( iPaintGroup, Ar );
        WriteGroupPaintWireframeColor( iPaintGroup, Ar );
        WriteGroupPaintGap( iPaintGroup, Ar );
        WriteGroupPaintBuckets( iPaintGroup, Ar );
    } );
}
