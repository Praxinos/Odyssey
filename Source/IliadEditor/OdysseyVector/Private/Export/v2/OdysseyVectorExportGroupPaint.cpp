// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Export/v2/OdysseyVectorExport.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorGroupPaint.h"

void
FOdysseyVectorExportV2::WriteGroupPaintBuckets( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    if ( iPaintGroup.GetBucketList().size() )
    {
        FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_BUCKETS
                                , Ar
                                , [&iPaintGroup](FArchive &Ar) -> void
        {
            for( FOdysseyVectorBucket* bucket : iPaintGroup.GetBucketList() )
            {
                FOdysseyVectorExportV2::WriteBucket( *bucket, Ar );
            }
        } );
    }
}

void
FOdysseyVectorExportV2::WriteGroupPaintGapSegmentExtensionSimplified( FOdysseyVectorGroupPaint& iPaintGroup
                                                                    , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_GAP_SEGMENTEXTENSION_SIMPLIFIED
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 simplified = static_cast<uint32>(iPaintGroup.IsSegmentExtensionSimplified());

        Ar << simplified;
    } );
}

void
FOdysseyVectorExportV2::WriteGroupPaintGapSegmentExtensionScheme( FOdysseyVectorGroupPaint& iPaintGroup
                                                                , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_GAP_SEGMENTEXTENSION_SCHEME
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 extensionScheme = static_cast<uint32>(iPaintGroup.GetSegmentExtensionScheme());

        Ar << extensionScheme;
    } );
}

void
FOdysseyVectorExportV2::WriteGroupPaintGapDetectionScheme( FOdysseyVectorGroupPaint& iPaintGroup
                                                         , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_GAP_DETECTIONSCHEME
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 gapDetectionScheme = static_cast<uint32>(iPaintGroup.GetGapDetectionScheme());

        Ar << gapDetectionScheme;
    } );
}

void
FOdysseyVectorExportV2::WriteGroupPaintGapTolerance( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_GAP_TOLERANCE
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
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_GAP
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        WriteGroupPaintGapTolerance( iPaintGroup, Ar );
    } );
}

void
FOdysseyVectorExportV2::WriteGroupPaintMonochromeColor( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_MONOCHROMECOLOR
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
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_MONOCHROME
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
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_WIREFRAMECOLOR
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
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_WIREFRAME
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
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_PAINTED
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 painted = static_cast<uint32>(iPaintGroup.IsPainted());

        Ar << painted;
    } );
}

void
FOdysseyVectorExportV2::WriteGroupPaintIntersectsCanevas( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_INTERSECTSCANVAS
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        uint32 intersectsCanevas = static_cast<uint32>(iPaintGroup.IntersectsCanvas());

        Ar << intersectsCanevas;
    } );
}

// Write chunks without encapsulation within the GroupPaint chunk header
void
FOdysseyVectorExportV2::WriteGroupPaintChunks( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    // inherited chunks
    WriteGroupChunks( iPaintGroup, Ar );
    // own chunks
    WriteGroupPaintPainted( iPaintGroup, Ar );
    WriteGroupPaintIntersectsCanevas( iPaintGroup, Ar );
    WriteGroupPaintMonochrome( iPaintGroup, Ar );
    WriteGroupPaintMonochromeColor( iPaintGroup, Ar );
    WriteGroupPaintWireframe( iPaintGroup, Ar );
    WriteGroupPaintWireframeColor( iPaintGroup, Ar );
    WriteGroupPaintGap( iPaintGroup, Ar );
    WriteGroupPaintBuckets( iPaintGroup, Ar );
    WriteGroupPaintGapDetectionScheme( iPaintGroup, Ar );
    WriteGroupPaintGapSegmentExtensionScheme( iPaintGroup, Ar );
    WriteGroupPaintGapSegmentExtensionSimplified( iPaintGroup, Ar );
}

// Write chunks with encapsulation within the GroupPaint chunk header
void
FOdysseyVectorExportV2::WriteGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUPPAINT
                            , Ar
                            , [&iPaintGroup](FArchive &Ar) -> void
    {
        WriteGroupPaintChunks( iPaintGroup, Ar );
    } );
}
