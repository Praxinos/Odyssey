// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Proxies/OdysseyBrushFormat.h"

::ULIS::eFormat
ULISFormatFromModelAndDepth(EOdysseyColorModel iColorModel, EOdysseyChannelDepth iChannelDepth)
{
    uint32 format = ::ULIS::Format_BGRA8;
    switch (iColorModel)
    {
        case EOdysseyColorModel::kRGB: format = ::ULIS::Format_RGB8; break;
        case EOdysseyColorModel::kGrey: format = ::ULIS::Format_G8; break;
        case EOdysseyColorModel::kHSL: format = ::ULIS::Format_HSL8; break;
        case EOdysseyColorModel::kHSV: format = ::ULIS::Format_HSV8; break;
        case EOdysseyColorModel::kCMYK: format = ::ULIS::Format_CMYK8; break;
        case EOdysseyColorModel::kLab: format = ::ULIS::Format_Lab8; break;

        case EOdysseyColorModel::kRGBA:
        {
            //Force GBRA8 because RGBA8 is deprecated in ETextureSourceFormat. This will preserve performance most of the time
            format = iChannelDepth == EOdysseyChannelDepth::k8 ? ::ULIS::Format_BGRA8 : ::ULIS::Format_RGBA8;
        }
        break;
        case EOdysseyColorModel::kGreyA: format = ::ULIS::Format_GA8; break;
        case EOdysseyColorModel::kHSLA: format = ::ULIS::Format_HSLA8; break;
        case EOdysseyColorModel::kHSVA: format = ::ULIS::Format_HSVA8; break;
        case EOdysseyColorModel::kCMYKA: format = ::ULIS::Format_CMYKA8; break;
        case EOdysseyColorModel::kLabA: format = ::ULIS::Format_LabA8; break;

        default: break;
    }

    //if 8 bits channel depth, then no work to do
    if (iChannelDepth == EOdysseyChannelDepth::k8)
        return  static_cast< ::ULIS::eFormat >( format );

    //erase
    format &= ULIS_E_TYPE & ULIS_E_DEPTH;

    switch (iChannelDepth)
    {
        case EOdysseyChannelDepth::k16: format |= ULIS_W_TYPE(ULIS_TYPE_UINT16) | ULIS_W_DEPTH(2); break;
        //case EOdysseyChannelDepth::k32: format |= ULIS_W_TYPE(ULIS_TYPE_UINT32) | ULIS_W_DEPTH(4); break;
        case EOdysseyChannelDepth::kFloat: format |= ULIS_W_TYPE(ULIS_TYPE_UFLOAT) | ULIS_W_DEPTH(4); break;
        //case EOdysseyChannelDepth::kDouble: format |= ULIS_W_TYPE(ULIS_TYPE_UDOUBLE) | ULIS_W_DEPTH(8); break;

        default: break;
    }
    return  static_cast< ::ULIS::eFormat >( format );
}

EOdysseyColorModel
OdysseyColorModelFromULISFormat(::ULIS::eFormat iFormat)
{
    uint32 format = iFormat;
    //erase
    format &= ULIS_E_TYPE & ULIS_E_DEPTH & ULIS_E_FLOATING;

    //set 8bits
    format |= ULIS_W_TYPE(ULIS_TYPE_UINT8) | ULIS_W_DEPTH(1);

    //, Format_RGBA8 = ( ULIS_W_TYPE( ULIS_TYPE_UINT8 )  | ULIS_W_CHANNELS( 3 ) | ULIS_W_MODEL( ULIS_ColorModel_RGB ) | ULIS_W_ALPHA( 1 ) | ULIS_W_DEPTH( 1 ) | ULIS_W_PROFILE( ULIS_sRGB ) )
    //, Format_RGBAF = ( ULIS_W_TYPE( ULIS_TYPE_UFLOAT ) | ULIS_W_CHANNELS( 3 ) | ULIS_W_MODEL( ULIS_ColorModel_RGB ) | ULIS_W_ALPHA( 1 ) | ULIS_W_DEPTH( 4 ) | ULIS_W_PROFILE( ULIS_sRGB ) )

    switch (format)
    {
        case ::ULIS::Format_RGB8: return EOdysseyColorModel::kRGB;
        case ::ULIS::Format_G8: return EOdysseyColorModel::kGrey;
        case ::ULIS::Format_HSL8: return EOdysseyColorModel::kHSL;
        case ::ULIS::Format_HSV8: return EOdysseyColorModel::kHSV;
        case ::ULIS::Format_CMYK8: return EOdysseyColorModel::kCMYK;
        case ::ULIS::Format_Lab8: return EOdysseyColorModel::kLab;
        case ::ULIS::Format_BGRA8: return EOdysseyColorModel::kRGBA;
        case ::ULIS::Format_RGBA8: return EOdysseyColorModel::kRGBA;
        case ::ULIS::Format_GA8: return EOdysseyColorModel::kGreyA;
        case ::ULIS::Format_HSLA8: return EOdysseyColorModel::kHSLA;
        case ::ULIS::Format_HSVA8: return EOdysseyColorModel::kHSVA;
        case ::ULIS::Format_CMYKA8: return EOdysseyColorModel::kCMYKA;
        case ::ULIS::Format_LabA8: return EOdysseyColorModel::kLabA;

        default: break;
    }

    return EOdysseyColorModel::kRGB;
}

EOdysseyChannelDepth
OdysseyChannelDepthFromULISFormat(::ULIS::eFormat iFormat)
{
    switch(ULIS_R_TYPE(iFormat))
    {
        case ULIS_TYPE_UINT8: return EOdysseyChannelDepth::k8;
        case ULIS_TYPE_UINT16: return EOdysseyChannelDepth::k16;
        //case ULIS_TYPE_UINT32: return EOdysseyChannelDepth::k32;
        case ULIS_TYPE_UFLOAT: return EOdysseyChannelDepth::kFloat;
        //case ULIS_TYPE_UDOUBLE: return EOdysseyChannelDepth::kDouble;

        default: break;
    }
    
    return EOdysseyChannelDepth::k8;
}