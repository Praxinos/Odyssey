// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Filter.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the Filter entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include <functional>

ULIS3_NAMESPACE_BEGIN

typedef void (*fpFilterOPFunc)( const FBlock* iBlock, const tByte* iPtr );
typedef void (*fpFilterOPInPlaceFunc)( FBlock* iBlock, tByte* iPtr );
typedef void (*fpFilterOPInto)( const FBlock* iSrcBlock, const tByte* iSrcPtr, FBlock* iDstBlock, tByte* iDstPtr );

ULIS3_API void Filter( FThreadPool*             iThreadPool
                     , bool                     iBlocking
                     , uint32                   iPerfIntent
                     , const FHostDeviceInfo&   iHostDeviceInfo
                     , bool                     iCallCB
                     , const FBlock*            iSource
                     , std::function< void( const FBlock* iBlock, const tByte* iPtr ) > iFunc );

ULIS3_API void FilterInPlace( FThreadPool*              iThreadPool
                            , bool                      iBlocking
                            , uint32                    iPerfIntent
                            , const FHostDeviceInfo&    iHostDeviceInfo
                            , bool                      iCallCB
                            , FBlock*                   iSource
                            , std::function< void( FBlock* iBlock, tByte* iPtr ) > iFunc );

ULIS3_API void FilterInto( FThreadPool*             iThreadPool
                         , bool                     iBlocking
                         , uint32                   iPerfIntent
                         , const FHostDeviceInfo&   iHostDeviceInfo
                         , bool                     iCallCB
                         , const FBlock*            iSource
                         , FBlock*                  iDestination
                         , std::function< void( const FBlock* iSrcBlock, const tByte* iSrcPtr, FBlock* iDstBlock, tByte* iDstPtr ) > iFunc );
ULIS3_NAMESPACE_END

