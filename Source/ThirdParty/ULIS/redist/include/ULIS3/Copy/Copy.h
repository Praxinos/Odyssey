// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Copy.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the Copy entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS3_NAMESPACE_BEGIN

ULIS3_API void Copy( FThreadPool*           iThreadPool
                   , bool                   iBlocking
                   , uint32                 iPerfIntent
                   , const FHostDeviceInfo& iHostDeviceInfo
                   , bool                   iCallCB
                   , const FBlock*          iSource
                   , FBlock*                iDestination
                   , const FRect&           iArea
                   , const FVec2I&          iPos );

ULIS3_API FBlock* XCopy( FThreadPool*           iThreadPool
                       , bool                   iBlocking
                       , uint32                 iPerfIntent
                       , const FHostDeviceInfo& iHostDeviceInfo
                       , bool                   iCallCB
                       , const FBlock*          iSource
                       , const FRect&           iArea );

ULIS3_API void CopyRaw( const FBlock* iSrc, FBlock* iDst, bool iCallCB );
ULIS3_API FBlock* XCopyRaw( const FBlock* iSrc, bool iCallCB );

ULIS3_NAMESPACE_END

