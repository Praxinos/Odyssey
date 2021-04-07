// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTexture2DWrapper.h"
#include "OdysseySurfaceTexture2DEditable.h"

FOdysseyTexture2DWrapper::~FOdysseyTexture2DWrapper()
{
    SetTexture(nullptr);
}

FOdysseyTexture2DWrapper::FOdysseyTexture2DWrapper(UTexture2D* iTexture) :
    FOdysseyTextureWrapper(),
    mTexture(nullptr),
    mSurface(nullptr),
    mLayerStack(nullptr)
{
    SetTexture(iTexture);
}

UTexture2D*
FOdysseyTexture2DWrapper::Texture() const
{
    return mTexture;
}

FOdysseySurfaceTexture2DEditable*
FOdysseyTexture2DWrapper::Surface() const
{
    return mSurface;
}

FOdysseyLayerStack*
FOdysseyTexture2DWrapper::LayerStack() const
{
    return mLayerStack;
}

void
FOdysseyTexture2DWrapper::SetTexture(UTexture2D* iTexture)
{
    if (iTexture == mTexture)
        return;

    PreTextureChange();
    mTexture = iTexture;
    PostTextureChange();
}

void
FOdysseyTexture2DWrapper::UpdateTextureFromSurface()
{
    if(!mTexture)
        return;

    CopyBlockDataIntoUTexture( Surface()->Block(), mTexture);
    mTexture->UpdateResource();
}

void
FOdysseyTexture2DWrapper::SetTextureProperties()
{
    if(!mTexture)
        return;

    FTextureFormatSettings textureFormatSettings;
    mTexture->GetLayerFormatSettings(0, textureFormatSettings);

	// Create new Texture Properties Backup
	mPropertyCompressionNone = textureFormatSettings.CompressionNone;

	// Overwrite Texture properties
    textureFormatSettings.CompressionNone = 1;
    mTexture->SetLayerFormatSettings(0, textureFormatSettings);

    mTexture->UpdateResource();
    mTexture->TemporarilyDisableStreaming(); //needed to be able to draw on previously streamed textures, avoids using NoMipMaps
}

void
FOdysseyTexture2DWrapper::RestoreTextureProperties()
{
    if(!mTexture)
        return;

    FTextureFormatSettings textureFormatSettings;
    mTexture->GetLayerFormatSettings(0,textureFormatSettings);
    textureFormatSettings.CompressionNone = mPropertyCompressionNone;
    mTexture->SetLayerFormatSettings(0, textureFormatSettings);

    mTexture->UpdateResource();
}

void
FOdysseyTexture2DWrapper::Finalize()
{
    SetTexture(nullptr);
}

void
FOdysseyTexture2DWrapper::FinalizeSurface()
{
    if (mSurface)
    {
        mSurface->Block()->GetBlock()->SetOnInvalid(::ul3::FOnInvalid());
        delete mSurface;
        mSurface = nullptr;
	}
}

void
FOdysseyTexture2DWrapper::InitializeSurface()
{
    if (!mTexture)
        return;

    mSurface = new FOdysseySurfaceTexture2DEditable(mTexture);
    mSurface->Invalidate();
}

void
FOdysseyTexture2DWrapper::FinalizeLayerStack()
{
    mLayerStack = nullptr;
}

void
FOdysseyTexture2DWrapper::InitializeLayerStack()
{
    if (!mTexture)
        return;

    mLayerStack = FindOrCreateTextureUserData()->GetLayerStack();
}

UOdysseyTextureAssetUserData*
FOdysseyTexture2DWrapper::FindOrCreateTextureUserData() const
{
    UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(mTexture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
    if (userData)
        return userData;

    //Init user data
    ::ul3::tFormat format = ULISFormatForUE4TextureSourceFormat(mTexture->Source.GetFormat());
    userData = NewObject< UOdysseyTextureAssetUserData >(mTexture, NAME_None, RF_Public);
    userData->GetLayerStack()->Init(mTexture->Source.GetSizeX(), mTexture->Source.GetSizeY(), format);
    mTexture->AddAssetUserData( userData );

    //Create image layer
    FOdysseyBlock* textureData = NewOdysseyBlockFromUTextureData( mTexture, userData->GetLayerStack()->Format() );
    FName layerName = userData->GetLayerStack()->GetLayerRoot()->GetNextLayerName();
    TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(layerName, textureData));

    //Add Layer
    userData->GetLayerStack()->AddLayer(imageLayer);

    // Notify for changes
    mTexture->PostEditChange();
    return userData;
}
