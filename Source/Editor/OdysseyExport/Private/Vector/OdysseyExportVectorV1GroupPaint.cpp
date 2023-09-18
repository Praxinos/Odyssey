#include "Vector/OdysseyExportVectorV1.h"

void
FOdysseyExportVectorV1::WriteGroupPaintBuckets( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    if ( iPaintGroup.GetBucketList().size() )
    {
        FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_GROUPPAINT_BUCKETS
                                        , Ar
                                        , [&iPaintGroup](FArchive &Ar) -> void
        {
            for( std::list<FOdysseyVectorBucket*>::iterator it = iPaintGroup.GetBucketList().begin(); it != iPaintGroup.GetBucketList().end(); ++it )
            {
                FOdysseyVectorBucket* bucket = static_cast<FOdysseyVectorBucket*>(*it);

                FOdysseyExportVectorV1::WriteBucket( *bucket, Ar );
            }
        } );
    }
}

void
FOdysseyExportVectorV1::WriteGroupPaintGapTolerance( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_GROUPPAINT_GAP_TOLERANCE
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        double gapTolerance = iPaintGroup.GetGapTolerance();

        Ar << gapTolerance;
    } );
}

void
FOdysseyExportVectorV1::WriteGroupPaintGap( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_GROUPPAINT_GAP
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        WriteGroupPaintGapTolerance( iPaintGroup, Ar );
    } );
}

void
FOdysseyExportVectorV1::WriteGroupPaintMonochromeColor( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_GROUPPAINT_MONOCHROMECOLOR
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
FOdysseyExportVectorV1::WriteGroupPaintMonochrome( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_GROUPPAINT_MONOCHROME
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 monochrome = static_cast<uint32>(iPaintGroup.IsMonochrome());

        Ar << monochrome;
    } );
}

void
FOdysseyExportVectorV1::WriteGroupPaintWireframeColor( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_GROUPPAINT_WIREFRAMECOLOR
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
FOdysseyExportVectorV1::WriteGroupPaintWireframe( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_GROUPPAINT_WIREFRAME
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 wireframe = static_cast<uint32>(iPaintGroup.IsWireframe());

        Ar << wireframe;
    } );
}

void
FOdysseyExportVectorV1::WriteGroupPaintPainted( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_GROUPPAINT_PAINTED
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 painted = static_cast<uint32>(iPaintGroup.IsPainted());

        Ar << painted;
    } );
}

void
FOdysseyExportVectorV1::WriteGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_OBJECT_GROUPPAINT
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
