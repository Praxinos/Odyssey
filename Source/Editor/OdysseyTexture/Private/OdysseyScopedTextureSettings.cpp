// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#include "OdysseyScopedTextureSettings.h"

FOdysseyScopedTextureSettings::~FOdysseyScopedTextureSettings()
{
    // restoring the initial values
    mTexture->SetLayerFormatSettings( 0, mBackupTextureFormatSettings );
    mTexture->CompressionSettings = mBackupTextureCompressionSettings;
    mTexture->UpdateResource();
}

FOdysseyScopedTextureSettings::FOdysseyScopedTextureSettings(
      UTexture* iTexture
    , const FTextureFormatSettings& iTextureFormatSettings
    , TextureCompressionSettings iTextureCompressionSettings
)
    : mTexture( iTexture )
{
    checkf( mTexture, TEXT( "No texture!" ) );
    // backuping the existing settings
    mTexture->GetLayerFormatSettings( 0, mBackupTextureFormatSettings );
    mBackupTextureCompressionSettings = mTexture->CompressionSettings;
    // changing the values
    mTexture->SetLayerFormatSettings( 0, iTextureFormatSettings );
    mTexture->CompressionSettings = iTextureCompressionSettings;
    mTexture->UpdateResource();
}

//static
FOdysseyScopedTextureSettings
FOdysseyScopedTextureSettings::MakeUncompressed( UTexture* iTexture )
{
    FTextureFormatSettings textureFormatSettings;
    iTexture->GetLayerFormatSettings( 0, textureFormatSettings );
    textureFormatSettings.CompressionNone = 1;
    return FOdysseyScopedTextureSettings( iTexture, textureFormatSettings, TC_VectorDisplacementmap );
}

