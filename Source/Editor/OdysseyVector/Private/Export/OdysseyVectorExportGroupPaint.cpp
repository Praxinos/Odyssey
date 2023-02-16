#include "Export/OdysseyVectorExport.h"
#include "OdysseyVectorSegmentCubic.h"

static void
WriteBucketPosition( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_POSITION
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        ::ULIS::FVec2D& position = iBucket.GetCoords();

        Ar << position.x;
        Ar << position.y;
    } );
}

static void
WriteBucketColor( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_COLOR
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        FColor fillColor = iBucket.GetColor();

        Ar << fillColor.R;
        Ar << fillColor.G;
        Ar << fillColor.B;
        Ar << fillColor.A;
    } );
}

static void
WriteBucketEntry( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_ENTRY
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        WriteBucketPosition( iBucket, Ar );
        WriteBucketColor( iBucket, Ar );
    } );
}

static void
WriteGroupPaintBuckets( UOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
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

                WriteBucketEntry( *bucket, Ar );
            }
        } );
    }
}

void
FOdysseyVectorExport::WriteObjectGroupPaint( UOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECT_GROUPPAINT
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        WriteGroupPaintBuckets( iPaintGroup, Ar );
    } );
}
