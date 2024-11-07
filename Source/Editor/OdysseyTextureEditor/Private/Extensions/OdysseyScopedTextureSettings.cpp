// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#include "OdysseyScopedTextureSettings.h"
#include "TextureCompiler.h"

FOdysseyScopedTextureSettings::~FOdysseyScopedTextureSettings()
{
    // restoring the initial values
    mTexture->SetLayerFormatSettings( 0, mBackupTextureFormatSettings );
    mTexture->CompressionSettings = mBackupTextureCompressionSettings;
    mTexture->MipGenSettings = mBackupTextureMipGenSettings;
    mTexture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({mTexture});
}

FOdysseyScopedTextureSettings::FOdysseyScopedTextureSettings(
      UTexture* iTexture
    , const FTextureFormatSettings& iTextureFormatSettings
    , TextureCompressionSettings iTextureCompressionSettings
    , TextureMipGenSettings iTextureMipGenSettings
)
    : mTexture( iTexture )
{
    checkf( mTexture, TEXT( "No texture!" ) );
    // backuping the existing settings
    mTexture->GetLayerFormatSettings( 0, mBackupTextureFormatSettings );
    mBackupTextureCompressionSettings = mTexture->CompressionSettings;
    mBackupTextureMipGenSettings = mTexture->MipGenSettings;
    // changing the values
    mTexture->SetLayerFormatSettings( 0, iTextureFormatSettings );
    mTexture->CompressionSettings = iTextureCompressionSettings;
    mTexture->MipGenSettings = iTextureMipGenSettings;
    mTexture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({mTexture});
}

//static
FOdysseyScopedTextureSettings
FOdysseyScopedTextureSettings::MakeUncompressedNoMipMaps( UTexture* iTexture )
{
    FTextureFormatSettings textureFormatSettings;
    iTexture->GetLayerFormatSettings( 0, textureFormatSettings );
    textureFormatSettings.CompressionNone = 1;
    return FOdysseyScopedTextureSettings( iTexture, textureFormatSettings, TC_VectorDisplacementmap, TMGS_NoMipmaps );
}

