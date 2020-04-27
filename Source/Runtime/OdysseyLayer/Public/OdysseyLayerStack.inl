// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

//The actual function to load and save a FOdysseyLayerStack to disk
inline FArchive& operator<<(FArchive &Ar, FOdysseyLayerStack* ioSaveLayerStack )
{
    if( !ioSaveLayerStack )
        return Ar;

    Ar << ioSaveLayerStack->mWidth;
    Ar << ioSaveLayerStack->mHeight;

    Ar << *(ioSaveLayerStack->mLayers);

    if ( Ar.IsLoading() )
    {
        ioSaveLayerStack->mTextureSourceFormat = ETextureSourceFormat::TSF_BGRA8;
        ioSaveLayerStack->mIsInitialized = false;
        ioSaveLayerStack->Init( ioSaveLayerStack->mWidth, ioSaveLayerStack->mHeight );
        ioSaveLayerStack->ComputeResultBlock();
    }

    return Ar;
}

