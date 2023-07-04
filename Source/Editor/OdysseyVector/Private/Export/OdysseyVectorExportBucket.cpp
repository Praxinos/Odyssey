#include "Export/OdysseyVectorExport.h"

static void
WriteBucketPaletteEntryDescriptionEntryId( FOdysseyVectorBucket& iBucket, FArchive& Ar)
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_PALETTEENTRYDESCRIPTION_ENTRYID
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        FName idEntry = iBucket.GetPaletteEntryDescription().EntryId;

        Ar << idEntry;
    } );
}

static void
WriteBucketPaletteEntryDescriptionUsedSet( FOdysseyVectorBucket& iBucket, FArchive& Ar)
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_PALETTEENTRYDESCRIPTION_USEDSET
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        uint8 usedSet = iBucket.GetPaletteEntryDescription().UsedSet;

        Ar << usedSet;
    } );
}

static void
WriteBucketPaletteEntryDescription( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_PALETTEENTRYDESCRIPTION
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        WriteBucketPaletteEntryDescriptionEntryId( iBucket, Ar );
        WriteBucketPaletteEntryDescriptionUsedSet( iBucket, Ar );
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

        Ar << fillColor.R;
        Ar << fillColor.G;
        Ar << fillColor.B;
        Ar << fillColor.A;
    } );
}

void
FOdysseyVectorExport::WriteBucket( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_BUCKET_ENTRY
                                    , Ar
                                    , [&iBucket](FArchive &Ar) -> void
    {
        WriteBucketPaletteEntryDescription( iBucket, Ar );
        WriteBucketPosition( iBucket, Ar );
        WriteBucketRotation( iBucket, Ar );
        WriteBucketPropagated( iBucket, Ar );

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
