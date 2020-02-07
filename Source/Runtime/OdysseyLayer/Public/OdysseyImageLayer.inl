// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

//The actual function to load and save a OdysseyImageLayer to disk
inline FArchive& operator<<(FArchive &Ar, FOdysseyImageLayer* ioSaveImageLayer )
{
    if(!ioSaveImageLayer) return Ar;
    
    IOdysseySerializable* serializable = (FOdysseyImageLayer*)(ioSaveImageLayer);
    Ar << serializable;
 
    Ar << ioSaveImageLayer->mName;
    
    Ar << ioSaveImageLayer->mIsLocked;
    
    Ar << ioSaveImageLayer->mIsVisible;
    
    Ar << ioSaveImageLayer->mIsAlphaLocked;
    
    Ar << ioSaveImageLayer->mBlendingMode;
    
    Ar << ioSaveImageLayer->mOpacity;
           
    return Ar;
}
