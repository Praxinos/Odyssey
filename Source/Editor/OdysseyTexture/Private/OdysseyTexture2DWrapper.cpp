// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTexture2DWrapper.h"
#include "OdysseySurfaceTexture2DEditable.h"

FOdysseyTexture2DWrapper::~FOdysseyTexture2DWrapper()
{
    Texture(nullptr);
}

FOdysseyTexture2DWrapper::FOdysseyTexture2DWrapper(UTexture2D* iTexture) :
    FOdysseyTextureWrapper<UTexture2D>()
{
    Texture(iTexture);
}

FOdysseyLayerStack*
FOdysseyTexture2DWrapper::LayerStack() const
{
    if (!Texture())
        return nullptr;

    UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(Texture()->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
    if( !userData )
    {
        userData = CreateTextureUserData(Texture());
    }

    return userData->GetLayerStack();
}

void
FOdysseyTexture2DWrapper::UpdateTextureFromSurface()
{
    if(!Texture())
        return;

    CopyBlockDataIntoUTexture( Surface()->Block(), Texture());
    Texture()->UpdateResource();
}

void
FOdysseyTexture2DWrapper::SetTextureProperties()
{
    if(!Texture())
        return;

    FTextureFormatSettings textureFormatSettings;
    Texture()->GetLayerFormatSettings(0, textureFormatSettings);

	// Create new Texture Properties Backup
	mPropertyCompressionNone = textureFormatSettings.CompressionNone;

	// Overwrite Texture properties
    textureFormatSettings.CompressionNone = 1;
    Texture()->SetLayerFormatSettings(0, textureFormatSettings);

    Texture()->UpdateResource();
    Texture()->TemporarilyDisableStreaming(); //needed to be able to draw on previously streamed textures, avoids using NoMipMaps
}

void
FOdysseyTexture2DWrapper::RestoreTextureProperties()
{
    if(!Texture())
        return;

    FTextureFormatSettings textureFormatSettings;
    Texture()->GetLayerFormatSettings(0,textureFormatSettings);
    textureFormatSettings.CompressionNone = mPropertyCompressionNone;
    Texture()->SetLayerFormatSettings(0, textureFormatSettings);

    Texture()->UpdateResource();
}

void
FOdysseyTexture2DWrapper::DestroySurface()
{
    if (Surface())
    {
        Surface()->Block()->GetBlock()->SetOnInvalid(::ul3::FOnInvalid());
        delete Surface();
	}
}

IOdysseySurfaceEditable*
FOdysseyTexture2DWrapper::CreateSurface()
{
    FOdysseySurfaceTexture2DEditable* surface = new FOdysseySurfaceTexture2DEditable(Texture());
    surface->Invalidate();
    return surface;
}

UOdysseyTextureAssetUserData*
FOdysseyTexture2DWrapper::CreateTextureUserData(UTexture2D* iTexture) const
{
    //Init user data
    ::ul3::tFormat format = ULISFormatForUE4TextureSourceFormat(iTexture->Source.GetFormat());
    UOdysseyTextureAssetUserData* userData = NewObject< UOdysseyTextureAssetUserData >(iTexture, NAME_None, RF_Public);
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
    return userData;
}
