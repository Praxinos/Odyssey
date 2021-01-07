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
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.Remove(mOnPrePropertyChangedDelegateHandle);
    UPackage::PreSavePackageEvent.Remove(mOnPackagePreSaveHandle);
    UPackage::PackageSavedEvent.Remove(mOnPackageSavedHandle);

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
    mOnPrePropertyChangedDelegateHandle = FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw(this, &FOdysseyFlipbookEditorData::OnPreGlobalObjectPropertyChanged);
    /* FDelegateHandle */ mOnPackagePreSaveHandle = UPackage::PreSavePackageEvent.AddRaw(this, &FOdysseyFlipbookEditorData::OnPackagePreSave);
	/* FDelegateHandle */ mOnPackageSavedHandle = UPackage::PackageSavedEvent.AddRaw(this, &FOdysseyFlipbookEditorData::OnPackageSaved);
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
        userData->GetLayerStack()->Init(iTexture->Source.GetSizeX(), iTexture->Source.GetSizeY(), format);
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
        mTexture->UpdateResource();
    }
}

void
FOdysseyFlipbookEditorData::PrepareTextureProperties()
{
    FTextureFormatSettings textureFormatSettings;
	mTexture->GetLayerFormatSettings(0, textureFormatSettings);

	// Create new Texture Properties Backup
	mPropertiesBackup = { textureFormatSettings.CompressionNone };
    
    textureFormatSettings.CompressionNone = 1;
	mTexture->SetLayerFormatSettings(0, textureFormatSettings);
	mTexture->UpdateResource();
    mTexture->TemporarilyDisableStreaming(); //needed to be able to draw on previously streamed textures, avoids using NoMipMaps
}

void
FOdysseyFlipbookEditorData::ApplyPropertiesBackup()
{
	if (mTexture) {
        
        FTextureFormatSettings textureFormatSettings;
        mTexture->GetLayerFormatSettings(0,textureFormatSettings);
        textureFormatSettings.CompressionNone = mPropertiesBackup.mTextureCompressionNone;
        mTexture->SetLayerFormatSettings(0, textureFormatSettings);

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

void
FOdysseyFlipbookEditorData::OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain)
{
	if (mTexture == Cast<UTexture2D>(iObject))
    {
        //Texture properties will change, we need to SyncTextureWithBlock
        CopyBlockDataIntoUTexture( mDisplaySurface->Block(), mTexture );
    }
}

void
FOdysseyFlipbookEditorData::OnPackagePreSave(UPackage* iPackage)
{
    if (!mTexture)
        return;

    UPackage* package = CastChecked<UPackage>(mTexture->GetOuter());
    if (package != iPackage)
        return;

    PaintEngine()->Flush();
    SyncTextureWithSurfaceBlock();
	ApplyPropertiesBackup();
}

void
FOdysseyFlipbookEditorData::OnPackageSaved(const FString& iPackageFilename, UObject* iOuter)
{
    if (!mTexture)
        return;

    if (mTexture->GetOuter() != iOuter)
        return;

    PrepareTextureProperties();
}
