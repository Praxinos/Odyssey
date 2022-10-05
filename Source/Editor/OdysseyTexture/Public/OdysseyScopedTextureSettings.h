// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#pragma once
#include "Engine/Texture.h"


class ODYSSEYTEXTURE_API FOdysseyScopedTextureSettings
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

