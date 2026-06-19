// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once
#include "Engine/Texture.h"


class FOdysseyScopedTextureSettings
{
public:
    ~FOdysseyScopedTextureSettings();
    FOdysseyScopedTextureSettings(
          UTexture* iTexture
        , const FTextureFormatSettings& iTextureFormatSettings
        , TextureCompressionSettings iTextureCompressionSettings
        , TextureMipGenSettings iBackupTextureMipGenSettings
    );

    FOdysseyScopedTextureSettings& operator=( const FOdysseyScopedTextureSettings& ) = delete; // Copy Assignement

    static
    FOdysseyScopedTextureSettings
    MakeUncompressedNoMipMaps( UTexture* iTexture );

private:
    UTexture* mTexture;
    FTextureFormatSettings mBackupTextureFormatSettings;
    TextureCompressionSettings mBackupTextureCompressionSettings;
    TextureMipGenSettings mBackupTextureMipGenSettings;
};
