#include "Export/OdysseyVectorExport.h"

static void
WriteGroupPaintBuckets( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    if ( iPaintGroup.GetBucketList().size() )
    {
        FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_GROUPPAINT_BUCKETS
                                        , Ar
                                        , [&iPaintGroup](FArchive &Ar) -> void
        {
            for( std::list<FOdysseyVectorBucket*>::iterator it = iPaintGroup.GetBucketList().begin(); it != iPaintGroup.GetBucketList().end(); ++it )
            {
                FOdysseyVectorBucket* bucket = static_cast<FOdysseyVectorBucket*>(*it);

                FOdysseyVectorExport::WriteBucket( *bucket, Ar );
            }
        } );
    }
}

static void
WriteGroupPaintGapTolerance( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_GROUPPAINT_GAP_TOLERANCE
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        double gapTolerance = iPaintGroup.GetGapTolerance();

        Ar << gapTolerance;
    } );
}

static void
WriteGroupPaintGap( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_GROUPPAINT_GAP
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        WriteGroupPaintGapTolerance( iPaintGroup, Ar );
    } );
}

static void
WriteGroupPaintWireframe( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_GROUPPAINT_WIREFRAME
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 wireframe = static_cast<uint32>(iPaintGroup.IsWireframe());

        Ar << wireframe;
    } );
}

void
FOdysseyVectorExport::WriteObjectGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECT_GROUPPAINT
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        WriteGroupPaintWireframe( iPaintGroup, Ar );
        WriteGroupPaintGap( iPaintGroup, Ar );
        WriteGroupPaintBuckets( iPaintGroup, Ar );
    } );
}
