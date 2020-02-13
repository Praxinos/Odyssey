// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

//The actual function to load and save a FOdysseyLayerStack to disk
inline FArchive& operator<<(FArchive &Ar, FOdysseyLayerStack* ioSaveLayerStack )
{
    if(!ioSaveLayerStack) return Ar;
    
    IOdysseySerializable* serializable = (IOdysseySerializable*)(ioSaveLayerStack);
    Ar << serializable;
        
    Ar << ioSaveLayerStack->mWidth;
    Ar << ioSaveLayerStack->mHeight;
    Ar << *(ioSaveLayerStack->mLayers);
    
    ioSaveLayerStack->mTextureSourceFormat = ETextureSourceFormat::TSF_BGRA8;
      
    /*
    if( Ar.IsSaving() )
    {
        int numLayers = ioSaveLayerStack->mLayers.Num();
        Ar << numLayers;
        
        TArray<TSharedPtr<IOdysseyLayer>>* layers = ioSaveLayerStack->GetLayers();

        for( int i = 0; i < numLayers; i++)
        {
            FOdysseyImageLayer* imageLayer = nullptr;
            imageLayer = static_cast<FOdysseyImageLayer*> ((*layers)[i].Get());

            if( imageLayer )
            {
                Ar << imageLayer;
            }
        }
    }
    else*/ if ( Ar.IsLoading() )
    {
        
        ioSaveLayerStack->mResultBlock = new FOdysseyBlock( ioSaveLayerStack->mWidth, ioSaveLayerStack->mHeight, ioSaveLayerStack->mTextureSourceFormat );
        ioSaveLayerStack->mTempBlock = new FOdysseyBlock( ioSaveLayerStack->mWidth, ioSaveLayerStack->mHeight, ioSaveLayerStack->mTextureSourceFormat );
        ::ULIS::FClearFillContext::Clear( ioSaveLayerStack->mResultBlock->GetIBlock() );
        ::ULIS::FClearFillContext::Clear( ioSaveLayerStack->mTempBlock->GetIBlock() );
        
        /*
        int numLayers;
        Ar << numLayers;
        for (int i = 0; i < numLayers; i++)
        {
            FOdysseyImageLayer* imageLayer = ioSaveLayerStack->AddLayer();
            
            Ar << imageLayer;
        }
     */
        ioSaveLayerStack->ComputeResultBlock();
    }

    return Ar;
}
