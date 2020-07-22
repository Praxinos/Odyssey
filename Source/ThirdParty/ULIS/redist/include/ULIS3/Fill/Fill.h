// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Fill.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the Fill entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
// Fill
ULIS3_API void Fill( FThreadPool* iThreadPool
                   , bool iBlocking
                   , uint32 iPerfIntent
                   , const FHostDeviceInfo& iHostDeviceInfo
                   , bool iCallCB
                   , FBlock* iDestination
                   , const IPixel& iColor
                   , const FRect& iArea );

ULIS3_NAMESPACE_END

