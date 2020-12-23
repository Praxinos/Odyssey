// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
    {
        delete mDisplaySurface;
    }
}

FOdysseyTextureEditorData::FOdysseyTextureEditorData(UTexture2D* iTexture)
    : mTexture( iTexture )
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyTextureEditorData::Init()
{   
	FTextureFormatSettings textureFormatSettings;
	mTexture->GetLayerFormatSettings(0, textureFormatSettings);

	PrepareTextureProperties();
    
    // Get or Create Texture userData
    UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(mTexture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
    if( !userData )
    {
        ::ul3::tFormat format = ULISFormatForUE4TextureSourceFormat(mTexture->Source.GetFormat());
        userData = NewObject< UOdysseyTextureAssetUserData >(mTexture, NAME_None, RF_Public);
        userData->GetLayerStack()->Init(mTexture->GetSizeX(), mTexture->GetSizeY(), format);
        mTexture->AddAssetUserData( userData );
        FOdysseyBlock* textureData = NewOdysseyBlockFromUTextureData( mTexture, userData->GetLayerStack()->Format());
		TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(userData->GetLayerStack()->GetLayerRoot()->GetNextLayerName(), textureData));
        userData->GetLayerStack()->AddLayer(imageLayer);
        mTexture->PostEditChange();
    }

	// Get Texture LayerStack
    mLayerStack = userData->GetLayerStack();

    // Setup Surface
    mDisplaySurface = new FOdysseySurfaceEditable(mTexture);
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
FOdysseyTextureEditorData::PrepareTextureProperties()
{
    FTextureFormatSettings textureFormatSettings;
	mTexture->GetLayerFormatSettings(0, textureFormatSettings);

	// Create new Texture Properties Backup
	mPropertiesBackup = { mTexture->MipGenSettings, mTexture->MaxTextureSize, textureFormatSettings };

	// Overwrite Texture properties
	mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps; //Mandatory or can lead to display not refreshing because it displays a mipmap instead of the texture itself (I guess)
    mTexture->MaxTextureSize = 0;
    
    textureFormatSettings.CompressionNone = 1;
	mTexture->SetLayerFormatSettings(0, textureFormatSettings);

	// mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	// mTexture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
	mTexture->UpdateResource();
}

void
FOdysseyTextureEditorData::ApplyPropertiesBackup()
{
	mTexture->MipGenSettings = mPropertiesBackup.mTextureMipGenBackup;
    mTexture->SetLayerFormatSettings(0, mPropertiesBackup.mTextureFormatSettings);
    mTexture->MaxTextureSize = mPropertiesBackup.mTextureMaxTextureSize;
	// mTexture->CompressionSettings = mPropertiesBackup.mTextureCompressionBackup;
	// mTexture->LODGroup = mPropertiesBackup.mTextureGroupBackup;
    mTexture->UpdateResource();
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

FOdysseySurfaceEditable*
FOdysseyTextureEditorData::DisplaySurface()
{
	return mDisplaySurface;
}
