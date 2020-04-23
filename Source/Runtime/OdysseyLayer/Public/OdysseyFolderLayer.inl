// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

//The actual function to load and save a OdysseyImageLayer to disk
inline FArchive& operator<<(FArchive &Ar, FOdysseyFolderLayer* ioSaveFolderLayer )
{
    if(!ioSaveFolderLayer) return Ar;
    
    /*IOdysseySerializable* serializable = (FOdysseyImageLayer*)(ioSaveImageLayer);
    Ar << serializable;*/
 
    Ar << ioSaveFolderLayer->mName;
    
    Ar << ioSaveFolderLayer->mIsLocked;
    
    Ar << ioSaveFolderLayer->mIsVisible;
        
    Ar << ioSaveFolderLayer->mBlendingMode;
    
    Ar << ioSaveFolderLayer->mIsOpen;
    
    Ar << ioSaveFolderLayer->mOpacity;
    
    return Ar;
}
