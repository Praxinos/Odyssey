// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Conv.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the Conv entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS3_NAMESPACE_BEGIN
ULIS3_API void Conv( const IPixel& iSrc, IPixel& iDst );
ULIS3_API FPixelValue Conv( const IPixel& iSrc, tFormat iDst );

ULIS3_API void Conv( FThreadPool*           iThreadPool
                   , bool                   iBlocking
                   , uint32                 iPerfIntent
                   , const FHostDeviceInfo& iHostDeviceInfo
                   , bool                   iCallCB
                   , const FBlock*          iSource
                   , FBlock*                iDestination );

ULIS3_API FBlock* XConv( FThreadPool*           iThreadPool
                       , bool                   iBlocking
                       , uint32                 iPerfIntent
                       , const FHostDeviceInfo& iHostDeviceInfo
                       , bool                   iCallCB
                       , const FBlock*          iSource
                       , tFormat                iDestinationFormat );

ULIS3_NAMESPACE_END

