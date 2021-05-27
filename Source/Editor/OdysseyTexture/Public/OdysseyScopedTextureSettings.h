// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
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
    );

    FOdysseyScopedTextureSettings& operator=( const FOdysseyScopedTextureSettings& ) = delete; // Copy Assignement 

    static
    FOdysseyScopedTextureSettings
    MakeUncompressed( UTexture* iTexture );

private:
    UTexture* mTexture;
    FTextureFormatSettings mBackupTextureFormatSettings;
    TextureCompressionSettings mBackupTextureCompressionSettings;
};

