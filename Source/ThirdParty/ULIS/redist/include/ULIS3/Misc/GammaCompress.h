// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         GammaCompress.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the GammaCompress entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS3_NAMESPACE_BEGIN

ULIS3_API void ApplysRGB2Linear( FThreadPool*           iThreadPool
                               , bool                   iBlocking
                               , uint32                 iPerfIntent
                               , const FHostDeviceInfo& iHostDeviceInfo
                               , bool                   iCallCB
                               , FBlock*                iDestination );

ULIS3_API void ApplyLinear2sRGB( FThreadPool*           iThreadPool
                               , bool                   iBlocking
                               , uint32                 iPerfIntent
                               , const FHostDeviceInfo& iHostDeviceInfo
                               , bool                   iCallCB
                               , FBlock*                iDestination );

ULIS3_NAMESPACE_END

