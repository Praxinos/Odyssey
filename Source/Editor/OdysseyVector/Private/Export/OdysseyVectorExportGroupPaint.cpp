#include "Export/OdysseyVectorExport.h"
#include "OdysseyVectorSegmentCubic.h"

static void
WriteBucketPosition( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_POSITION
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        ::ULIS::FVec2D position = iBucket.GetCoords();

        Ar << position.x;
        Ar << position.y;
    } );
}

static void
WriteBucketGradientHandlePosition( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_GRADIENT_HANDLE_POSITION
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        ::ULIS::FVec2D& position = iBucket.GetHandle()->GetCoords();

        Ar << position.x;
        Ar << position.y;
    } );
}

static void
WriteBucketGradientHandle( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_GRADIENT_HANDLE
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        WriteBucketGradientHandlePosition( iBucket, Ar );
    } );
}

static void
WriteBucketGradientStop( FColor& iStopColor, double iStopAt, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_GRADIENT_STOP
                                    , Ar
                                    , [&iStopColor,iStopAt](FArchive &Ar) -> void
    {
        Ar << iStopColor.R;
        Ar << iStopColor.G;
        Ar << iStopColor.B;
        Ar << iStopColor.A;

        Ar << (double) iStopAt;
    } );
}

static void
WriteBucketGradient( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_GRADIENT
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        FColor gradientColor0 = iBucket.GetGradientColor0();
        FColor gradientColor1 = iBucket.GetGradientColor1();

        WriteBucketGradientHandle( iBucket, Ar );
        WriteBucketGradientStop( gradientColor0, 0.0f, Ar );
        WriteBucketGradientStop( gradientColor1, 1.0f, Ar );
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

        if( iBucket.IsGradient() == true )
        {
            WriteBucketGradient( iBucket, Ar );
        }

        if( iBucket.IsGradient() == false )
        {
            WriteBucketColor( iBucket, Ar );
        }
    } );
}

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

                WriteBucketEntry( *bucket, Ar );
            }
        } );
    }
}

void
FOdysseyVectorExport::WriteObjectGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECT_GROUPPAINT
                                    , Ar
                                    , [&iPaintGroup](FArchive &Ar) -> void
    {
        WriteGroupPaintBuckets( iPaintGroup, Ar );
    } );
}
