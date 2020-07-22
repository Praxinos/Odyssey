// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Methods.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the resampling methods enums.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
// eResamplingMethod
enum eResamplingMethod
{
      INTERP_NN
    , INTERP_BILINEAR
    , INTERP_BICUBIC
    , INTERP_AREA
};

static const char* kwResamplingMethod[] =
{
      "NearestNeighbour"
    , "Bilinear"
    , "Bicubic"
    , "Area"
};

/////////////////////////////////////////////////////
// eMipsLayout
enum eMipsLayout
{
      MPL_STANDARD
    , MPL_MIPSONLY
};

static const char* kwMipsLayout[] =
{
      "Standard"
    , "MipsOnly"
};

ULIS3_NAMESPACE_END

