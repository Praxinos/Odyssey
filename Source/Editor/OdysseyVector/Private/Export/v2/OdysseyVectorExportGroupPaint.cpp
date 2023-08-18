#include "Export/v2/OdysseyVectorExport.h"

void
FOdysseyVectorExportV2::WriteGroupPaintBuckets( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    if ( iPaintGroup.GetBucketList().size() )
    {
        FOdysseyVectorExportV2::WriteChunk( FOdysseyVectorExportV2::CHUNK_GROUPPAINT_BUCKETS
                                        , Ar
                                        , [&iPaintGroup](FArchive &Ar) -> void
        {
            for( std::list<FOdysseyVectorBucket*>::iterator it = iPaintGroup.GetBucketList().begin(); it != iPaintGroup.GetBucketList().end(); ++it )
            {
                FOdysseyVectorBucket* bucket = static_cast<FOdysseyVectorBucket*>(*it);

                FOdysseyVectorExportV2::WriteBucket( *bucket, Ar );
            }
        } );
    }
}

void
FOdysseyVectorExportV2::WriteGroupPaintGapTolerance( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExportV2::WriteChunk( FOdysseyVectorExportV2::CHUNK_GROUPPAINT_GAP_TOLERANCE
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        double gapTolerance = iPaintGroup.GetGapTolerance();

        Ar << gapTolerance;
    } );
}

void
FOdysseyVectorExportV2::WriteGroupPaintGap( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExportV2::WriteChunk( FOdysseyVectorExportV2::CHUNK_GROUPPAINT_GAP
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        WriteGroupPaintGapTolerance( iPaintGroup, Ar );
    } );
}

void
FOdysseyVectorExportV2::WriteGroupPaintMonochromeColor( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExportV2::WriteChunk( FOdysseyVectorExportV2::CHUNK_GROUPPAINT_MONOCHROMECOLOR
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
FOdysseyVectorExportV2::WriteGroupPaintMonochrome( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExportV2::WriteChunk( FOdysseyVectorExportV2::CHUNK_GROUPPAINT_MONOCHROME
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 monochrome = static_cast<uint32>(iPaintGroup.IsMonochrome());

        Ar << monochrome;
    } );
}

void
FOdysseyVectorExportV2::WriteGroupPaintWireframeColor( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExportV2::WriteChunk( FOdysseyVectorExportV2::CHUNK_GROUPPAINT_WIREFRAMECOLOR
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
FOdysseyVectorExportV2::WriteGroupPaintWireframe( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExportV2::WriteChunk( FOdysseyVectorExportV2::CHUNK_GROUPPAINT_WIREFRAME
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 wireframe = static_cast<uint32>(iPaintGroup.IsWireframe());

        Ar << wireframe;
    } );
}

void
FOdysseyVectorExportV2::WriteGroupPaintPainted( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExportV2::WriteChunk( FOdysseyVectorExportV2::CHUNK_GROUPPAINT_PAINTED
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 painted = static_cast<uint32>(iPaintGroup.IsPainted());

        Ar << painted;
    } );
}

void
FOdysseyVectorExportV2::WriteGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExportV2::WriteChunk( FOdysseyVectorExportV2::CHUNK_GROUPPAINT
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        // inherited chunks
        WriteObjectChunks( iPaintGroup, Ar );
        // own chunks
        WriteGroupPaintPainted( iPaintGroup, Ar );
        WriteGroupPaintMonochrome( iPaintGroup, Ar );
        WriteGroupPaintMonochromeColor( iPaintGroup, Ar );
        WriteGroupPaintWireframe( iPaintGroup, Ar );
        WriteGroupPaintWireframeColor( iPaintGroup, Ar );
        WriteGroupPaintGap( iPaintGroup, Ar );
        WriteGroupPaintBuckets( iPaintGroup, Ar );
    } );
}
