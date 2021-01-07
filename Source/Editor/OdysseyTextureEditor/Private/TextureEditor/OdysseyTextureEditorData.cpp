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
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.Remove(mOnPrePropertyChangedDelegateHandle);
    UPackage::PreSavePackageEvent.Remove(mOnPackagePreSaveHandle);
    UPackage::PackageSavedEvent.Remove(mOnPackageSavedHandle);

    if( mDisplaySurface )
    {
        delete mDisplaySurface;
    }
}

FOdysseyTextureEditorData::FOdysseyTextureEditorData(UTexture2D* iTexture)
    : mTexture( iTexture )
{
    mOnPrePropertyChangedDelegateHandle = FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw(this, &FOdysseyTextureEditorData::OnPreGlobalObjectPropertyChanged);

    /* FDelegateHandle */ mOnPackagePreSaveHandle = UPackage::PreSavePackageEvent.AddRaw(this, &FOdysseyTextureEditorData::OnPackagePreSave);
	/* FDelegateHandle */ mOnPackageSavedHandle = UPackage::PackageSavedEvent.AddRaw(this, &FOdysseyTextureEditorData::OnPackageSaved);
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
        userData->GetLayerStack()->Init(mTexture->Source.GetSizeX(), mTexture->Source.GetSizeY(), format);
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
    mTexture->UpdateResource();
}

void
FOdysseyTextureEditorData::PrepareTextureProperties()
{
    FTextureFormatSettings textureFormatSettings;
	mTexture->GetLayerFormatSettings(0, textureFormatSettings);

	// Create new Texture Properties Backup
	mPropertiesBackup = { textureFormatSettings.CompressionNone };

	// Overwrite Texture properties
    textureFormatSettings.CompressionNone = 1;
	mTexture->SetLayerFormatSettings(0, textureFormatSettings);

	mTexture->UpdateResource();
    mTexture->TemporarilyDisableStreaming(); //needed to be able to draw on previously streamed textures, avoids using NoMipMaps
}

void
FOdysseyTextureEditorData::ApplyPropertiesBackup()
{
    FTextureFormatSettings textureFormatSettings;
    mTexture->GetLayerFormatSettings(0,textureFormatSettings);
    textureFormatSettings.CompressionNone = mPropertiesBackup.mTextureCompressionNone;
    mTexture->SetLayerFormatSettings(0, textureFormatSettings);

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

void
FOdysseyTextureEditorData::OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain)
{
	if (mTexture == Cast<UTexture2D>(iObject))
    {
        //Texture properties will change, we need to SyncTextureWithBlock
        CopyBlockDataIntoUTexture( mDisplaySurface->Block(), mTexture );
    }
}

void
FOdysseyTextureEditorData::OnPackagePreSave(UPackage* iPackage)
{
    if (!mTexture)
        return;

    UPackage* package = CastChecked<UPackage>(mTexture->GetOuter());
    if (package != iPackage)
        return;

    PaintEngine()->Flush();
	SyncTextureAndInvalidate();
	ApplyPropertiesBackup();
}

void
FOdysseyTextureEditorData::OnPackageSaved(const FString& iPackageFilename, UObject* iOuter)
{
    if (!mTexture)
        return;

    if (mTexture->GetOuter() != iOuter)
        return;

    PrepareTextureProperties();
}