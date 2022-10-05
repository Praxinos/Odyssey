// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         TextInfo.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FTextInfo class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "String/WString.h"
#include "Font/Font.h"
#include "Image/Color.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FTextInfo
/// @brief      Simple TextInfo class.
struct ULIS_API FTextInfo
{
    FWString string;
    FFont font;
    int size;
    FColor color;
    bool isAntiAliased;
};

ULIS_NAMESPACE_END

