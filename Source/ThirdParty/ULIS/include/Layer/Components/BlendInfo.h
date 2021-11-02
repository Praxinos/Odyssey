// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         BlendInfo.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FBlendInfo class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FBlendInfo
/// @brief      Simple POD BlendInfo class.
struct ULIS_API FBlendInfo
{
    eBlendMode blendMode;
    eAlphaMode alphaMode;
    ufloat opacity;
};

ULIS_NAMESPACE_END

