#include "Import/OdysseyVectorImport.h"

void
FOdysseyVectorImport::ReadObjectEllipse( UOdysseyVectorCircle& iCircle, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&iCircle](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyVectorExport::CHUNK_ELLIPSE_FILLED:
                {
                    uint32 filled;

                    Ar << filled;

                    iCircle.SetFilled( filled ? true : false );
                }
                break;

                case FOdysseyVectorExport::CHUNK_ELLIPSE_GEOMETRY:
                break;

                case FOdysseyVectorExport::CHUNK_ELLIPSE_GEOMETRY_RADIUS:
                {
                    double radiusX;
                    double radiusY;

                    Ar << radiusX;
                    Ar << radiusY;

                    iCircle.SetRadius( radiusX, radiusY );
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
