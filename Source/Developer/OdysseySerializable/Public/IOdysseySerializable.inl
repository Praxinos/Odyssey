// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

//The actual function to load and save a IOdysseySerializable to disk
inline FArchive& operator<<(FArchive &Ar, IOdysseySerializable* ioSaveSerializable )
{
    if(!ioSaveSerializable) return Ar;
    
    Ar << ioSaveSerializable->mVersion;
    
    return Ar;
}
