#include "Export/v1/OdysseyVectorExport.h"

void
FOdysseyVectorExportV1::WriteChunk( uint32 iChunkID, FArchive &Ar, std::function<void(FArchive &Ar)> iCallback )
{
    uint64 chunkLen = 0;
    uint64 chunkLenAddress;
    uint64 currentAddress;

    Ar << iChunkID;

    chunkLenAddress = Ar.Tell();
    // write dummy value, we will set it at the end
    Ar << chunkLen;
    // write data
    iCallback( Ar );

    currentAddress = Ar.Tell();
    // compute the size of this chunk
    chunkLen = currentAddress - chunkLenAddress - sizeof( uint64 );

    Ar.Seek( chunkLenAddress );

    Ar << chunkLen;

    Ar.Seek( currentAddress );
}

void
FOdysseyVectorExportV1::Write( FOdysseyVectorScene* iScene, FArchive &Ar )
{
    // write the chunk even if iVEngine is nullptr to prevent "expected size mismatch" when unreal reads the data.
    // iVEngine is nullptr when unreal preloads the file.
    // The reading process can then just skip the chunk no matter its size.

    FOdysseyVectorExportV1::WriteChunk( FOdysseyVectorExportV1::CHUNK_VECTOR_MAGIC_V1
                                    , Ar
                                    , [iScene](FArchive &Ar) -> void
    {
        if( iScene )
        {
            std::vector<FOdysseyVectorObject*> vectorObjectArray;

            FOdysseyVectorObject::TreeToArray ( iScene, vectorObjectArray );

            FOdysseyVectorExportV1::WriteDeclareObjects( vectorObjectArray, Ar );
            FOdysseyVectorExportV1::WriteDefineObjects( vectorObjectArray, Ar );
        }
    } );
}
