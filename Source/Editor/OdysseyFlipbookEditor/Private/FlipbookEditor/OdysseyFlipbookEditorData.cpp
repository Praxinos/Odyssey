// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyFlipbookEditorData.h"

#include "ULISLoaderModule.h"
#include "PaperFlipbook.h"
#include "PaperSprite.h"
#include "OdysseyTextureAssetUserData.h"
#include "OdysseyFlipbookWrapper.h"
#include "OdysseyFlipbookEditorToolkit.h"



/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorData
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorData::~FOdysseyFlipbookEditorData()
{
    if( mDisplaySurface ) {
        delete mDisplaySurface;
        mDisplaySurface = NULL;
    }

	if (mPreviewSurface) {
		delete mPreviewSurface;
		mPreviewSurface = NULL;
	}
}

FOdysseyFlipbookEditorData::FOdysseyFlipbookEditorData(TSharedPtr<FOdysseyFlipbookWrapper>& iFlipbookWrapper, TSharedPtr<FOdysseyFlipbookEditorToolkit> iToolkit)
    : mFlipbookWrapper( iFlipbookWrapper )
    , mTexture( NULL )
    , mLayerStack( NULL )
	, mDisplaySurface(NULL)
	, mPreviewSurface(new FOdysseySurfaceReadOnly(NULL))
    , mToolkit( iToolkit )
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyFlipbookEditorData::Init()
{
    // Setup Flipbook
    // mFlipbook->MarkPackageDirty(); //TODO: Call MarkPackageDirty only when needed, not here

    // Get Flipbook keyFrames
    if (mFlipbookWrapper->Flipbook()->GetNumKeyFrames() <= 0)
    {
        //We don't need to initialize anything if there is no keyFrames
        return;
    }
    Texture(mFlipbookWrapper->GetKeyframeTexture(0));
}

void
FOdysseyFlipbookEditorData::Texture(UTexture2D* iTexture)
{
    // Apply current Texture properties backup
    ApplyPropertiesBackup();

    //Sync current texture with surface data
    SyncTextureWithSurfaceBlock();

	//Remove Surface
	if (mDisplaySurface)
    {
		mDisplaySurface->Block()->GetBlock()->SetOnInvalid(::ul3::FOnInvalid());
		delete mDisplaySurface;
	}
    mDisplaySurface = NULL;
    
    //Remove Layerstack
    mLayerStack = NULL;

    // Replace current Texture with the given one
    mTexture = iTexture;

    //If no Texture provided, then nothing to do
    if (!mTexture)
        return;

    //Prepare the texture to be edited and store its properties in a backup structure
	PrepareTextureProperties();

    // Get or Create Texture userData
    UOdysseyTextureAssetUserData* userData = FindOrCreateTextureUserData(mTexture);

	// Get Texture LayerStack
    mLayerStack = userData->GetLayerStack();

    // Setup Surface
    mDisplaySurface = new FOdysseySurfaceEditable( mTexture, mLayerStack->GetResultBlock() );
    mDisplaySurface->Block()->GetBlock()->SetOnInvalid( ::ul3::FOnInvalid( &InvalidateSurfaceCallback, static_cast<void*>( mDisplaySurface ) ) );
    mDisplaySurface->Invalidate();
}

UOdysseyTextureAssetUserData*
FOdysseyFlipbookEditorData::FindOrCreateTextureUserData(UTexture2D* iTexture)
{
    UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(iTexture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
    if( !userData )
    {
        userData = NewObject< UOdysseyTextureAssetUserData >(iTexture, NAME_None, RF_Public);
        iTexture->AddAssetUserData( userData );
        FOdysseyBlock* textureData = NewOdysseyBlockFromUTextureData( iTexture );
        userData->GetLayerStack()->InitFromData( textureData );
        delete textureData;
        iTexture->PostEditChange();
    }
    return userData;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override

void
FOdysseyFlipbookEditorData::SyncTextureWithSurfaceBlock()
{
    if (mTexture) {
        CopyBlockDataIntoUTexture( mDisplaySurface->Block(), mTexture);
        InvalidateTextureFromData( mDisplaySurface->Block(), mTexture);
    }
}

void
FOdysseyFlipbookEditorData::PrepareTextureProperties()
{
	// Create new Texture Properties Backup
	mPropertiesBackup = { mTexture->MipGenSettings, mTexture->CompressionSettings, mTexture->LODGroup };
    
	// Overwrite Texture properties
	mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    mTexture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
    mTexture->UpdateResource();
}

void
FOdysseyFlipbookEditorData::ApplyPropertiesBackup()
{
	if (mTexture) {
		mTexture->MipGenSettings = mPropertiesBackup.mTextureMipGenBackup;
		mTexture->CompressionSettings = mPropertiesBackup.mTextureCompressionBackup;
		mTexture->LODGroup = mPropertiesBackup.mTextureGroupBackup;
        mTexture->UpdateResource();
	}
}

FOdysseyLayerStack*
FOdysseyFlipbookEditorData::LayerStack() const
{
    return mLayerStack;
}


TSharedPtr<FOdysseyFlipbookWrapper>&
FOdysseyFlipbookEditorData::FlipbookWrapper()
{
	return mFlipbookWrapper;
}

UTexture2D*
FOdysseyFlipbookEditorData::Texture()
{
	return mTexture;
}

TWeakPtr<FOdysseyFlipbookEditorToolkit>&
FOdysseyFlipbookEditorData::Toolkit()
{
	return mToolkit;
}

FOdysseySurfaceEditable*
FOdysseyFlipbookEditorData::DisplaySurface()
{
	return mDisplaySurface;
}

FOdysseySurfaceReadOnly*
FOdysseyFlipbookEditorData::PreviewSurface()
{
	return mPreviewSurface;
}
