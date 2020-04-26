// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

//The actual function to load and save a OdysseyImageLayer to disk
#include "OdysseyBlock.h"
#include <ULIS3>

inline FArchive& operator<<(FArchive &Ar, FOdysseyImageLayer* ioSaveImageLayer )
{
    if( !ioSaveImageLayer ) 
        return Ar;
    
    /*IOdysseySerializable* serializable = (FOdysseyImageLayer*)(ioSaveImageLayer);
    Ar << serializable;*/
 
    Ar << ioSaveImageLayer->mName;
    
    Ar << ioSaveImageLayer->mIsLocked;
    
    Ar << ioSaveImageLayer->mIsVisible;
    
    Ar << ioSaveImageLayer->mIsAlphaLocked;
    
    Ar << ioSaveImageLayer->mBlendingMode;
    
    Ar << ioSaveImageLayer->mOpacity;
    
    if( Ar.IsSaving() )
    {
        int width = ioSaveImageLayer->mBlock->Width();
        int height = ioSaveImageLayer->mBlock->Height();
        
        Ar << width;
        Ar << height;
        
        ::ul3::IBlock* blockLayerData = ::ul3::FMakeContext::CopyBlockRect( ioSaveImageLayer->mBlock->GetBlock(), ::ul3::FRect( 0, 0, ioSaveImageLayer->mBlock->Width(), ioSaveImageLayer->mBlock->Height() ) );
        TArray<uint8> layerData = TArray<uint8>();
        layerData.AddUninitialized(blockLayerData->BytesTotal());
        FMemory::Memcpy(layerData.GetData(), blockLayerData->DataPtr(), blockLayerData->BytesTotal());
        delete blockLayerData;

        Ar << layerData;
    }
    else if( Ar.IsLoading() )
    {
        int width;
        int height;
        ETextureSourceFormat textureFormat = ETextureSourceFormat::TSF_BGRA8;
                
        Ar << width;
        Ar << height;
        
        check( !ioSaveImageLayer->mBlock );
        ioSaveImageLayer->mBlock = new FOdysseyBlock( width, height, textureFormat );
        ::ul3::FClearFillContext::Clear( ioSaveImageLayer->mBlock->GetBlock() );
        
        TArray<uint8> layerData = TArray<uint8>();
        layerData.AddUninitialized(ioSaveImageLayer->mBlock->GetBlock()->BytesTotal());
        
        Ar << layerData;

        for( int j = 0; j < layerData.Num(); j++ )
        {
            *(ioSaveImageLayer->mBlock->GetBlock()->DataPtr() + j) = layerData[j];
        }
    }
    return Ar;
}
