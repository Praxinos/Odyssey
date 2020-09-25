// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Proxies/OdysseyBrushFormat.h"

::ul3::tFormat
ULISFormatFromOdysseyBlockFormat(EOdysseyBlockFormat iFormat, ::ul3::tFormat iAutoFormat)
{
	::ul3::tFormat format = iAutoFormat;
	switch (iFormat)
	{
		case EOdysseyBlockFormat::kAuto: format = iAutoFormat; break;
		case EOdysseyBlockFormat::kRGBA8: format = ULIS3_FORMAT_RGBA8; break;
		case EOdysseyBlockFormat::kBGRA8: format = ULIS3_FORMAT_BGRA8; break;
		case EOdysseyBlockFormat::kGrey8: format = ULIS3_FORMAT_G8; break;
		default: format = iAutoFormat; break;
	}
	return format;
}