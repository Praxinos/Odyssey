// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyTextureEditorData.h"

#include "OdysseyTextureAssetUserData.h"
#include "ULISLoaderModule.h"


/////////////////////////////////////////////////////
// FOdysseyTextureEditorData
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorData::~FOdysseyTextureEditorData()
{
    if( mDisplaySurface )
        delete mDisplaySurface;
}

FOdysseyTextureEditorData::FOdysseyTextureEditorData(UTexture2D* iTexture)
    : mTexture( iTexture )
    , mLiveUpdateInfo()
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyTextureEditorData::Init()
{    
	// Backup Texture properties
	mPropertiesBackup = { mTexture->MipGenSettings, mTexture->CompressionSettings, mTexture->LODGroup };
    
	// Overwrite Texture properties
	mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    mTexture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
    mTexture->UpdateResource();
    
    // Get or Create Texture userData
    UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(mTexture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
    if( !userData )
    {
        userData = NewObject< UOdysseyTextureAssetUserData >(mTexture, NAME_None, RF_Public);
        mTexture->AddAssetUserData( userData );
        FOdysseyBlock* textureData = NewOdysseyBlockFromUTextureData( mTexture );
        userData->GetLayerStack()->InitFromData( textureData );
        delete textureData;
        mTexture->PostEditChange();
    }

	// Get Texture LayerStack
    mLayerStack = userData->GetLayerStack();

    // Setup Surface
    mDisplaySurface = new FOdysseySurface( mLayerStack->GetResultBlock() );
    mLiveUpdateInfo.main = mDisplaySurface->Texture();
    mLiveUpdateInfo.live = mTexture;
    mLiveUpdateInfo.enabled = true;
    mDisplaySurface->Block()->GetBlock()->SetOnInvalid( ::ul3::FOnInvalid( &InvalidateLiveSurfaceCallback, static_cast<void*>( &mLiveUpdateInfo ) ) );

    mDisplaySurface->Invalidate();

    // Support undo/redo
    //mDisplaySurface->Texture()->SetFlags( RF_Transactional );
    //GEditor->RegisterForUndo( this );
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override

void
FOdysseyTextureEditorData::SyncTextureAndInvalidate()
{
    CopyBlockDataIntoUTexture( mDisplaySurface->Block(), mTexture );
    InvalidateTextureFromData( mDisplaySurface->Block(), mTexture );
}


void
FOdysseyTextureEditorData::ApplyPropertiesBackup()
{
	mTexture->MipGenSettings = mPropertiesBackup.mTextureMipGenBackup;
	mTexture->CompressionSettings = mPropertiesBackup.mTextureCompressionBackup;
	mTexture->LODGroup = mPropertiesBackup.mTextureGroupBackup;
}

FOdysseyLayerStack*
FOdysseyTextureEditorData::LayerStack()
{
    return mLayerStack;
}


UTexture2D*
FOdysseyTextureEditorData::Texture()
{
	return mTexture;
}

FOdysseySurface*
FOdysseyTextureEditorData::DisplaySurface()
{
	return mDisplaySurface;
}

FOdysseyLiveUpdateInfo*
FOdysseyTextureEditorData::LiveUpdateInfo()
{
	return &mLiveUpdateInfo;
}
