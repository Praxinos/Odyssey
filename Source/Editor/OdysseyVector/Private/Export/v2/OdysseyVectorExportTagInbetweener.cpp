#include "Export/v2/OdysseyVectorExport.h"
#include "Palette/OdysseyPaletteEntry.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorTagInbetweener.h"

void
FOdysseyVectorExportV2::WriteTagInbetweenerFFDGridGeometry( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAG_INBETWEENER_FFDGRID_GEOMETRY
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        std::vector<FInbetweenerPoint>& gridPointbuffer = iInbetweenerTag.GetGridPointBuffer();

        for( FInbetweenerPoint& point : gridPointbuffer )
        {
            double sourceX = point.sourcePosition.x;
            double sourceY = point.sourcePosition.y;
            double targetX = point.targetPosition.x;
            double targetY = point.targetPosition.y;

            Ar << sourceX;
            Ar << sourceY;
            Ar << targetX;
            Ar << targetY;
        }
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerFFDGridSize( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAG_INBETWEENER_FFDGRID_SIZE
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 numCellX = iInbetweenerTag.GetFFDNumCellX();
        uint32 numCellY = iInbetweenerTag.GetFFDNumCellY();

        Ar << numCellX;
        Ar << numCellY;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerFFDGrid( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                  , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_FFDGRID
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        WriteTagInbetweenerFFDGridSize( iInbetweenerTag, Ar );
        WriteTagInbetweenerFFDGridGeometry( iInbetweenerTag, Ar );
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerChart( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_CHART
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        for( FInbetweenerInbetween& inbetween : iInbetweenerTag.GetChart().inbetweenBuffer )
        {
            float spacing = inbetween.spacing;

            Ar << spacing;
        }
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerInbetweenCount( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                         , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_INBETWEENCOUNT
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 inbetweenCount = iInbetweenerTag.GetInbetweenCount();

        Ar << inbetweenCount;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweener( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        WriteTagInbetweenerInbetweenCount( iInbetweenerTag, Ar );
        WriteTagInbetweenerChart( iInbetweenerTag, Ar );

        if( iInbetweenerTag.GetGridType() == eInbetweenerGridType::FFD )
        {
            WriteTagInbetweenerFFDGrid( iInbetweenerTag, Ar );
        }
    } );
}
