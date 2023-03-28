#include "Export/OdysseyVectorExport.h"
#include "OdysseyVectorSegmentCubic.h"

static void
WriteEllipseGeometryRadius( FOdysseyVectorEllipse& iCircle, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_ELLIPSE_GEOMETRY_RADIUS
                                    , Ar
                                    , [&iCircle](FArchive &Ar) -> void
    {
        double radiusX = iCircle.GetRadiusX();
        double radiusY = iCircle.GetRadiusY();

        Ar << radiusX;
        Ar << radiusY;
    } );
}

static void
WriteEllipseGeometry( FOdysseyVectorEllipse& iCircle, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_ELLIPSE_GEOMETRY
                                    , Ar
                                    , [&iCircle](FArchive &Ar) -> void
    {
        WriteEllipseGeometryRadius( iCircle, Ar );
    } );
}

static void
WriteEllipseFilled( FOdysseyVectorEllipse& iCircle, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_ELLIPSE_FILLED
                                    , Ar
                                    , [&iCircle](FArchive &Ar) -> void
    {
        uint32 filled = iCircle.IsFilled() ? 1 : 0;

        Ar << filled;
    } );
}

void
FOdysseyVectorExport::WriteObjectEllipse( FOdysseyVectorEllipse& iCircle, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECT_ELLIPSE
                                    , Ar
                                    , [&iCircle](FArchive &Ar) -> void
    {
        WriteEllipseFilled( iCircle, Ar );
        WriteEllipseGeometry( iCircle, Ar );
    } );
}
