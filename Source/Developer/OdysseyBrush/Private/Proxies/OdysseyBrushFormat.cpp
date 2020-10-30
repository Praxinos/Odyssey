// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Proxies/OdysseyBrushFormat.h"

::ul3::tFormat
ULISFormatFromOdysseyBlockFormat(EOdysseyPixelFormat iFormat, EOdysseyPixelFormatPrecision iPrecision, ::ul3::tFormat iAutoFormat)
{
	if (iFormat == EOdysseyPixelFormat::kAuto && iPrecision == EOdysseyPixelFormatPrecision::kAuto)
		return iAutoFormat;

	::ul3::tFormat format = iAutoFormat;
	switch (iFormat)
	{
		case EOdysseyPixelFormat::kAuto: format = iAutoFormat; break;

		case EOdysseyPixelFormat::kRGB: format = ULIS3_FORMAT_RGB8; break;
		case EOdysseyPixelFormat::kGrey: format = ULIS3_FORMAT_G8; break;
		case EOdysseyPixelFormat::kHSL: format = ULIS3_FORMAT_HSL8; break;
		case EOdysseyPixelFormat::kHSV: format = ULIS3_FORMAT_HSV8; break;
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
		case EOdysseyPixelFormat::kLabA: format = ULIS3_FORMAT_LabA8; break;

		default: break;
	}

	//if 8 bits precision, then no work to do
	if (iPrecision == EOdysseyPixelFormatPrecision::k8)
		return format;

	if (iPrecision == EOdysseyPixelFormatPrecision::kAuto &&
		iAutoFormat & ULIS3_W_TYPE( ULIS3_TYPE_UINT8 ) &&
		iAutoFormat & ULIS3_W_DEPTH( 2 ))
		return format;

	//erase
	format &= ULIS3_E_TYPE & ULIS3_E_DEPTH); //TODO: Remove the last parenthesis once ULIS is patched

	switch (iPrecision)
	{
		case EOdysseyPixelFormatPrecision::kAuto: format |= ULIS3_W_TYPE(ULIS3_R_TYPE(iAutoFormat)) | ULIS3_W_DEPTH(ULIS3_R_DEPTH(iAutoFormat)); break;
		case EOdysseyPixelFormatPrecision::k16: format |= ULIS3_W_TYPE(ULIS3_TYPE_UINT16) | ULIS3_W_DEPTH(2); break;
		case EOdysseyPixelFormatPrecision::k32: format |= ULIS3_W_TYPE(ULIS3_TYPE_UINT32) | ULIS3_W_DEPTH(4); break;
		case EOdysseyPixelFormatPrecision::kFloat: format |= ULIS3_W_TYPE(ULIS3_TYPE_UFLOAT) | ULIS3_W_DEPTH(4); break;
		case EOdysseyPixelFormatPrecision::kDouble: format |= ULIS3_W_TYPE(ULIS3_TYPE_UDOUBLE) | ULIS3_W_DEPTH(8); break;

		default: break;
	}
	return format;
}