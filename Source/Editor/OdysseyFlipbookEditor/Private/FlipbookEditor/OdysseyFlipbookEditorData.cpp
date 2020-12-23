// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
	//Sync current texture with surface data
	SyncTextureWithSurfaceBlock();

    // Apply current Texture properties backup
    ApplyPropertiesBackup();

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
    mDisplaySurface = new FOdysseySurfaceEditable( mTexture);
    mDisplaySurface->Invalidate();
}

UOdysseyTextureAssetUserData*
FOdysseyFlipbookEditorData::FindOrCreateTextureUserData(UTexture2D* iTexture)
{
    UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(iTexture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
    if( !userData )
    {
        //Init user data
        ::ul3::tFormat format = ULISFormatForUE4TextureSourceFormat(iTexture->Source.GetFormat());
        userData = NewObject< UOdysseyTextureAssetUserData >(iTexture, NAME_None, RF_Public);
        userData->GetLayerStack()->Init(iTexture->GetSizeX(), iTexture->GetSizeY(), format);
        iTexture->AddAssetUserData( userData );

        //Create image layer
        FOdysseyBlock* textureData = NewOdysseyBlockFromUTextureData( iTexture, userData->GetLayerStack()->Format() );
		FName layerName = userData->GetLayerStack()->GetLayerRoot()->GetNextLayerName();
		TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(layerName, textureData));

        //Add Layer
        userData->GetLayerStack()->AddLayer(imageLayer);

        // Notify for changes
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
    FTextureFormatSettings textureFormatSettings;
	mTexture->GetLayerFormatSettings(0, textureFormatSettings);

	// Create new Texture Properties Backup
	mPropertiesBackup = { mTexture->MipGenSettings, mTexture->MaxTextureSize, textureFormatSettings };

	// Overwrite Texture properties
	mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps; //Mandatory or can lead to display not refreshing because it displays a mipmap instead of the texture itself (I guess)
    
    textureFormatSettings.CompressionNone = 1;
	mTexture->SetLayerFormatSettings(0, textureFormatSettings);

	// mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	// mTexture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
	mTexture->UpdateResource();
}

void
FOdysseyFlipbookEditorData::ApplyPropertiesBackup()
{
	if (mTexture) {
		mTexture->MipGenSettings = mPropertiesBackup.mTextureMipGenBackup;
        mTexture->SetLayerFormatSettings(0, mPropertiesBackup.mTextureFormatSettings);
        mTexture->MaxTextureSize = mPropertiesBackup.mTextureMaxTextureSize;
        // mTexture->CompressionSettings = mPropertiesBackup.mTextureCompressionBackup;
        // mTexture->LODGroup = mPropertiesBackup.mTextureGroupBackup;
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
