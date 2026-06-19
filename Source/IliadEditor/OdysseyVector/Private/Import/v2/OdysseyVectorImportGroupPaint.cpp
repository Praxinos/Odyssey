// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Import/v2/OdysseyVectorImport.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorBucket.h"
#include "OdysseyVectorGroupPaint.h"

void
FOdysseyVectorImportV2::ParseGroupPaintChunks( FOdysseyVectorGroupPaint& iPaintGroup
                                             , uint32 iChunkID
                                             , uint64 iChunkLen
                                             , FArchive &Ar )
{
    switch( iChunkID )
    {
        case FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_BUCKETS:
        break;

        case FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_WIREFRAME:
        {
            uint32 wireframe;

            Ar << wireframe;

            iPaintGroup.SetWireframe( wireframe ? true : false );
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_WIREFRAMECOLOR:
        {
            uint8 R, G, B, A;

            Ar << R;
            Ar << G;
            Ar << B;
            Ar << A;

            iPaintGroup.SetWireframeColor( R, G, B, A );
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_MONOCHROME:
        {
            uint32 monochrome;

            Ar << monochrome;

            iPaintGroup.SetMonochrome( monochrome ? true : false );
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_MONOCHROMECOLOR:
        {
            uint8 R, G, B, A;

            Ar << R;
            Ar << G;
            Ar << B;
            Ar << A;

            iPaintGroup.SetMonochromeColor( R, G, B, A );
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_PAINTED:
        {
            uint32 painted;

            Ar << painted;

            iPaintGroup.SetPainted( painted ? true : false );
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_INTERSECTSCANVAS:
        {
            uint32 intersectsCanvas;

            Ar << intersectsCanvas;

            iPaintGroup.SetIntersectsCanvas( intersectsCanvas ? true : false );
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_GAP:
        break;

        case FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_GAP_TOLERANCE:
        {
            double gapTolerance;

            Ar << gapTolerance;

            iPaintGroup.SetGapTolerance( gapTolerance );
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_GAP_DETECTIONSCHEME:
        {
            uint32 gapDetectionScheme;

            Ar << gapDetectionScheme;

            iPaintGroup.SetGapDetectionScheme( static_cast<eGapDetectionScheme>(gapDetectionScheme) );
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_GAP_SEGMENTEXTENSION_SCHEME:
        {
            uint32 segmentExtensionScheme;

            Ar << segmentExtensionScheme;

            iPaintGroup.SetSegmentExtensionScheme( static_cast<eSegmentExtensionScheme>(segmentExtensionScheme) );
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_GROUPPAINT_GAP_SEGMENTEXTENSION_SIMPLIFIED:
        {
            uint32 simplified;

            Ar << simplified;

            iPaintGroup.SetSegmentExtensionSimplified( simplified ? true : false );
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_BUCKET_ENTRY:
        {
            FOdysseyVectorBucket* bucket = new FOdysseyVectorBucket( &iPaintGroup, 0.0f, 0.0f, false );

            iPaintGroup.AddBucket( bucket );

            ReadBucket( *bucket, Ar.Tell() + iChunkLen, Ar );
        }
        break;

        default:
            FOdysseyVectorImportV2::ParseGroupChunks( iPaintGroup, iChunkID, iChunkLen, Ar );
        break;
    }
}

void
FOdysseyVectorImportV2::ReadGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, uint64 iChunkEnd, FArchive &Ar )
{
    // for legacy reason, we set some default values that are different than the object's default value at constructor call.
   iPaintGroup.SetGapDetectionScheme( eGapDetectionScheme::ClosestNeighbour );

    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [this,&iPaintGroup](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            ParseGroupPaintChunks( iPaintGroup, iChunkID, iChunkLen, Ar );
        } );
}
