// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Proxies/OdysseyBrushFormat.h"

::ul3::tFormat
ULISFormatFromOdysseyPixelFormat(EOdysseyPixelFormat iFormat, EOdysseyPixelFormatPrecision iPrecision, ::ul3::tFormat iAutoFormat)
{
	if (iFormat == EOdysseyPixelFormat::kCanvasFormat && iPrecision == EOdysseyPixelFormatPrecision::kCanvasPrecision)
		return iAutoFormat;

	::ul3::tFormat format = iAutoFormat;
	switch (iFormat)
	{
		case EOdysseyPixelFormat::kCanvasFormat: format = iAutoFormat; break;

		case EOdysseyPixelFormat::kRGB: format = ULIS3_FORMAT_RGB8; break;
		case EOdysseyPixelFormat::kGrey: format = ULIS3_FORMAT_G8; break;
		case EOdysseyPixelFormat::kHSL: format = ULIS3_FORMAT_HSL8; break;
		case EOdysseyPixelFormat::kHSV: format = ULIS3_FORMAT_HSV8; break;
		case EOdysseyPixelFormat::kCMYK: format = ULIS3_FORMAT_CMYK8; break;
		case EOdysseyPixelFormat::kLab: format = ULIS3_FORMAT_Lab8; break;

		case EOdysseyPixelFormat::kRGBA:
		{
			//Force GBRA8 because RGBA8 is deprecated in ETextureSourceFormat. This will preserve performance most of the time
			format = iPrecision == EOdysseyPixelFormatPrecision::k8 ? ULIS3_FORMAT_BGRA8 : ULIS3_FORMAT_RGBA16;
		}
		break;
		case EOdysseyPixelFormat::kGreyA: format = ULIS3_FORMAT_GA8; break;
		case EOdysseyPixelFormat::kHSLA: format = ULIS3_FORMAT_HSLA8; break;
		case EOdysseyPixelFormat::kHSVA: format = ULIS3_FORMAT_HSVA8; break;
		case EOdysseyPixelFormat::kCMYKA: format = ULIS3_FORMAT_CMYKA8; break;
		case EOdysseyPixelFormat::kLabA: format = ULIS3_FORMAT_LabA8; break;

		default: break;
	}

	//if 8 bits precision, then no work to do
	if (iPrecision == EOdysseyPixelFormatPrecision::k8)
		return format;

	if (iPrecision == EOdysseyPixelFormatPrecision::kCanvasPrecision &&
		iAutoFormat & ULIS3_W_TYPE( ULIS3_TYPE_UINT8 ) &&
		iAutoFormat & ULIS3_W_DEPTH( 2 ))
		return format;

	//erase
	format &= ULIS3_E_TYPE & ULIS3_E_DEPTH); //TODO: Remove the last parenthesis once ULIS is patched

	switch (iPrecision)
	{
		case EOdysseyPixelFormatPrecision::kCanvasPrecision: format |= ULIS3_W_TYPE(ULIS3_R_TYPE(iAutoFormat)) | ULIS3_W_DEPTH(ULIS3_R_DEPTH(iAutoFormat)); break;
		case EOdysseyPixelFormatPrecision::k16: format |= ULIS3_W_TYPE(ULIS3_TYPE_UINT16) | ULIS3_W_DEPTH(2); break;
		case EOdysseyPixelFormatPrecision::k32: format |= ULIS3_W_TYPE(ULIS3_TYPE_UINT32) | ULIS3_W_DEPTH(4); break;
		case EOdysseyPixelFormatPrecision::kFloat: format |= ULIS3_W_TYPE(ULIS3_TYPE_UFLOAT) | ULIS3_W_DEPTH(4); break;
		case EOdysseyPixelFormatPrecision::kDouble: format |= ULIS3_W_TYPE(ULIS3_TYPE_UDOUBLE) | ULIS3_W_DEPTH(8); break;

		default: break;
	}
	return format;
}

EOdysseyPixelFormat
OdysseyPixelFormatFromULISFormat(::ul3::tFormat iFormat)
{
	//erase
	iFormat &= ULIS3_E_TYPE & ULIS3_E_DEPTH); //TODO: Remove the last parenthesis once ULIS is patched

	//set 8bits
	iFormat |= ULIS3_W_TYPE(ULIS3_TYPE_UINT8) | ULIS3_W_DEPTH(1);

	switch (iFormat)
	{
		case ULIS3_FORMAT_RGB8: return EOdysseyPixelFormat::kRGB;
		case ULIS3_FORMAT_G8: return EOdysseyPixelFormat::kGrey;
		case ULIS3_FORMAT_HSL8: return EOdysseyPixelFormat::kHSL;
		case ULIS3_FORMAT_HSV8: return EOdysseyPixelFormat::kHSV;
		case ULIS3_FORMAT_CMYK8: return EOdysseyPixelFormat::kCMYK;
		case ULIS3_FORMAT_Lab8: return EOdysseyPixelFormat::kLab;
		case ULIS3_FORMAT_BGRA8: return EOdysseyPixelFormat::kRGBA;
		case ULIS3_FORMAT_RGBA16: return EOdysseyPixelFormat::kRGBA;
		case ULIS3_FORMAT_GA8: return EOdysseyPixelFormat::kGreyA;
		case ULIS3_FORMAT_HSLA8: return EOdysseyPixelFormat::kHSLA;
		case ULIS3_FORMAT_HSVA8: return EOdysseyPixelFormat::kHSVA;
		case ULIS3_FORMAT_CMYKA8: return EOdysseyPixelFormat::kCMYKA;
		case ULIS3_FORMAT_LabA8: return EOdysseyPixelFormat::kLabA;

		default: break;
	}

	return EOdysseyPixelFormat::kRGB;
}

EOdysseyPixelFormatPrecision
OdysseyPixelFormatPrecisionFromULISFormat(::ul3::tFormat iFormat)
{
	switch(ULIS3_R_TYPE(iFormat))
	{
		case ULIS3_TYPE_UINT8: return EOdysseyPixelFormatPrecision::k8;
		case ULIS3_TYPE_UINT16: return EOdysseyPixelFormatPrecision::k16;
		case ULIS3_TYPE_UINT32: return EOdysseyPixelFormatPrecision::k32;
		case ULIS3_TYPE_UFLOAT: return EOdysseyPixelFormatPrecision::kFloat;
		case ULIS3_TYPE_UDOUBLE: return EOdysseyPixelFormatPrecision::kDouble;

		default: break;
	}
	
	return EOdysseyPixelFormatPrecision::k8;
}