// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         TimeSettings.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FTimeSettings class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FTimeSettings
/// @brief      Simple TimeSettings class.
struct ULIS_API FTimeSettings
{
    ufloat fps;
    int64 beginFrame;
    int64 endFrame;
    int64 currentFrame;
};

ULIS_NAMESPACE_END

