// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Swap.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the Swap entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS3_NAMESPACE_BEGIN

ULIS3_API void Swap( FThreadPool*           iThreadPool
                   , bool                   iBlocking
                   , uint32                 iPerfIntent
                   , const FHostDeviceInfo& iHostDeviceInfo
                   , bool                   iCallCB
                   , FBlock*                iDestination
                   , uint8                  iChannel1
                   , uint8                  iChannel2 );

ULIS3_NAMESPACE_END

