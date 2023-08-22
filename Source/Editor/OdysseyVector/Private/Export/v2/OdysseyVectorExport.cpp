#include "Export/v2/OdysseyVectorExport.h"

void
FOdysseyVectorExportV2::WriteChunk( uint32 iChunkID, FArchive &Ar, std::function<void(FArchive &Ar)> iCallback )
{
    uint64 chunkLen = 0;
    uint64 chunkLenAddress;
    uint64 currentAddress;
    uint64 chunkAddress = Ar.Tell();

    Ar << iChunkID;

    chunkLenAddress = Ar.Tell();
    // write dummy value, we will set it at the end
    Ar << chunkLen;

//    UE_LOG( LogTemp, Warning, TEXT("Writing Chunk %X at %X"), iChunkID, chunkAddress );

    // write data
    iCallback( Ar );

    currentAddress = Ar.Tell();
    // compute the size of this chunk
    chunkLen = currentAddress - chunkLenAddress - sizeof( uint64 );

    Ar.Seek( chunkLenAddress );

    Ar << chunkLen;

    Ar.Seek( currentAddress );

//    UE_LOG( LogTemp, Warning, TEXT("Chunk %X Size: %d"), iChunkID, chunkLen );
}

void
FOdysseyVectorExportV2::Write( FOdysseyVectorScene* iScene, FArchive &Ar )
{
    // write the chunk even if iVEngine is nullptr to prevent "expected size mismatch" when unreal reads the data.
    // iVEngine is nullptr when unreal preloads the file.
    // The reading process can then just skip the chunk no matter its size.

    FOdysseyVectorExportV2::WriteChunk( FOdysseyVectorExportV2::CHUNK_VECTOR_MAGIC_V2
                                    , Ar
                                    , [iScene](FArchive &Ar) -> void
    {
        if( iScene )
        {
            std::vector<FOdysseyVectorObject*> vectorObjectArray;

            FOdysseyVectorObject::TreeToArray ( iScene, vectorObjectArray );

            FOdysseyVectorExportV2::WriteDeclareObjects( vectorObjectArray, Ar );
            FOdysseyVectorExportV2::WriteDefineObjects( vectorObjectArray, Ar );
        }
    } );
}
