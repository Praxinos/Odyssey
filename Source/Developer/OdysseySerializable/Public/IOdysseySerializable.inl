// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

//The actual function to load and save a IOdysseySerializable to disk
inline FArchive& operator<<(FArchive &Ar, IOdysseySerializable* ioSaveSerializable )
{
    if(!ioSaveSerializable) return Ar;
    
    Ar << ioSaveSerializable->mVersion;
    
    return Ar;
}
