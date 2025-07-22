// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyFile.h"

void
FOdysseyFile::WriteChunk( uint32 iChunkID, FArchive &Ar, std::function<void(FArchive &Ar)> iCallback )
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
FOdysseyFile::ReadChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback )
{
    //UE_LOG( LogTemp, Warning, TEXT("ReadChunks") );

    while( Ar.Tell() != iChunkEnd )
    {
        uint64 chunkLen;
        uint32 chunkID;

        Ar << chunkID;
        Ar << chunkLen;

        //UE_LOG( LogTemp, Warning, TEXT("Reading Chunk %X %d"), chunkID, chunkLen );

        /*if ( chunkLen )
        {*/
            iCallback( chunkID, chunkLen, Ar );
        /*}*/
    }
}
