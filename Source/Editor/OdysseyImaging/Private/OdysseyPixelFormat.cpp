// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPixelFormat.h"

/////////////////////////////////////////////////////
// Utlity

int TextureSourceFormatBytesPerPixel(ETextureSourceFormat iFormat)
{
    switch (iFormat) {
        case TSF_G8:        return 1;
        case TSF_G16:       return 2;
        case TSF_BGRA8:     return 4;
        case TSF_BGRE8:     return 4;
        case TSF_RGBA16:    return 8;
        case TSF_RGBA16F:   return 8;
        case TSF_RGBA8:     return 4;
        case TSF_RGBE8:     return 4;
        case TSF_Invalid:
        case TSF_MAX:
        default: break;
    }
    return 0;
}

bool TextureSourceFormatNeedsConversionToULISFormat( ETextureSourceFormat iFormat )
{
    switch( iFormat )
    {
        case TSF_BGRE8:
        case TSF_RGBA16F:
        case TSF_RGBE8:
            return true;
        default:
            break;
    }
    return false;
}

void
ConvertTextureSourceFormatToULISFormat( const uint8* iSrc, uint8* oDst, int iWidth, int iHeight, ETextureSourceFormat iFormat)
{
    switch( iFormat )
    {
        case TSF_BGRE8:
        {
            FColor* bgre = (FColor*)(iSrc);
            ::ULIS::FBlock* dst = new ::ULIS::FBlock(oDst, iWidth, iHeight, ::ULIS::Format_RGBF);
            ::ULIS::FPixel proxy(oDst, ::ULIS::Format_RGBF);
            for (int y = 0; y < iHeight; y++)
            {
                for (int x = 0; x < iWidth; x++)
                {
                    int i = y * iWidth + x;
                    proxy.SetPointer(dst->PixelBits(x, y));
                    FLinearColor rgba = bgre[i].FromRGBE();
                    proxy.SetRF(rgba.R);
                    proxy.SetGF(rgba.G);
                    proxy.SetBF(rgba.B);
                }
            }
            delete dst;
        }
        break;

        case TSF_RGBE8:
        {
            FColor* rgbe = (FColor*)(iSrc);
            ::ULIS::FBlock* dst = new ::ULIS::FBlock(oDst, iWidth, iHeight, ::ULIS::Format_RGBF);
            ::ULIS::FPixel proxy(oDst, ::ULIS::Format_RGBF);
            for (int y = 0; y < iHeight; y++)
            {
                for (int x = 0; x < iWidth; x++)
                {
                    int i = y * iWidth + x;
                    FColor bgre(rgbe[i].B, rgbe[i].G, rgbe[i].R, rgbe[i].A);
                    proxy.SetPointer(dst->PixelBits(x, y));
                    FLinearColor rgba = bgre.FromRGBE();
                    proxy.SetRF(rgba.R);
                    proxy.SetGF(rgba.G);
                    proxy.SetBF(rgba.B);
                }
            }
            delete dst;
        }
        break;

        case TSF_RGBA16F:
        {
            FFloat16Color* rgba16 = (FFloat16Color*)(iSrc);
            FLinearColor* rgbaf = (FLinearColor*)(oDst);
            int size = iWidth * iHeight;
            for (int i = 0; i < size; i++)
            {
                rgbaf[i] = FLinearColor(rgba16[i]);
            }
        }
        break;
        
        default:
        break;
    }
}

void
ConvertULISFormatToTextureSourceFormat( const uint8* iSrc, uint8* oDst, int iWidth, int iHeight, ETextureSourceFormat iFormat)
{
    switch( iFormat )
    {
        case TSF_BGRE8:
        {
            ::ULIS::FBlock* src = new ::ULIS::FBlock((uint8*)iSrc, iWidth, iHeight, ::ULIS::Format_RGBF);
            ::ULIS::FPixel proxy(iSrc, ::ULIS::Format_RGBF);
            FColor* bgre = (FColor*)(oDst);
            for (int y = 0; y < iHeight; y++)
            {
                for (int x = 0; x < iWidth; x++)
                {
                    int i = y * iWidth + x;
                    proxy.SetPointer(src->PixelBits(x, y));
                    FLinearColor rgba(proxy.RF(), proxy.GF(), proxy.BF(), 1.0f);
                    bgre[i] = rgba.ToRGBE();
                }
            }
            delete src;
        }
        break;

        case TSF_RGBE8:
        {
            ::ULIS::FBlock* src = new ::ULIS::FBlock((uint8*)iSrc, iWidth, iHeight, ::ULIS::Format_RGBF);
            ::ULIS::FPixel proxy(iSrc, ::ULIS::Format_RGBF);
            FColor* rgbe = (FColor*)(oDst);
            for (int y = 0; y < iHeight; y++)
            {
                for (int x = 0; x < iWidth; x++)
                {
                    int i = y * iWidth + x;
                    proxy.SetPointer(src->PixelBits(x, y));
                    FLinearColor rgba(proxy.RF(), proxy.GF(), proxy.BF(), 1.0f);
                    FColor bgre = rgba.ToRGBE();
                    rgbe[i] = FColor(bgre.B, bgre.G, bgre.R, bgre.A);
                }
            }
            delete src;
        }
        break;

        case TSF_RGBA16F:
        {
            FLinearColor* rgbaf = (FLinearColor*)(iSrc);
            FFloat16Color* rgba16 = (FFloat16Color*)(oDst);
            int size = iWidth * iHeight;
            for (int i = 0; i < size; i++)
            {
                rgba16[i] = FFloat16Color(rgbaf[i]);
            }
        }
        break;
        
        default:
        break;
    }
}

