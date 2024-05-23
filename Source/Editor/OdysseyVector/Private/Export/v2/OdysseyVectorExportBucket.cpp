#include "Export/v2/OdysseyVectorExport.h"
#include "Palette/OdysseyPaletteEntry.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorBucket.h"

void
FOdysseyVectorExportV2::WriteBucketColorMode( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BUCKET_COLORMODE
                            , Ar
                            , [&iBucket](FArchive &Ar) -> void
    {
        uint32 colorMode = static_cast<uint32>( iBucket.GetColorMode());

        Ar << colorMode;
    } );
}


void
FOdysseyVectorExportV2::WriteBucketPaletteEntryMark2( FOdysseyVectorBucket& iBucket, FArchive& Ar)
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BUCKET_PALETTEENTRY_MK2
                            , Ar
                            , [&iBucket](FArchive &Ar) -> void
    {
        FString assetName = FSoftObjectPath( iBucket.GetPaletteEntry() ).ToString();

        Ar << assetName;
    } );
}

// unused. Kept as history
void
FOdysseyVectorExportV2::WriteBucketPaletteEntryMark1( FOdysseyVectorBucket& iBucket, FArchive& Ar)
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BUCKET_PALETTEENTRY_MK1
                            , Ar
                            , [&iBucket](FArchive &Ar) -> void
    {
        FName idEntry = iBucket.GetPaletteEntry() ? iBucket.GetPaletteEntry()->GetFName() : FName();

        Ar << idEntry;
    } );
}

void
FOdysseyVectorExportV2::WriteBucketPropagated( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BUCKET_PROPAGATED
                            , Ar
                            , [&iBucket](FArchive &Ar) -> void
    {
        uint32 propagated = iBucket.IsPropagated() ? 1 : 0;

        Ar << propagated;
    } );
}

void
FOdysseyVectorExportV2::WriteBucketSpreading( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BUCKET_SPREADING
                            , Ar
                            , [&iBucket](FArchive &Ar) -> void
    {
        uint32 spreadingPolicy = static_cast<uint32>(iBucket.GetSpreadingPolicy());

        Ar << spreadingPolicy;
    } );
}

void
FOdysseyVectorExportV2::WriteBucketPosition( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BUCKET_POSITION
                            , Ar
                            , [&iBucket](FArchive &Ar) -> void
    {
        ::ULIS::FVec2D position = iBucket.GetCoords();

        Ar << position.x;
        Ar << position.y;
    } );
}

void
FOdysseyVectorExportV2::WriteBucketRotation( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BUCKET_ROTATION
                            , Ar
                            , [&iBucket](FArchive &Ar) -> void
    {
        double rotation = iBucket.GetRotation();

        Ar << rotation;
    } );
}

void
FOdysseyVectorExportV2::WriteBucketGradientRadialRadius( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BUCKET_GRADIENT_RADIALRADIUS
                            , Ar
                            , [&iBucket](FArchive &Ar) -> void
    {
        double radialRadius = iBucket.GetRadialRadius();

        Ar << radialRadius;
    } );
}

void
FOdysseyVectorExportV2::WriteBucketGradientRadialOffset( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BUCKET_GRADIENT_RADIALOFFSET
                            , Ar
                            , [&iBucket](FArchive &Ar) -> void
    {
        ::ULIS::FVec2D& radialOffset = iBucket.GetRadialOffset();

        Ar << radialOffset.x;
        Ar << radialOffset.y;
    } );
}

void
FOdysseyVectorExportV2::WriteBucketGradientStop( FColor& iStopColor, double iStopAt, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BUCKET_GRADIENT_STOP
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

void
FOdysseyVectorExportV2::WriteBucketGradient( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BUCKET_GRADIENT
                            , Ar
                            , [&iBucket](FArchive &Ar) -> void
    {
        FColor gradientColor0 = iBucket.GetGradientColor0();
        FColor gradientColor1 = iBucket.GetGradientColor1();

        WriteBucketGradientStop( gradientColor0, 0.0f, Ar );
        WriteBucketGradientStop( gradientColor1, 1.0f, Ar );
        WriteBucketGradientRadialRadius( iBucket, Ar );
        WriteBucketGradientRadialOffset( iBucket, Ar );
    } );
}

void
FOdysseyVectorExportV2::WriteBucketColor( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BUCKET_SOLIDCOLOR
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
FOdysseyVectorExportV2::WriteBucket( FOdysseyVectorBucket& iBucket, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BUCKET_ENTRY
                            , Ar
                            , [&iBucket](FArchive &Ar) -> void
    {
        WriteBucketSpreading( iBucket, Ar );
        WriteBucketPosition( iBucket, Ar );
        WriteBucketRotation( iBucket, Ar );
        WriteBucketPropagated( iBucket, Ar );

        if( iBucket.GetColorMode() == eBucketColorMode::Palette )
        {
            WriteBucketPaletteEntryMark2( iBucket, Ar );
        }

        WriteBucketGradient( iBucket, Ar );
        WriteBucketColor( iBucket, Ar );

        WriteBucketColorMode( iBucket, Ar );
    } );
}
