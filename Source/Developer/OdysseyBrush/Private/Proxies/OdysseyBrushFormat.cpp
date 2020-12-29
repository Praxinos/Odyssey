// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Proxies/OdysseyBrushFormat.h"

::ul3::tFormat
ULISFormatFromModelAndDepth(EOdysseyColorModel iColorModel, EOdysseyChannelDepth iChannelDepth)
{
	::ul3::tFormat format = ULIS3_FORMAT_BGRA8;
	switch (iColorModel)
	{
		case EOdysseyColorModel::kRGB: format = ULIS3_FORMAT_RGB8; break;
		case EOdysseyColorModel::kGrey: format = ULIS3_FORMAT_G8; break;
		case EOdysseyColorModel::kHSL: format = ULIS3_FORMAT_HSL8; break;
		case EOdysseyColorModel::kHSV: format = ULIS3_FORMAT_HSV8; break;
		case EOdysseyColorModel::kCMYK: format = ULIS3_FORMAT_CMYK8; break;
		case EOdysseyColorModel::kLab: format = ULIS3_FORMAT_Lab8; break;

		case EOdysseyColorModel::kRGBA:
		{
			//Force GBRA8 because RGBA8 is deprecated in ETextureSourceFormat. This will preserve performance most of the time
			format = iChannelDepth == EOdysseyChannelDepth::k8 ? ULIS3_FORMAT_BGRA8 : ULIS3_FORMAT_RGBA8;
		}
		break;
		case EOdysseyColorModel::kGreyA: format = ULIS3_FORMAT_GA8; break;
		case EOdysseyColorModel::kHSLA: format = ULIS3_FORMAT_HSLA8; break;
		case EOdysseyColorModel::kHSVA: format = ULIS3_FORMAT_HSVA8; break;
		case EOdysseyColorModel::kCMYKA: format = ULIS3_FORMAT_CMYKA8; break;
		case EOdysseyColorModel::kLabA: format = ULIS3_FORMAT_LabA8; break;

		default: break;
	}

	//if 8 bits channel depth, then no work to do
	if (iChannelDepth == EOdysseyChannelDepth::k8)
		return format;

	//erase
	format &= ULIS3_E_TYPE & ULIS3_E_DEPTH;

	switch (iChannelDepth)
	{
		case EOdysseyChannelDepth::k16: format |= ULIS3_W_TYPE(ULIS3_TYPE_UINT16) | ULIS3_W_DEPTH(2); break;
		case EOdysseyChannelDepth::k32: format |= ULIS3_W_TYPE(ULIS3_TYPE_UINT32) | ULIS3_W_DEPTH(4); break;
		case EOdysseyChannelDepth::kFloat: format |= ULIS3_W_TYPE(ULIS3_TYPE_UFLOAT) | ULIS3_W_DEPTH(4); break;
		case EOdysseyChannelDepth::kDouble: format |= ULIS3_W_TYPE(ULIS3_TYPE_UDOUBLE) | ULIS3_W_DEPTH(8); break;

		default: break;
	}
	return format;
}

EOdysseyColorModel
OdysseyColorModelFromULISFormat(::ul3::tFormat iFormat)
{
	//erase
	iFormat &= ULIS3_E_TYPE & ULIS3_E_DEPTH;

	//set 8bits
	iFormat |= ULIS3_W_TYPE(ULIS3_TYPE_UINT8) | ULIS3_W_DEPTH(1);

	switch (iFormat)
	{
		case ULIS3_FORMAT_RGB8: return EOdysseyColorModel::kRGB;
		case ULIS3_FORMAT_G8: return EOdysseyColorModel::kGrey;
		case ULIS3_FORMAT_HSL8: return EOdysseyColorModel::kHSL;
		case ULIS3_FORMAT_HSV8: return EOdysseyColorModel::kHSV;
		case ULIS3_FORMAT_CMYK8: return EOdysseyColorModel::kCMYK;
		case ULIS3_FORMAT_Lab8: return EOdysseyColorModel::kLab;
		case ULIS3_FORMAT_BGRA8: return EOdysseyColorModel::kRGBA;
		case ULIS3_FORMAT_RGBA8: return EOdysseyColorModel::kRGBA;
		case ULIS3_FORMAT_GA8: return EOdysseyColorModel::kGreyA;
		case ULIS3_FORMAT_HSLA8: return EOdysseyColorModel::kHSLA;
		case ULIS3_FORMAT_HSVA8: return EOdysseyColorModel::kHSVA;
		case ULIS3_FORMAT_CMYKA8: return EOdysseyColorModel::kCMYKA;
		case ULIS3_FORMAT_LabA8: return EOdysseyColorModel::kLabA;

		default: break;
	}

	return EOdysseyColorModel::kRGB;
}

EOdysseyChannelDepth
OdysseyChannelDepthFromULISFormat(::ul3::tFormat iFormat)
{
	switch(ULIS3_R_TYPE(iFormat))
	{
		case ULIS3_TYPE_UINT8: return EOdysseyChannelDepth::k8;
		case ULIS3_TYPE_UINT16: return EOdysseyChannelDepth::k16;
		case ULIS3_TYPE_UINT32: return EOdysseyChannelDepth::k32;
		case ULIS3_TYPE_UFLOAT: return EOdysseyChannelDepth::kFloat;
		case ULIS3_TYPE_UDOUBLE: return EOdysseyChannelDepth::kDouble;

		default: break;
	}
	
	return EOdysseyChannelDepth::k8;
}