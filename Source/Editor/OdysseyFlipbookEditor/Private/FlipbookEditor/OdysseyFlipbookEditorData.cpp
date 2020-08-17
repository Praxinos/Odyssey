// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyFlipbookEditorData.h"

#include "ULISLoaderModule.h"
#include "PaperFlipbook.h"
#include "PaperSprite.h"
#include "OdysseyTextureAssetUserData.h"


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

FOdysseyFlipbookEditorData::FOdysseyFlipbookEditorData(UPaperFlipbook* iFlipbook)
    : mFlipbook( iFlipbook )
    , mTexture( NULL )
    //, mKeyFrame(-1)
    , mLayerStack( NULL )
	, mDisplaySurface(NULL)
	, mPreviewSurface(new FOdysseySurfaceReadOnly(NULL))
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
    if (mFlipbook->GetNumKeyFrames() <= 0)
    {
        //We don't need to initialize anything if there is no keyFrames
        return;
    }

    Texture(TextureAtKeyframe(0)); //TODO: Check if this should be done here or if we should rely on the timeline scrubposition to select teh keyframe
}

/* int
FOdysseyFlipbookEditorData::KeyFrame()
{
	return mKeyFrame;
} */

// void
// FOdysseyFlipbookEditorData::KeyFrame(int iIndex/*, bool iForceUpdate*/) //should we set a keyframe or the timeline scrubposition
// {
    /* if (iIndex == mKeyFrame && !iForceUpdate)
        return; */

   /* UTexture2D* texture = TextureAtKeyframe(iIndex);
    if (texture == mTexture)
        return;*/

    //
    //SetCurrentTexture(texture);

    //mKeyFrame = iIndex;
//}

void
FOdysseyFlipbookEditorData::Texture(UTexture2D* iTexture)
{
    // Apply current Texture properties backup
    ApplyPropertiesBackup(); //TODO: Find Better name

    //Sync current texture with surface data
    SyncTextureWithSurfaceBlock(); //TODO: Find Better name

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


UPaperFlipbook*
FOdysseyFlipbookEditorData::Flipbook()
{
	return mFlipbook;
}

UTexture2D*
FOdysseyFlipbookEditorData::Texture()
{
	return mTexture; //TextureAtKeyframe(mKeyFrame);
}

UTexture2D*
FOdysseyFlipbookEditorData::TextureAtKeyframe(int32 iKeyframe)
{
	if (iKeyframe < 0 || iKeyframe >= mFlipbook->GetNumKeyFrames())
		return NULL;

	const FPaperFlipbookKeyFrame& keyFrame = mFlipbook->GetKeyFrameChecked(iKeyframe);
	const UPaperSprite* sprite = keyFrame.Sprite;
	if (!sprite)
	{
		return NULL;
	}

	return sprite->GetSourceTexture();
}

/* UTexture2D*
FOdysseyFlipbookEditorData::TextureAtScrubPosition(float iScrubPosition)
{
	if (iScrubPosition < 0.0f || iScrubPosition >= mFlipbook->GetNumFrames())
		return NULL;

	if (FramesPerSecond > 0.0f)
	{
		float SumTime = 0.0f;

		for (int32 KeyFrameIndex = 0; KeyFrameIndex < KeyFrames.Num(); ++KeyFrameIndex)
		{
			SumTime += KeyFrames[KeyFrameIndex].FrameRun / FramesPerSecond;

			if (Time <= SumTime)
			{
				return KeyFrameIndex;
			}
		}

		// Return the last frame (note: relies on INDEX_NONE = -1 if there are no key frames)
		return KeyFrames.Num() - 1;
	}
	else
	{
		return (KeyFrames.Num() > 0) ? 0 : INDEX_NONE;
	}

	const FPaperFlipbookKeyFrame& keyFrame = mFlipbook->GetKeyFrameChecked(iKeyframe);
	const UPaperSprite* sprite = keyFrame.Sprite;
	if (!sprite)
	{
		return NULL;
	}

	return sprite->GetSourceTexture();
} */

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
