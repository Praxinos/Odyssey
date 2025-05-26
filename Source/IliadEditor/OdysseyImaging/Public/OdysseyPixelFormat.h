// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture.h"
#include "ImageCore.h"
#include <ULIS>

//
ODYSSEYIMAGING_API int TextureSourceFormatBytesPerPixel(ETextureSourceFormat iFormat);
ODYSSEYIMAGING_API bool TextureSourceFormatNeedsConversionToULISFormat( ETextureSourceFormat iFormat );
ODYSSEYIMAGING_API void ConvertTextureSourceFormatToULISFormat( const uint8* iSrc, uint8* oDst, int iWidth, int iHeight, ETextureSourceFormat iFormat);
ODYSSEYIMAGING_API bool RawImageFormatNeedsConversionToULISFormat( ERawImageFormat::Type iFormat );
ODYSSEYIMAGING_API void ConvertRawImageFormatToULISFormat( const uint8* iSrc, uint8* oDst, int iWidth, int iHeight, ERawImageFormat::Type iFormat);
ODYSSEYIMAGING_API void ConvertULISFormatToTextureSourceFormat(const uint8* iSrc, uint8* oDst, int iWidth, int iHeight, ETextureSourceFormat iFormat);

ODYSSEYIMAGING_API ::ULIS::eFormat ULISFormatForTextureSourceFormat( ETextureSourceFormat iFormat );
ODYSSEYIMAGING_API ::ULIS::eFormat ULISFormatForPixelFormat( EPixelFormat iFormat );
ODYSSEYIMAGING_API ::ULIS::eFormat ULISFormatForRawImageFormat( ERawImageFormat::Type iFormat );
ODYSSEYIMAGING_API ERawImageFormat::Type RawImageFormatForULISFormat( ::ULIS::eFormat iFormat );
ODYSSEYIMAGING_API ETextureSourceFormat TextureSourceFormatForULISFormat(::ULIS::eFormat iFormat);
ODYSSEYIMAGING_API EPixelFormat PixelFormatForULISFormat( ::ULIS::eFormat iFormat );
