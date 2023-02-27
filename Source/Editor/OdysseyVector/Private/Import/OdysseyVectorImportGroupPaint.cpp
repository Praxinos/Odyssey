#include "Import/OdysseyVectorImport.h"


static void
ReadBucketEntry( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&iBucket](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyVectorExport::CHUNK_BUCKET_POSITION:
                {
                    double x;
                    double y;

                    Ar << x;
                    Ar << y;

                    iBucket.SetCoords( x, y );
                }
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_COLOR:
                {
                    uint8 R;
                    uint8 G;
                    uint8 B;
                    uint8 A;

                    Ar << R;
                    Ar << G;
                    Ar << B;
                    Ar << A;

                    iBucket.SetColor( R, G, B, A );
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}

void
FOdysseyVectorImport::ReadObjectGroupPaint( UOdysseyVectorGroupPaint& iPaintGroup, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&iPaintGroup](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyVectorExport::CHUNK_GROUPPAINT_BUCKETS:
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_ENTRY:
                {
                    FOdysseyVectorBucket* bucket = new FOdysseyVectorBucket( iPaintGroup, 0.0f, 0.0f );

                    iPaintGroup.AddBucket( bucket );

                    ReadBucketEntry( *bucket, Ar.Tell() + iChunkLen, Ar );
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
