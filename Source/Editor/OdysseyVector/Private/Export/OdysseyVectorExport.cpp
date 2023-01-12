#include "Export/OdysseyVectorExport.h"

void
FOdysseyVectorExport::WriteChunk( uint32 iChunkID, FArchive &Ar, std::function<void(FArchive &Ar)> iCallback )
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
FOdysseyVectorExport::Write( FOdysseyVectorEngine* iVEngine, FArchive &Ar )
{
    // write the chunk even if iVEngine is nullptr to prevent "expected size mismatch" when unreal reads the data.
    // iVEngine is nullptr when unreal preloads the file.
    // The reading process can then just skip the chunk no matter its size.
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_VECTOR_MAGIC
                                    , Ar
                                    , [iVEngine](FArchive &Ar) -> void
    {
        if( iVEngine )
        {
            std::vector<UOdysseyVectorObject*> vectorObjectArray;

            UOdysseyVectorObject::TreeToArray ( iVEngine->GetScene(), vectorObjectArray );

            FOdysseyVectorExport::WriteDeclareObjects( vectorObjectArray, Ar );
            FOdysseyVectorExport::WriteDefineObjects( vectorObjectArray, Ar );
        }
    } );
}
