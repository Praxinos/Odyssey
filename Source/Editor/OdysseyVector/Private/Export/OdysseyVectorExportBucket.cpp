#include "Export/OdysseyVectorExport.h"
#include "Palette/OdysseyPaletteEntry.h"

static void
WriteBucketPaletteEntry( FOdysseyVectorBucket& iBucket, FArchive& Ar)
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_PALETTEENTRY
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        FName idEntry = iBucket.GetPaletteEntry() ? iBucket.GetPaletteEntry()->GetFName() : FName();

        Ar << idEntry;
    } );
}

static void
WriteBucketPropagated( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_PROPAGATED
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        uint32 propagated = iBucket.IsPropagated() ? 1 : 0;

        Ar << propagated;
    } );
}

static void
WriteBucketSpreading( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_SPREADING
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        uint32 spreadingPolicy = static_cast<uint32>(iBucket.GetSpreadingPolicy());

        Ar << spreadingPolicy;
    } );
}

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
WriteBucketRotation( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_ROTATION
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        double rotation = iBucket.GetRotation();

        Ar << rotation;
    } );
}

static void
WriteBucketGradientStop( FColor& iStopColor, double iStopAt, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_GRADIENT_STOP
                                    , Ar
                                    , [&iStopColor,iStopAt](FArchive &Ar) -> void
    {
        double stopAt = iStopAt; //iStopAt is const in the lambda, and FArchive << doesn't like it, so we use a variable here
        uint8 R = iStopColor.R;
        uint8 G = iStopColor.G;
        uint8 B = iStopColor.B;
        uint8 A = iStopColor.A;

        Ar << R;
        Ar << G;
        Ar << B;
        Ar << A;
        Ar << stopAt;
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

        WriteBucketGradientStop( gradientColor0, 0.0f, Ar );
        WriteBucketGradientStop( gradientColor1, 1.0f, Ar );
    } );
}

static void
WriteBucketColor( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_SOLIDCOLOR
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        FColor fillColor = iBucket.GetSolidColor();
        uint8 R = fillColor.R;
        uint8 G = fillColor.G;
        uint8 B = fillColor.B;
        uint8 A = fillColor.A;

        Ar << R;
        Ar << G;
        Ar << B;
        Ar << A;
    } );
}

void
FOdysseyVectorExport::WriteBucket( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_ENTRY
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        WriteBucketSpreading( iBucket, Ar );
        WriteBucketPaletteEntry( iBucket, Ar );
        WriteBucketPosition( iBucket, Ar );
        WriteBucketRotation( iBucket, Ar );
        WriteBucketPropagated( iBucket, Ar );

        if( iBucket.GetColorMode() == eBucketColorMode::LinearGradient )
        {
            WriteBucketGradient( iBucket, Ar );
        }

        if( iBucket.GetColorMode() ==  eBucketColorMode::SolidColor  )
        {
            WriteBucketColor( iBucket, Ar );
        }
    } );
}
