#include "OdysseyExport.h"

void
FOdysseyExport::WriteChunk( uint32 iChunkID, FArchive &Ar, std::function<void(FArchive &Ar)> iCallback )
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