::ULIS::eFormat ULISFormatForTextureSourceFormat( ETextureSourceFormat iFormat )
{
    uint32 ret = 0;
    switch( iFormat ) {
        case TSF_Invalid:   ret = 0;                        break;
        case TSF_G8:        ret = ::ULIS::Format_G8;        break;
        case TSF_G16:       ret = ::ULIS::Format_G16;       break;
        case TSF_BGRA8:     ret = ::ULIS::Format_BGRA8;     break;
        case TSF_BGRE8:     ret = ::ULIS::Format_RGBF;      break;
        case TSF_RGBA16:    ret = ::ULIS::Format_RGBA16;    break;
        case TSF_RGBA16F:   ret = ::ULIS::Format_RGBAF;     break; //TODO: Change to RGBA16G ULIS FORMAT (RGBA half floating points 16 bits, see Unreal implementation)
        case TSF_RGBA8:     ret = ::ULIS::Format_RGBA8;     break;
        case TSF_RGBE8:     ret = ::ULIS::Format_RGBF;      break;
        case TSF_MAX:       ret = 0;                        break;
        default:            ret = 0;                        break;
    }
    checkf( ret, TEXT( "Error, bad format !" ) ); // Crash
    return static_cast< ::ULIS::eFormat >( ret );
}

::ULIS::eFormat ULISFormatForPixelFormat( EPixelFormat iFormat )
{
    uint32 ret = 0;
    switch( iFormat ) {
        case PF_Unknown:            ret = ::ULIS::Format_BGRA8;     break;
        case PF_A8:                 ret = ::ULIS::Format_G8;        break;
        case PF_G8:                 ret = ::ULIS::Format_G8;        break;
        case PF_R8_UINT:            ret = ::ULIS::Format_G8;        break;
        case PF_L8:                 ret = ::ULIS::Format_G8;        break;
        case PF_G16:                ret = ::ULIS::Format_G16;       break;
        case PF_A8R8G8B8:           ret = ::ULIS::Format_ARGB8;     break;
        case PF_B8G8R8A8:           ret = ::ULIS::Format_BGRA8;     break;
        case PF_A32B32G32R32F:      ret = ::ULIS::Format_ABGRF;     break;
        //case PF_R32G32B32A32_UINT:  ret = ::ULIS::Format_RGBA32;    break; // Disabled 32bit support
        case PF_R16G16B16A16_UINT:  ret = ::ULIS::Format_RGBA16;    break;
        case PF_R16_UINT:           ret = ::ULIS::Format_G16;       break;
        //case PF_R32_UINT:           ret = ::ULIS::Format_G32;       break; // Disabled 32bit support
        case PF_R8G8B8A8_UINT:      ret = ::ULIS::Format_RGBA8;     break;
        case PF_R16G16B16A16_UNORM: ret = ::ULIS::Format_RGBA16;    break;
        default:                    ret = 0;                        break;
    }
    // checkf( ret, TEXT( "Error, bad format !" ) ); // Crash
    return  static_cast< ::ULIS::eFormat >( ret );
}

ETextureSourceFormat TextureSourceFormatForULISFormat( ::ULIS::eFormat iFormat )
{
    ETextureSourceFormat ret = TSF_Invalid;
    switch(iFormat) {
        case ::ULIS::Format_G8:     ret = TSF_G8;       break;
        case ::ULIS::Format_G16:    ret = TSF_G16;      break;
        case ::ULIS::Format_BGRA8:  ret = TSF_BGRA8;    break;
        case ::ULIS::Format_RGBA8:  ret = TSF_RGBA8;    break;
        case ::ULIS::Format_RGBA16: ret = TSF_RGBA16;   break;
        case ::ULIS::Format_RGBAF:  ret = TSF_RGBA16F;  break;
        default:                    ret = TSF_Invalid;  break;
    }
    checkf(ret,TEXT("Error, bad format !")); // Crash
    return ret;
}

EPixelFormat PixelFormatForULISFormat( ::ULIS::eFormat iFormat )
{
    EPixelFormat ret = PF_Unknown;
    switch( iFormat ) {
        case ::ULIS::Format_G8:     ret = PF_G8;                    break;
        case ::ULIS::Format_G16:    ret = PF_G16;                   break;
        case ::ULIS::Format_ARGB8:  ret = PF_A8R8G8B8;              break;
        case ::ULIS::Format_BGRA8:  ret = PF_B8G8R8A8;              break;
        case ::ULIS::Format_ABGRF:  ret = PF_A32B32G32R32F;         break;
        //case ::ULIS::Format_RGBA32: ret = PF_R32G32B32A32_UINT;     break; // Disabled 32bit support
        case ::ULIS::Format_RGBA16: ret = PF_R16G16B16A16_UINT;     break;
        case ::ULIS::Format_RGBA8:  ret = PF_R8G8B8A8_UINT;         break;
        default:                    ret = PF_Unknown;               break;
    }
    //checkf( ret, TEXT( "Error, bad format !" ) ); // Crash
    return ret;
}
