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
                case FOdysseyVectorExport::CHUNK_BUCKET_PROPAGATED:
                {
                    uint32 propagated;

                    Ar << propagated;

                    iBucket.SetPropagated( propagated ? true : false );
                }
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_ROTATION:
                {
                    double rotation;

                    Ar << rotation;

                    iBucket.SetRotation( rotation );
                }
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_POSITION:
                {
                    double x;
                    double y;

                    Ar << x;
                    Ar << y;

                    iBucket.Set( x, y );
                }
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_SOLIDCOLOR:
                {
                    uint8 R, G, B, A;

                    Ar << R;
                    Ar << G;
                    Ar << B;
                    Ar << A;

                    iBucket.SetGradient(false);
                    iBucket.SetSolidColor( R, G, B, A );
                }
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_GRADIENT: // container
                    iBucket.SetGradient( true );
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_GRADIENT_STOP:
                {
                    uint8 R,G,B,A;
                    double stopAt;

                    Ar << R;
                    Ar << G;
                    Ar << B;
                    Ar << A;

                    Ar << stopAt;

                    if( stopAt == 0.0f ) iBucket.SetGradientColor0( R, G, B, A );
                    if( stopAt == 1.0f ) iBucket.SetGradientColor1( R, G, B, A );
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
FOdysseyVectorImport::ReadObjectGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&iPaintGroup](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyVectorExport::CHUNK_GROUPPAINT_BUCKETS:
                break;

                case FOdysseyVectorExport::CHUNK_GROUPPAINT_WIREFRAME:
                {
                    uint32 wireframe;

                    Ar << wireframe;

                    iPaintGroup.SetWireframe( wireframe ? true : false );
                }
                break;

                case FOdysseyVectorExport::CHUNK_GROUPPAINT_GAP:
                break;

                case FOdysseyVectorExport::CHUNK_GROUPPAINT_GAP_TOLERANCE:
                {
                    double gapTolerance;

                    Ar << gapTolerance;

                    iPaintGroup.SetGapTolerance( gapTolerance );
                }
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_ENTRY:
                {
                    FOdysseyVectorBucket* bucket = new FOdysseyVectorBucket( iPaintGroup, 0.0f, 0.0f, false );

                    iPaintGroup.AddBucket( bucket );

                    ReadBucket( *bucket, Ar.Tell() + iChunkLen, Ar );
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
